#ifndef UART_H
#define UART_H

#define S2RI 0x01        //����2�����ж������־λ
#define S2TI 0x02        //����2�����ж������־λ

void InitUART(void);
void UART_1SendOneByte(unsigned char c);
void UART_2SendOneByte(unsigned char c);
void Uart2SendStr(char* data_at);
void Uart1SendStr(char* data_at);

#endif