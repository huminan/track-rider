#ifndef TIMER_H
#define TIMER_H

extern volatile unsigned int data timer0_cnt;
extern volatile int data sec_cnt;
extern void (code * pfun1)(void);	// 定时到达时操作函数的函数指针1

void Timer0Init(void);
void DelaySec(unsigned char sec);

#endif