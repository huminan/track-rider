#include "sys.h"

char rcv_sim[MAXRCV];
uchar SimRcvCnt = 0;

/************串行口2 SIM中断处理函数*************/
void UART_2Interrupt(void) interrupt 8
{
				char tmp;
//				P0_1 = 1;		// test
				IE2 = 0x00;		//关闭串口2中断//ES2 = 0;
	
// 串口2接受到了数据
        if(S2RI)
        {
                
                S2CON &= ~S2RI;
                tmp = S2BUF;
								
								rcv_sim[SimRcvCnt] = tmp;	//把这次的数据装入数组等检验
								SimRcvCnt++;
					
								if(SimRcvCnt > MAXRCV)
								{
										SimRcvCnt = 0;
								}
								
        } 
				
				IE2 = 0x01;				//打开串口2中断 ES2 = 1;
}

/***************************GPRS接收缓冲数据初始化函数***************/
/*函数原型:void CleanGsmRcv(void)
/*函数功能:将GsmRcv[]数据清除，并且使缓冲指针移到第一位。
/*输入参数:无
/*输出参数:无
/********************************************************************/
void CleanSimRcv(void)
{
	uchar i = 0;
	for(i=0; i<=MAXRCV; i++)
	{
		rcv_sim[i] = 0;	
	}

	SimRcvCnt = 0;	
}

/***************************GPRS缓冲数据握手函数***************/
/*函数原型:bit Hand(unsigned char *a)
/*函数功能:判断收到的数据是否出现判断的数据。
/*输入参数:判断出现的数据
/*输出参数:	1:出现了判断的数据
			0:没有出现判断的数据
/********************************************************************/
bit Hand(unsigned char *a)
{ 
	if(strstr(rcv_sim,a)!=NULL)
		return 1;
	else
		return 0;
}