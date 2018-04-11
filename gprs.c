#include "sys.h"

char rcv_sim[MAXRCV];
uchar SimRcvCnt = 0;

/************���п�2 SIM�жϴ�����*************/
void UART_2Interrupt(void) interrupt 8
{
				char tmp;
//				P0_1 = 1;		// test
				IE2 = 0x00;		//�رմ���2�ж�//ES2 = 0;
	
// ����2���ܵ�������
        if(S2RI)
        {
                
                S2CON &= ~S2RI;
                tmp = S2BUF;
								
								rcv_sim[SimRcvCnt] = tmp;	//����ε�����װ������ȼ���
								SimRcvCnt++;
					
								if(SimRcvCnt > MAXRCV)
								{
										SimRcvCnt = 0;
								}
								
        } 
				
				IE2 = 0x01;				//�򿪴���2�ж� ES2 = 1;
}

/***************************GPRS���ջ������ݳ�ʼ������***************/
/*����ԭ��:void CleanGsmRcv(void)
/*��������:��GsmRcv[]�������������ʹ����ָ���Ƶ���һλ��
/*�������:��
/*�������:��
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

/***************************GPRS�����������ֺ���***************/
/*����ԭ��:bit Hand(unsigned char *a)
/*��������:�ж��յ��������Ƿ�����жϵ����ݡ�
/*�������:�жϳ��ֵ�����
/*�������:	1:�������жϵ�����
			0:û�г����жϵ�����
/********************************************************************/
bit Hand(unsigned char *a)
{ 
	if(strstr(rcv_sim,a)!=NULL)
		return 1;
	else
		return 0;
}