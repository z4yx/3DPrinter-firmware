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

//当前限位开关是否被触碰
bool bSwitchPressed[NUM_SWITCHS];
//是否已经发送限位开关事件
bool bEventSent[NUM_SWITCHS];
//上次中断触发时间
SysTick_t lastInterrupt[NUM_SWITCHS];

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
		case LimitSwitch_EXT2HB:
			val = GPIO_ReadInputDataBit(Ext2Hb_Limit_Port, Ext2Hb_Limit_Pin);
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
	
	RCC_GPIOClockCmd(Ext2Hb_Limit_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Ext2Hb_Limit_Pin;
	GPIO_Init(Ext2Hb_Limit_Port, &GPIO_InitStructure);

	for (int i = 0; i < NUM_SWITCHS; ++i)
	{
		bSwitchPressed[i] = LimitSwitch_Pressed(i);
		lastInterrupt[i] = 0;
		bEventSent[i] = 0;
	}

	//不使用中断
	// limitSwitch_ExtiConfig();
}

void LimitSwitch_Task()
{
	SysTick_t now = GetSystemTick();
	for (int i = 0; i < NUM_SWITCHS; ++i)
	{
		bool cur_state;

		cur_state = LimitSwitch_Pressed(i);
		//状态改变
		if(cur_state != bSwitchPressed[i]) {
			//有效的按键变化
			if(now - lastInterrupt[i] >= LIMIT_SWITCH_MIN_TOGGLE_PERIOD){
				lastInterrupt[i] = now;
				bSwitchPressed[i] = cur_state;
				bEventSent[i] = false;
			}
		}else{
			//超过阈值,按下有效
			if(cur_state && !bEventSent[i] && now - lastInterrupt[i] >= LIMIT_SWITCH_VALID_TIME) {
				bEventSent[i] = true;
				Move_LimitReached(i);
			}
		}
	}
}
