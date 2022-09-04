#include <REGX52.H>
#include "Delay1ms.h"

sbit RCK = P3 ^ 5;
sbit SCK = P3 ^ 6;
sbit SER = P3 ^ 4;

#define MATRIX_LED_PORT P0

void Matrix_Init()
{
	SCK = 0;
	RCK = 0;
}

void _74HC595_WriteByte(unsigned char Byte)
{
	unsigned char i;
	for (i = 0; i < 8; ++i) {
		SER = Byte & (0x80 >> i); 
		SCK = 1; 
		SCK = 0;
	}
	RCK = 1;
	RCK = 0;
}

void MatrixLED_ShowColumn(unsigned char Column, Data) 
{
	_74HC595_WriteByte(Data);
	MATRIX_LED_PORT = ~(0x80 >> Column);
	Delay1ms(1);
	MATRIX_LED_PORT = 0xFF;
}

// 定时器扫描LED矩阵
void MatrixLED_Loop(unsigned char LED_buf[8][8])
{
	unsigned char i, j, buf = 0;
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			if (LED_buf[i][j])
				buf |= (0x01 << j);
		}
		MatrixLED_ShowColumn(i, buf);
		buf = 0x00;
	}
}