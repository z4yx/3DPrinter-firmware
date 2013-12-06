/*
 * =====================================================================================
 *
 *       Filename:  keyboard.c
 *
 *    Description:  板载按键驱动
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
#include "keyboard.h"
#include "systick.h"

static bool bPressed[NUM_KEYS];
static SysTick_t lastChange[NUM_KEYS], lastRepeat[NUM_KEYS];

void KeyBoard_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	RCC_GPIOClockCmd(Btn_Left_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Btn_Left_Pin;
	GPIO_Init(Btn_Left_Port, &GPIO_InitStructure);

	RCC_GPIOClockCmd(Btn_Down_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Btn_Down_Pin;
	GPIO_Init(Btn_Down_Port, &GPIO_InitStructure);

	RCC_GPIOClockCmd(Btn_Right_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Btn_Right_Pin;
	GPIO_Init(Btn_Right_Port, &GPIO_InitStructure);

	RCC_GPIOClockCmd(Btn_Up_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Btn_Up_Pin;
	GPIO_Init(Btn_Up_Port, &GPIO_InitStructure);

	RCC_GPIOClockCmd(Btn_Center_Port, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Btn_Center_Pin;
	GPIO_Init(Btn_Center_Port, &GPIO_InitStructure);

	for (int i = 0; i < NUM_KEYS; ++i)
	{
		bPressed[i] = KeyBoard_isPressed(i);
		lastRepeat[i] = lastChange[i] = GetSystemTick();
	}
}

bool KeyBoard_isPressed(uint8_t num)
{
	bool pressed;
	switch(num) {
		case KEY_LEFT:
			pressed = !GPIO_ReadInputDataBit(Btn_Left_Port, Btn_Left_Pin);
			break;
		case KEY_RIGHT:
			pressed = !GPIO_ReadInputDataBit(Btn_Right_Port, Btn_Right_Pin);
			break;
		case KEY_UP:
			pressed = !GPIO_ReadInputDataBit(Btn_Up_Port, Btn_Up_Pin);
			break;
		case KEY_DOWN:
			pressed = !GPIO_ReadInputDataBit(Btn_Down_Port, Btn_Down_Pin);
			break;
		case KEY_CENTER:
			pressed = !GPIO_ReadInputDataBit(Btn_Center_Port, Btn_Center_Pin);
			break;
		default:
			ERR_MSG("Invalid key number %d", (int)num);
			return false;
	}
	return pressed;
}

void KeyBoard_Task()
{
	SysTick_t now = GetSystemTick();
	for (int i = 0; i < NUM_KEYS; ++i)
	{
		bool cur_state;

		cur_state = KeyBoard_isPressed(i);
		//按键状态改变
		if(cur_state != bPressed[i]) {
			//有效的按键变化
			if(now - lastChange[i] >= KEYBOARD_MIN_TOGGLE_PERIOD){
				lastChange[i] = now;
				bPressed[i] = cur_state;

				DBG_MSG("State changed key=%d, state=%d", (int)i, (int)cur_state);
			}
		}else{
			//长时间按下,开始自动重复
			if(cur_state && now - lastChange[i] >= KEYBOARD_START_REPEAT_TIME) {
				if(now - lastRepeat[i] >= KEYBOARD_REPEAT_PERIOD){
					lastRepeat[i] = now;
					DBG_MSG("Repeat key=%d, state=%d", (int)i, (int)cur_state);
				}
			}
		}
	}
}