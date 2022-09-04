#ifndef __MATRIXLED_H__
#define __MATRIXLED_H__

void Matrix_Init();
void _74HC595_WriteByte(unsigned char Byte);
void MatrixLED_ShowColumn(unsigned char Column, Data);
void MatrixLED_Loop(unsigned char LED_Buf[8][8]);

#endif