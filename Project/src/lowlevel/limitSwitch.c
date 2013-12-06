/*
 * =====================================================================================
 *
 *       Filename:  limitSwitch.c
 *
 *    Description:  限位开关控制
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
#include "limitSwitch.h"
#include "systick.h"

#define NUM_SWITCHS 3

//当前限位开关是否被触碰
bool bSwitchPressed[NUM_SWITCHS];
//上次中断触发时间
SysTick_t lastInterrupt[NUM_SWITCHS];

static void limitSwitch_ExtiConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_Init(&EXTI_InitStructure);	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource14);	
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15);
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_Init(&EXTI_InitStructure);	

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;      // 指定中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 指定抢占优先级别
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 指定响应优先级别
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

bool LimitSwitch_Pressed(uint8_t sw)
{
	uint8_t val;
	switch(sw) {
		case LimitSwitch_XMin:
			val = GPIO_ReadInputDataBit(X_Axis_Min_Port, X_Axis_Min_Pin);
			break;
		case LimitSwitch_YMin:
			val = GPIO_ReadInputDataBit(Y_Axis_Min_Port, Y_Axis_Min_Pin);
			break;
		case LimitSwitch_ZMin:
			val = GPIO_ReadInputDataBit(Z_Axis_Min_Port, Z_Axis_Min_Pin);
			break;
		default:
			ERR_MSG("Invalid switch number %d", sw);
			return false;
	}
	return !val;
}

void LimitSwitch_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	RCC_GPIOClockCmd(X_Axis_Min_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = X_Axis_Min_Pin;
	GPIO_Init(X_Axis_Min_Port, &GPIO_InitStructure);

	RCC_GPIOClockCmd(Y_Axis_Min_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Y_Axis_Min_Pin;
	GPIO_Init(Y_Axis_Min_Port, &GPIO_InitStructure);
	
	RCC_GPIOClockCmd(Z_Axis_Min_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Z_Axis_Min_Pin;
	GPIO_Init(Z_Axis_Min_Port, &GPIO_InitStructure);

	limitSwitch_ExtiConfig();

	for (int i = 0; i < NUM_SWITCHS; ++i)
	{
		bSwitchPressed[i] = LimitSwitch_Pressed(i);
		lastInterrupt[i] = 0;
	}
}

static void doInterrupt(uint8_t sw)
{
	SysTick_t now = GetSystemTick();
	//确保不是按键抖动
	if(now - lastInterrupt[sw] > LIMIT_SWITCH_MIN_TOGGLE_PERIOD) {
		bSwitchPressed[sw] = !bSwitchPressed[sw];
		DBG_MSG("lastInterrupt=%d now=%d", (int)lastInterrupt[sw], (int)now);
		lastInterrupt[sw] = now;

		DBG_MSG("limitSwitch irq, %d current state: %d", (int)sw, (int)bSwitchPressed[sw]);
		if(bSwitchPressed[sw]) {
			Move_LimitReached(sw);
		}
	}
}

void LimitSwitch_Interrupt(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        doInterrupt(LimitSwitch_XMin);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if (EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        doInterrupt(LimitSwitch_YMin);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        doInterrupt(LimitSwitch_ZMin);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}