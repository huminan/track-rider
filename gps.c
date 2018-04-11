#include "sys.h"

char latitude[LATITUDE];
char longitude[LONGITUDE];

bit start_cnt = 0;
bit start_record = 0;
uchar done_record = 0;	// 0: haven't record
												// 1: record done
												// 2: error

char rcv_gps[MAXRCV];
uchar GpsRcvCnt = 0;

/************串行口1 GPS中断处理函数*************/
void UART_1Interrupt(void) interrupt 4
{
		char tmp = 0;
		P0_1 = 1;
        if(RI)
        {
            ES=0;	//关闭串行口中断
						RI=0;	//接收中断信号清零，继续接收
            tmp=SBUF;
// 2nd
						if(start_cnt) // 开始判断是否是GPGLL指令
						{
								GpsRcvCnt++;
								if(GpsRcvCnt == 6)
								{		
										if(tmp == 'L')		 // 若是GPGLL指令，就开始记录
										{
												start_record = 1;
												start_cnt = 0;
												GpsRcvCnt = 0;
										}
										else {						 // 若不是GPGLL指令
												start_cnt = 0; // 等待下一个'$'出现
												GpsRcvCnt = 0;
											
												// recive wrong
												// 亮红灯
												P0_1 = 1;
										}
								}
						}
// 1st
						else if(tmp == '$')
						{
								if(!start_record) {		// 若是第一次见到'$'，开始数数
										start_cnt = 1;
										GpsRcvCnt++;
								}
								else {								// 若不是第一次见到'$'，说明已经读完数据
										done_record = 1;
										start_record = 0;
										// GpsRcvCnt在外面进行判断是否有效数据，并清零
								}
						}
// 3rd
						if(start_record)	//开始记录数据
						{
								rcv_gps[GpsRcvCnt] = tmp;
								GpsRcvCnt++;
								
								if(GpsRcvCnt > MAXRCV)	// too much info
								{
										GpsRcvCnt = 0;
										start_record = 0;
										done_record = 2;		// error
								}
						}
						ES=1;
        }
}


void Record_Gps()
{
		P0_1 = 0;
		P0_3 = 0;
		REN = 1;	//允许接收数据
		ES = 1;		//打开串口1接收中断
		while(!done_record);		// 一直等待接收中断函数中处理完数据
		P0_1 = 0;	// test
		REN = 0;  //禁止接收数据
		ES = 0;		//关闭串口1接收中断
		if(done_record == 1)
		{
				if(GpsRcvCnt < 20) // 说明没有信号
				{
						// 亮黄灯
						P0_3 = 1;
				}
				else		//有信号，进行拆解
				{
						uchar posx1,posx2;
						// double lati, longi;
						if(Locate_Pos(rcv_gps, 1)==2)	// 第2个一定是逗号
						{
								posx1 = 2;
								posx2 = Locate_Pos(rcv_gps, 2);
								mid(latitude,rcv_gps, posx2-posx1-1,posx1);	// 将第一个逗号和第二个逗号之间的值给lati数组
							
								posx1 = Locate_Pos(rcv_gps, 3);
								posx2 = Locate_Pos(rcv_gps, 4);
								mid(longitude,rcv_gps, posx2-posx1-1,posx1);// 将第三个逗号和第四个逗号之间的值给longi数组
								
								//向服务器发送数据
								Uart2SendStr("AT+CIPSEND\r");
								DelaySec(1);
							
								//格式："A2,1255.12001,12322.54000"
								UART_2SendOneByte('A');
								UART_2SendOneByte(DEVICEID);
								UART_2SendOneByte(',');

								Uart2SendStr(latitude);
								UART_2SendOneByte(',');
								Uart2SendStr(longitude);

								CleanSimRcv();//清除接收缓存
							
						}
						else
						{
								// recive wrong
								// 亮红灯
								P0_1 = 1;
						}
				}
		}
		else if(done_record == 2)
		{
				// something wrong
				// 亮红灯
				// P0_1 = 1;
		}
		else{		//done_record = 0
				// something wrong
				// 亮红灯
				// P0_1 = 1;
		}
		done_record = 0;
		CleanGpsRcv();// 清除接收缓存
}

/*从字符串的中间截取n个字符*/
char * mid(char *dst,char *src, int n,int m) /*n为长度，m为位置*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*从第m个到最后*/
    if(m<0) m=0;    /*从第一个开始*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗？很有必要*/
    return dst;
}

//从buf里面得到第cx个逗号所在的位置，从1开始算
//返回值:1~0XFE,代表逗号所在位置的偏移，也从1开始算
//       0XFF,代表不存在第cx个逗号							  
char Locate_Pos(char *buf,char cx)
{	 		    
	char *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}


/***************************GPRS接收缓冲数据初始化函数***************/
/*函数原型:void CleanGsmRcv(void)
/*函数功能:将GsmRcv[]数据清除，并且使缓冲指针移到第一位。
/*输入参数:无
/*输出参数:无
/********************************************************************/
void CleanGpsRcv(void)
{
	uchar i = 0;
	for(i=0; i<=MAXRCV; i++)
	{
		rcv_gps[i] = 0;	
	}

	GpsRcvCnt = 0;	
}






char Send_ATComm(const char *comm, const char *ret, uchar delay, bit wait)
{
    uchar i;
    for(i = 0;i<delay;i++)
    {
        if(!wait)
            Uart2SendStr(comm); 
        DelaySec(1);
        if(Hand(ret)) //Match
        {
            CleanGpsRcv();
            return 1;
        }
    }
    CleanGpsRcv();
    return 0;
}
