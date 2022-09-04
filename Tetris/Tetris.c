#include <REGX52.H>
#include "Delay1ms.h"
#include "Timer2.h"
#include "IR.h"
#include "MatrixLED.h"
#include "UART.h"

unsigned char Command;	// 接收红外信号

unsigned char Score;	// 游戏分数
unsigned char Random;	// 随机生成俄罗斯方块
unsigned char Start;	// 判断是否开始

unsigned char idata LED_buf[8][8];	// 8x8 LED矩阵缓存
// 七种俄罗斯方块形状
char code Shape[7][4][2] = {
	{{-1,-1}, {0,-1}, {0,0}, {1,0}}, // Z
	{{-1,-1},{0,-1},{-1,0},{0,0}},	// O
	{{-1,0},{0,0},{0,-1},{1,-1}},		// S
	{{-1,0},{0,0},{0,-1},{1,0}},		// T
	{{0,0},{0,-1},{0,-2},{0,-3}},		// I
	{{-1,0},{0,0},{-1,-1},{-1,-2}},	// L
	{{-1,0},{0,0},{0,-1},{0,-2}}		// J
};

char temp_pos[2];	// 下落方块位置
char direction[2] = {0, -1};	// 下落方向
char idata temp_shape[4][2];	// 下落方块形状
unsigned char Type;	// 下落方块类型

// 在指定位置设置一个俄罗斯方块
void  Set_Block(char c, char r, char Shape[4][2])
{
	unsigned char i;
	for (i = 0; i < 4; ++i)
		LED_buf[c + Shape[i][0]][r + Shape[i][1]] = 1;	
}

// 向指定方向移动方块
void Block_Move(char temp_pos[2], char temp_shape[4][2], char direction[2])
{
	unsigned char i;
	if (!Start)
		return;
	// 清除
	for (i = 0; i < 4; ++i)
		LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]] = 0;
	temp_pos[0] += direction[0];
	temp_pos[1] += direction[1];
	// 下一位置点亮
	for (i = 0; i < 4; ++i)
		LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]] = 1;
}

// 判断是否游戏结束
char End_Check(char temp_pos[2], char temp_shape[4][2])
{
	unsigned char i;
	for (i = 0; i < 4; ++i) 
		if (LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]]) {
			UART_SendString("Game over!!!\n");
			Delay1ms(10);
			UART_SendString("Press Power_key to replay.\n");
			Start = 0;
			return 0;
		}
	return 1;
}

// 生成一个随机的新方块
void Generate_New_Block()
{
	unsigned char i;
	Type = Random % 7;	// 随机类型，Random依靠定时器
	for (i = 0; i < 4; ++i) {
		temp_shape[i][0] = Shape[Type][i][0];
		temp_shape[i][1] = Shape[Type][i][1];
	}
	temp_pos[0] = 4;
	temp_pos[1] = 7;
	End_Check(temp_pos, temp_shape);	// 判断新生成方块是否已满
	Set_Block(4,7,Shape[Type]);
}

// 判断是否在指定方向可以移动
char Move_Check(char temp_pos[2], char temp_shape[4][2], char direction[2])
{
	char x, y, flag = 1, i;
	x = temp_pos[0] + direction[0];
	y = temp_pos[1] + direction[1];
	// 清除
	for (i = 0; i < 4; ++i)
		LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]] = 0;
	// 是否越界
	for (i = 0; i < 4; ++i)
		if (x + temp_shape[i][0] < 0 || x + temp_shape[i][0] > 7 ||
			y + temp_shape[i][1] < 0 || y + temp_shape[i][1] > 7) 
			flag = 0;
	// 是否碰到其他方块
	for (i = 0; i < 4; ++i)
		if (LED_buf[x + temp_shape[i][0]][y + temp_shape[i][1]])
			flag = 0;
	// 点亮
	for (i = 0; i < 4; ++i)
		LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]] = 1;
	return flag;
}

// 判断行是否已满
char Row_Check(unsigned char r)
{
	unsigned char i;
	for (i = 0; i < 8; ++i)
		if (!LED_buf[i][r])
			return 0;
	return 1;
}

// 检查并且清除俄罗斯方块
void Check_Clear()
{
	unsigned char i, j, k, flag = 0;
	for (i = 0; i < 8;) {
		if (Row_Check(i)) {
			for (j = i + 1; j < 8; ++j)
				for (k = 0; k < 8; ++k)
					LED_buf[k][j - 1] = LED_buf[k][j];	// 将上面的方块移到下面
			Score += 10;
			flag = 1;
		}
		else 
			++i;
	}
	if (flag) {
		// 发出信息
		UART_SendString("Your score is ");
		UART_SendByte('0' + Score / 100 % 10);
		UART_SendByte('0' + Score / 10 % 10);
		UART_SendByte('0' + Score % 10);
		UART_SendByte('.');
		UART_SendByte('\n');
	}
}

