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
#include "robotArm.h"
#include <stdlib.h>

#define MIN2SEC 60

//三轴当前位置(相对原点的距离,单位um),及挤出器累计旋转量
static int currentPos[4];

//当前位置(以电机的步进数量为单位)
static int currentSteps[4];

//步进电机每个脉冲产生的位移
static float um_per_pulse[4];

//三轴当前状态
static volatile uint8_t currentState[3];

static const int8_t motorDirFix[4] = {X_DIRECTION_ADJ, Y_DIRECTION_ADJ, Z_DIRECTION_ADJ, A_DIRECTION_ADJ};

void Move_Init()
{
	Motor_Init();
	LimitSwitch_Config();
	RobotArm_Init();

	um_per_pulse[X_Axis] = (float)X_DISTANCE_PER_CYCLE / X_PULSES_PER_CYCLE;
	um_per_pulse[Y_Axis] = (float)Y_DISTANCE_PER_CYCLE / Y_PULSES_PER_CYCLE;
	um_per_pulse[Z_Axis] = (float)Z_DISTANCE_PER_CYCLE / Z_PULSES_PER_CYCLE;
	um_per_pulse[A_Axis] = (float)A_DISTANCE_PER_CYCLE / A_PULSES_PER_CYCLE;

	for (int i = 0; i < 3; ++i)
	{
		currentState[i] = Axis_State_Ready;
	}

	int xyza[4] = {0};
	Move_SetCurrentPos(xyza);
}

void Move_MotorHomeDone(uint8_t axis)
{
	DBG_MSG("Axis %d homing Done", (int)axis);
	Motor_Stop(axis);
	currentState[axis] = Axis_State_Ready;
}

//步进运动完成,由电机中断调用
void Move_Axis_Eneded(uint8_t axis)
{
	if(axis < 3)
		currentState[axis] = Axis_State_Ready;
}

//三轴均处于待命状态
bool Move_XYZ_Ready()
{
	for (int i = 0; i < 3; ++i)
	{
		if(currentState[i] != Axis_State_Ready)
			return false;
	}
	return true;
}

//开始回原点过程
bool Move_Home(uint8_t selected_dir)
{
	return RobotArm_HomeOperation(selected_dir);
}

//针对某一个电机的回原点操作，由RobotArm_HomeOperation调用
bool Move_MotorHome(uint8_t axis, int8_t dir)
{
	if(currentState[axis] != Axis_State_Ready) {
		ERR_MSG("Axis %d is not Ready! state = %d", (int)axis, (int)currentState[axis]);
		return false;
	}

	currentState[axis] = Axis_State_Homing;
	//一直运动直到触碰限位开关
	Motor_Start(axis, -1, dir * motorDirFix[axis], DEFAULT_FEEDRATE / MIN2SEC / um_per_pulse[axis]);

	return true;
}

//根据距离计算步进数量
int calc_step(int axis, int um)
{
	return um/um_per_pulse[axis];
}

//三轴相对移动及挤出器旋转
bool Move_RelativeMove(int xyza[4], int feedrate)
{
	int tmp[4];

	for (int i = 0; i < 4; ++i)
		tmp[i] = currentPos[i] + xyza[i];

	return Move_AbsoluteMove(tmp, feedrate);
}

//三轴绝对移动及挤出器旋转
bool Move_AbsoluteMove(int xyza[4], int feedrate)
{
	int tmp[4], delta[4], dir[4], motorPos[4];

	if(!Move_XYZ_Ready())
		return false;

	RobotArm_Coordinate2MotorPos(xyza, motorPos);
	motorPos[3] = xyza[3];

	for (int i = 0; i < 4; ++i)
	{
		delta[i] = xyza[i] - currentPos[i];
		currentPos[i] = xyza[i];
	}

	if(motorPos[X_Axis] < 0 || motorPos[X_Axis] > X_MAX_LIMIT)
		return false;
	if(motorPos[Y_Axis] < 0 || motorPos[Y_Axis] > Y_MAX_LIMIT)
		return false;
	if(motorPos[Z_Axis] < 0 || motorPos[Z_Axis] > Z_MAX_LIMIT)
		return false;

	for (int i = 0; i < 4; ++i){
		int step = calc_step(i, motorPos[i]);
		int d = step - currentSteps[i];

		dir[i] = motorDirFix[i] * (d > 0 ? Move_Dir_Forward : Move_Dir_Back);
		tmp[i] = abs(d);
		currentSteps[i] = step;
	}

	float distance = Distance3D(delta[X_Axis], delta[Y_Axis], delta[Z_Axis]);
	if(distance < 1) {
		distance = abs(delta[A_Axis]);
		if(distance < 1) {
			ERR_MSG("Move distance < 1", 0);
			return false;
		}
	}
	float duration = distance / feedrate * MIN2SEC;

	DBG_MSG("duration: %dms", (int)(duration*1000));

	for (int i = 0; i < 4; ++i)
	{
		if(!tmp[i])
			continue;
		currentState[i] = Axis_State_Moving;
		Motor_Start(i, tmp[i], dir[i], (uint32_t)(tmp[i]/duration));
	}

	return true;
}

bool Move_SetCurrentPos(int xyza[4])
{
	int motorPos[4];

	RobotArm_Coordinate2MotorPos(xyza, motorPos);
	motorPos[3] = xyza[3];

	if(motorPos[X_Axis] < 0 || motorPos[X_Axis] > X_MAX_LIMIT)
		return false;
	if(motorPos[Y_Axis] < 0 || motorPos[Y_Axis] > Y_MAX_LIMIT)
		return false;
	if(motorPos[Z_Axis] < 0 || motorPos[Z_Axis] > Z_MAX_LIMIT)
		return false;

	for (int i = 0; i < 4; ++i){
		currentPos[i] = xyza[i];
		currentSteps[i] = calc_step(i, motorPos[i]);
	}
	
	return true;
}

void Move_ResetAxisA()
{
	currentPos[A_Axis] = currentSteps[A_Axis] = 0;
}

void Move_ResetDirection(uint8_t selected_dir)
{
	int xyza[4];
	for (int i = 0; i < 4; ++i)
	{
		xyza[i] = currentPos[i];
	}
	if(selected_dir&MOVE_DIR_X)
		xyza[X_Axis] = 0;
	if(selected_dir&MOVE_DIR_Y)
		xyza[Y_Axis] = 0;
	if(selected_dir&MOVE_DIR_Z)
		xyza[Z_Axis] = 0;
	if(!Move_SetCurrentPos(xyza))
		ERR_MSG("Failed to set position");
}

//由限位开关中断调用
void Move_LimitReached(uint8_t sw_num)
{
	bool err = false;
	uint8_t err_axis;
	if(currentState[X_Axis] == Axis_State_Homing ||
		currentState[Y_Axis] == Axis_State_Homing ||
		currentState[Z_Axis] == Axis_State_Homing){

		RobotArm_Home_LimitSwitchTrig(sw_num);
	}else{
		err = true;
		switch(sw_num) {
			case LimitSwitch_XMax:
			case LimitSwitch_XMin:
				err_axis = X_Axis;
				break;
			case LimitSwitch_YMax:
			case LimitSwitch_YMin:
				err_axis = Y_Axis;
				break;
			case LimitSwitch_ZMax:
			case LimitSwitch_ZMin:
				err_axis = Z_Axis;
				break;
		}
		// Motor_Stop(err_axis);
		ERR_MSG("Limit switch on illegally! axis=%d, switch=%d", (int)err_axis, (int)sw_num);
	}
}