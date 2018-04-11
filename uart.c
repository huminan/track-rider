#include "sys.h"

/****************串行口初始化函数****************/
/* 串行口1打开但中断关闭
/* 串行口2打开并开始接收数据*/
void InitUART(void)
{
// UART 1
/*    TMOD = 0x20;    //定时器1工作在方式2  8位自动重装
    SCON = 0x40;    //串口1工作在方式1   10位异步收发 REN=0禁止接收
    TH1 = 0x40;     //定时器1初值
		TR1 = 1;      	//定时器1开始计数
		ES = 0;        	//关串口1中断	*/
	
		SCON = 0x40;    //串口1工作在方式1   10位异步收发 REN=0禁止接收
		
// UART 2
    S2CON = 0x50;   //串口2工作在方式1：2^S2SMODE*溢出率/32 ： S2REN=1接收
    BRT = 0xFA;     //独立波特率发生器初值	BAUD：9600
    AUXR = 0x11;    //BRTR=1 独立波特率发生器开始计数		T1x12=0 ; BRTx12=0 ；S2SMODE=0 ; S1BRS=1 : Serial 1 use BRT
    IE2 = 0x01;     //开串口2中断  ES2=1

		EA =1;        	//开总中断
}

/****************串行口1发送****************/
void UART_1SendOneByte(unsigned char c)
{
    SBUF = c;
    while(!TI);           //若TI=0，在此等待
    TI = 0;         
}
/****************串行口2发送****************/
void UART_2SendOneByte(unsigned char c)
{
    S2BUF = c;
    while(!(S2CON&S2TI));  //若S2TI=0，在此等待
    S2CON&=~S2TI;          //S2TI=0
}

/***************************串口2发送字符串函数**********************/
/*函数原型:void Uart2SendStr(char* data_at)
/*函数功能:串口2发送函数，发送字符串
/*输入参数:字符串指针
/*输出参数:无
/********************************************************************/
void Uart2SendStr(char* data_at)
{
	while(*data_at)
	{
		UART_2SendOneByte(*data_at++);	
	}
}


/***************************串口1发送字符串函数**********************/
/*函数原型:void Uart1SendStr(char* data_at)
/*函数功能:串口2发送函数，发送字符串
/*输入参数:字符串指针
/*输出参数:无
/********************************************************************/
void Uart1SendStr(char* data_at)
{
	while(*data_at)
	{
		UART_1SendOneByte(*data_at++);	
	}
}