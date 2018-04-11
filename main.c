/***********************************************************************
时间：2012.11.24
芯片：STC12C5A60S2
晶振：22.1184MHz  波特率：9600bps

引脚定义：串行口1：发送————TxD/P3.1； 接收————RxD/P3.0
          串行口2：发送————TxD2/P1.3；接收————RxD2/P1.2
					
需要功能：串行口1与GPS通信，只负责收；不需要用中断方式，每5s接收一次
					串行口2与SIM通信，收发均需要。

注意：每个烧写前要不一样的device_id

若sim注册成功，P0_0亮绿灯
若接收gps信号错误，P0_1亮红灯
若还未定位，P0_3亮黄灯
***********************************************************************/
#include "sys.h"

/*****************主函数******************/
void main(void)
{
    InitUART();        //串行口1和2初始化
		Timer0Init();			 //定时器0初始化
	
		CleanSimRcv();	
		while(!Hand("OK"))//与GSM模块握手，判断是否收到GSM模块返回的OK
		{
				Uart2SendStr("ATE0\r");//关闭回显命令
				DelaySec(1);
		}
		Uart1SendStr("done 1\r");		// test
		
		CleanSimRcv();//清除接收缓存
    while(!(Hand("+CREG: 1,1") || Hand("+CREG: 1,5")))//检测是否以注册到服务商
		{
				Uart2SendStr("AT+CREG=1\r");//设置服务商状态为数据显示
				DelaySec(1);
				Uart2SendStr("AT+CREG?\r");//检测服务商状态
				DelaySec(1);		
		}
		CleanSimRcv();//清除接收缓存
		//灭绿灯表示注册成功
		P0_0 = 0;
		Uart1SendStr("done 2\r");	// test
		
		while(!Hand("OK"))
		{
				Uart2SendStr("AT+CSTT=\"CMNET\"\r");	// 指定移动营业端
				DelaySec(1);
		}
		Uart1SendStr("done 3\r");		// test
		while(!Hand("OK"))
		{
				Uart2SendStr("AT+CIICR\r");	// IP激活场景
				DelaySec(1);
		}
		Uart1SendStr("done 4\r");		// test
		
		
// start
		Uart2SendStr("AT+CLPORT=\"TCP\",\"2022\"\r");//发送指令指定本地端口
		DelaySec(1);

		Uart2SendStr("AT+CIPHEAD=1\r");//显示IP头？？？？？？？？？？？？？？？？？？？？？？？？
		DelaySec(1);


		CleanSimRcv();//清除接收缓存
	
		Uart2SendStr("AT+CIPSTART=\"TCP\",\"www.huminant.top\",1592\r");	//连接服务器，服务器端口号1592***********************	
		while(!(Hand("CONNECT OK")));	//检测是否收到 CONNECT OK ,如果连接成功 */
		Uart1SendStr("done 5\r");		// test
		
		while(1)
		{
				//过5秒发送一次GPS数据
				DelaySec(5);
//				Record_Gps();
		}
		
	
		/*开始等待接收服务器数据*********************************
		while(1)
		{
				if((Hand("+IPD") && (Hand("LED1") || Hand("led1"))))//检测是否收到led1
				{
						LED2_ON;
						LCD_prints(0,1,"LED2 ON");
						CleanSimRcv();//清除接收缓存	
				}
				
				if((Hand("+IPD") && (Hand("LED0") || Hand("led0"))))//检测是否收到led0
				{
						LED2_OFF;
						LCD_prints(0,1,"LED2 OFF");
						CleanSimRcv();//清除接收缓存	
				}

				if(sec_cnt >= 60)//一分钟后跳出循环，重新连接。
				{
						break;
				}	
		}*/
}