// 方块旋转
void Block_Rotate(char temp_pos[2], char Shape[4][2])
{
	char temp_shape[4][2], i, flag = 0;
	// 创建旋转后的形状
	for (i = 0; i< 4; ++i) {
		temp_shape[i][0] = -Shape[i][1];
		temp_shape[i][1] = Shape[i][0];
	}
	// 消除
	for (i = 0; i < 4; ++i)
		LED_buf[temp_pos[0] + Shape[i][0]][temp_pos[1] + Shape[i][1]] = 0;
	// 判断是否碰到其他方块
	for (i = 0; i < 4; ++i)
		if (!LED_buf[temp_pos[0] + temp_shape[i][0]][temp_pos[1] + temp_shape[i][1]])
			flag = 1;
	// 判断是否越界x
	for (i = 0; i < 4; ++i)
		if (temp_pos[0] + temp_shape[i][0] < 0 || temp_pos[0] + temp_shape[i][0] > 7 ||
			temp_pos[1] + temp_shape[i][1] < 0 || temp_pos[1] + temp_shape[i][1] > 7) 
			flag = 0;
	if (flag) {
		for (i = 0; i< 4; ++i) {
			// 将旋转后的形状赋给全局变量
			Shape[i][0] = temp_shape[i][0]; 
			Shape[i][1] = temp_shape[i][1]; 		
		}
		for (i = 0; i < 4; ++i)
				LED_buf[temp_pos[0] + Shape[i][0]][temp_pos[1] + Shape[i][1]] = 1;
	}
	else
		// 点亮
		for (i = 0; i < 4; ++i)
			LED_buf[temp_pos[0] + Shape[i][0]][temp_pos[1] + Shape[i][1]] = 1;
}

// 方块加速下落
void Block_Land(char temp_pos[2], char temp_shape[4][2])
{
	unsigned char dir[2] = {0, -1};
	while (Move_Check(temp_pos, temp_shape, dir))
		Block_Move(temp_pos, temp_shape, dir);
}

// 主循环
void Tetris_Run()
{
	// 初始化
	Timer2_Init();
	IR_Init();
	Matrix_Init();
	UART_Init();
	// 开始信息
	Delay1ms(500);
	UART_SendString("Welcome to play Tetris!\n");
	Delay1ms(10);
	UART_SendString("Press Power_Key to start.\n");
	
	while(1)
	{
		if(IR_GetDataFlag() || IR_GetRepeatFlag())	
		{	
			Command=IR_GetCommand();

			switch (Command) {
				case IR_4:	// 左移
					direction[0] = -1;
					direction[1] = 0;
					if (Move_Check(temp_pos, temp_shape, direction))
						Block_Move(temp_pos, temp_shape, direction);
					break;
				case IR_6:	// 右移
					direction[0] = 1;
					direction[1] = 0;
					if (Move_Check(temp_pos, temp_shape, direction))
						Block_Move(temp_pos, temp_shape, direction);
					break;
				case IR_POWER:	// 游戏(重新)开始
					if (!Start) {
						unsigned char i, j;
						// 清屏
						for (i = 0; i < 8; ++i)
							for (j = 0; j < 8; ++j)
								LED_buf[i][j] = 0;
						Generate_New_Block();
						Start = 1;
					}
					break;
				case IR_2:	// 旋转
					Block_Rotate(temp_pos, temp_shape);
					break;
				case IR_5:	// 加速下落
					Block_Land(temp_pos, temp_shape);
			}
			// 保持方向一直向下
			direction[0] = 0;
			direction[1] = -1;
		}
	}
}

// 随机数生成与LED矩阵刷新
void Timer2_Routine() interrupt 5
{
	static unsigned int Count = 0;
	TF2 = 0;
	RCAP2L  = 0x66;	   
	RCAP2H = 0xFC;	
	++Count;
	++Random;

	if (Count % 10 == 0) {
		// 刷新LED矩阵
		MatrixLED_Loop(LED_buf);
	}
	else if (Count >= 307) {
		if (Start) {
			if (Move_Check(temp_pos, temp_shape, direction))
				Block_Move(temp_pos, temp_shape, direction);
			else {
				Check_Clear();
				Generate_New_Block();
			}
		}
		Count = 0;	
	}
}