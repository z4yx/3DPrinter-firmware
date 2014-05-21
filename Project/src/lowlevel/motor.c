/*
 * =====================================================================================
 *
 *       Filename:  motor.c
 *
 *    Description:  步进电机底层控制模块
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"
#include "motor.h"
#include "move.h"
#include "usart.h"

#define NUM_MOTORS 4

#define MOTOR_USART USART2
#define MOTOR_BAUD  9600

static volatile bool isPowerOn;

//强行停止电机运行,不会触发运行完成事件
void Motor_Stop(int motor_enum)
{
	USART_putchar(MOTOR_USART, 0x03);
	USART_putchar(MOTOR_USART, motor_enum);
}

void Motor_Task(void)
{
	if(USART_GetFlagStatus(MOTOR_USART, USART_FLAG_RXNE) == SET){
		uint8_t byte = USART_getchar(MOTOR_USART);
		DBG_MSG("Received from motor: %d", byte);
		if(byte == 0x04){
			for (int i = 0; i < 3; ++i)
			{
				Move_Axis_Eneded(i);
			}
		}
	}
}

void Motor_MoveCmd(int xyza[4])
{
	const char* content = (const char*)xyza;
	USART_putchar(MOTOR_USART, 0x01);
	for (int i = 0; i < 16; ++i)
	{
		USART_putchar(MOTOR_USART, content[i]);
	}
}


void Motor_PowerOn()
{
	for (int i = 0; i < 3; ++i)
	{
		USART_putchar(MOTOR_USART, 0x02);
		USART_putchar(MOTOR_USART, i);
	}
	isPowerOn = true;
}

void Motor_PowerOff()
{
	for (int i = 0; i < 3; ++i)
	{
		USART_putchar(MOTOR_USART, 0x02);
		USART_putchar(MOTOR_USART, i|0x10);
	}
	isPowerOn = false;
}

bool Motor_isPowerOn()
{
	return isPowerOn;
}

void Motor_Init()
{
	USART_Config(MOTOR_USART, MOTOR_BAUD);
	isPowerOn = false;
}
