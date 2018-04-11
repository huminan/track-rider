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

/************���п�1 GPS�жϴ�����*************/
void UART_1Interrupt(void) interrupt 4
{
		char tmp = 0;
		P0_1 = 1;
        if(RI)
        {
            ES=0;	//�رմ��п��ж�
						RI=0;	//�����ж��ź����㣬��������
            tmp=SBUF;
// 2nd
						if(start_cnt) // ��ʼ�ж��Ƿ���GPGLLָ��
						{
								GpsRcvCnt++;
								if(GpsRcvCnt == 6)
								{		
										if(tmp == 'L')		 // ����GPGLLָ��Ϳ�ʼ��¼
										{
												start_record = 1;
												start_cnt = 0;
												GpsRcvCnt = 0;
										}
										else {						 // ������GPGLLָ��
												start_cnt = 0; // �ȴ���һ��'$'����
												GpsRcvCnt = 0;
											
												// recive wrong
												// �����
												P0_1 = 1;
										}
								}
						}
// 1st
						else if(tmp == '$')
						{
								if(!start_record) {		// ���ǵ�һ�μ���'$'����ʼ����
										start_cnt = 1;
										GpsRcvCnt++;
								}
								else {								// �����ǵ�һ�μ���'$'��˵���Ѿ���������
										done_record = 1;
										start_record = 0;
										// GpsRcvCnt����������ж��Ƿ���Ч���ݣ�������
								}
						}
// 3rd
						if(start_record)	//��ʼ��¼����
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
		REN = 1;	//�����������
		ES = 1;		//�򿪴���1�����ж�
		while(!done_record);		// һֱ�ȴ������жϺ����д���������
		P0_1 = 0;	// test
		REN = 0;  //��ֹ��������
		ES = 0;		//�رմ���1�����ж�
		if(done_record == 1)
		{
				if(GpsRcvCnt < 20) // ˵��û���ź�
				{
						// ���Ƶ�
						P0_3 = 1;
				}
				else		//���źţ����в��
				{
						uchar posx1,posx2;
						// double lati, longi;
						if(Locate_Pos(rcv_gps, 1)==2)	// ��2��һ���Ƕ���
						{
								posx1 = 2;
								posx2 = Locate_Pos(rcv_gps, 2);
								mid(latitude,rcv_gps, posx2-posx1-1,posx1);	// ����һ�����ź͵ڶ�������֮���ֵ��lati����
							
								posx1 = Locate_Pos(rcv_gps, 3);
								posx2 = Locate_Pos(rcv_gps, 4);
								mid(longitude,rcv_gps, posx2-posx1-1,posx1);// �����������ź͵��ĸ�����֮���ֵ��longi����
								
								//���������������
								Uart2SendStr("AT+CIPSEND\r");
								DelaySec(1);
							
								//��ʽ��"A2,1255.12001,12322.54000"
								UART_2SendOneByte('A');
								UART_2SendOneByte(DEVICEID);
								UART_2SendOneByte(',');

								Uart2SendStr(latitude);
								UART_2SendOneByte(',');
								Uart2SendStr(longitude);

								CleanSimRcv();//������ջ���
							
						}
						else
						{
								// recive wrong
								// �����
								P0_1 = 1;
						}
				}
		}
		else if(done_record == 2)
		{
				// something wrong
				// �����
				// P0_1 = 1;
		}
		else{		//done_record = 0
				// something wrong
				// �����
				// P0_1 = 1;
		}
		done_record = 0;
		CleanGpsRcv();// ������ջ���
}

/*���ַ������м��ȡn���ַ�*/
char * mid(char *dst,char *src, int n,int m) /*nΪ���ȣ�mΪλ��*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*�ӵ�m�������*/
    if(m<0) m=0;    /*�ӵ�һ����ʼ*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*�б�Ҫ�𣿺��б�Ҫ*/
    return dst;
}

//��buf����õ���cx���������ڵ�λ�ã���1��ʼ��
//����ֵ:1~0XFE,����������λ�õ�ƫ�ƣ�Ҳ��1��ʼ��
//       0XFF,�������ڵ�cx������							  
char Locate_Pos(char *buf,char cx)
{	 		    
	char *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}


/***************************GPRS���ջ������ݳ�ʼ������***************/
/*����ԭ��:void CleanGsmRcv(void)
/*��������:��GsmRcv[]�������������ʹ����ָ���Ƶ���һλ��
/*�������:��
/*�������:��
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
