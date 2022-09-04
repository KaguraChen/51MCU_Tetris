#include <REGX52.H>

void Timer0_Init(void)
{
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0;		//设置定时初值
	TH0 = 0;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0不计时
	PT0 = 1;
}

// 计数器设置数字
void Timer0_SetCounter(unsigned int Value)
{
	TH0=Value/256;
	TL0=Value%256;
}

// 计数器返回数字
unsigned int Timer0_GetCounter(void)
{
	return (TH0<<8)|TL0;
}

// 计数器开始
void Timer0_Run(unsigned char Flag)
{
	TR0=Flag;
}