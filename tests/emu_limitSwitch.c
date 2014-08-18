#include "common.h"
#include "limitSwitch.h"
#include "systick.h"
#include "emu_machinery.h"

#define NUM_SWITCHS 3

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
			val = EMUMACH_GetLimitSwitch(sw);
			break;
		case LimitSwitch_YMin:
			val = EMUMACH_GetLimitSwitch(sw);
			break;
		case LimitSwitch_ZMin:
			val = EMUMACH_GetLimitSwitch(sw);
			break;
		default:
			ERR_MSG("Invalid switch number %d", sw);
			return false;
	}
	return !val;
}

void LimitSwitch_Config(void)
{
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
