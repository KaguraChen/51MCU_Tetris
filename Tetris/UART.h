#ifndef __UART_H__
#define __UART_H_

void UART_Init();

void UART_SendByte(unsigned char Byte);

void UART_SendString(const char *s);

#endif