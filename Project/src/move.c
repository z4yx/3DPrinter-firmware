/*
 * =====================================================================================
 *
 *       Filename:  move.c
 *
 *    Description:  三轴运动控制
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
#include "systick.h"
#include "limitSwitch.h"

//三轴当前位置(相对原点的距离,单位um)
static int currentPos[3];

//步进电机每个脉冲产生的位移
static float um_per_pulse[3];

//三轴当前状态
static uint8_t currentState[3];

static const int8_t motorDirFix[3] = {X_DIRECTION_ADJ, Y_DIRECTION_ADJ, Z_DIRECTION_ADJ};

void Move_Init()
{
	Motor_Init();
	LimitSwitch_Config();

	um_per_pulse[X_Axis] = (float)X_DISTANCE_PER_CYCLE / X_PULSES_PER_CYCLE;
	um_per_pulse[Y_Axis] = (float)Y_DISTANCE_PER_CYCLE / Y_PULSES_PER_CYCLE;
	um_per_pulse[Z_Axis] = (float)Z_DISTANCE_PER_CYCLE / Z_PULSES_PER_CYCLE;

	for (int i = 0; i < 3; ++i)
	{
		currentState[i] = Axis_State_Ready;
	}
}

static bool alreadyHomed(uint8_t axis)
{
	if(axis == X_Axis)
		return LimitSwitch_Pressed(LimitSwitch_XMin);
	if(axis == Y_Axis)
		return LimitSwitch_Pressed(LimitSwitch_YMin);
	if(axis == Z_Axis)
		return LimitSwitch_Pressed(LimitSwitch_ZMin);
	return false;
}

static void homingDone(uint8_t axis)
{
	DBG_MSG("Axis %d homing Done", (int)axis);
	Motor_Stop(axis);
	currentPos[axis] = 0;
	currentState[axis] = Axis_State_Ready;
}

//开始回原点过程
bool Move_Home(uint8_t axis)
{
	if(currentState[axis] != Axis_State_Ready) {
		ERR_MSG("Axis %d is not Ready! state = %d", (int)axis, (int)currentState[axis]);
		return false;
	}
	//回原点前先检查限位开关是否已经被按下了
	if(alreadyHomed(axis)) {
		DBG_MSG("Axis %d already homed!", (int)axis);
		homingDone(axis);
		return true;
	}

	currentState[axis] = Axis_State_Homing;
	//一直运动直到触碰限位开关
	Motor_Start(axis, -1, 1, Move_Dir_Back * motorDirFix[axis]);

	return true;
}

//由限位开关中断调用
void Move_LimitReached(uint8_t sw_num)
{
	bool err = false;
	uint8_t err_axis;
	switch(sw_num) {
		case LimitSwitch_XMin:
			if(currentState[X_Axis] == Axis_State_Homing)
				homingDone(X_Axis);
			else{
				err = true;
				err_axis = X_Axis;
			}
			break;

		case LimitSwitch_YMin:
			if(currentState[Y_Axis] == Axis_State_Homing)
				homingDone(Y_Axis);
			else{
				err = true;
				err_axis = Y_Axis;
			}
			break;

		case LimitSwitch_ZMin:
			if(currentState[Z_Axis] == Axis_State_Homing)
				homingDone(Z_Axis);
			else{
				err = true;
				err_axis = Z_Axis;
			}
			break;

		// case LimitSwitch_XMax:
		// ...

	}
	if(err) {
		Motor_Stop(err_axis);
		ERR_MSG("Limit switch on illegally! axis=%d, switch=%d", (int)err_axis, (int)sw_num);
	}
}