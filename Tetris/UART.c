#include <REGX52.H>

void UART_Init()		//2400bps@11.0592MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xF4;		//设定定时初值
	TH1 = 0xF4;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

// 串口发送字节
void UART_SendByte(unsigned char Byte)
{
	SBUF = Byte;
	while (TI == 0);
	TI = 0;
}

// 串口发送字符串
void UART_SendString(const char *s)
{
	unsigned char i;
	for (i = 0; s[i] != '\0'; ++i) 
		UART_SendByte(s[i]);
}