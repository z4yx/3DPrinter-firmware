/*
 * =====================================================================================
 *
 *       Filename:  extruder.c
 *
 *    Description:  挤出器控制
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
#include "pid.h"
#include "max6675.h"
#include "fanControl.h"
#include "pwmOutput.h"
#include "extruder.h"
#include "systick.h"
#include "motor.h"

static bool bHeating;
static int16_t targetTemp;
static int16_t currentTemp;
static int currentOutput;
static struct PIDController pid;
static SysTick_t lastUpdatingTime;

void Extruder_Init()
{
	bHeating = false;
	currentTemp = -1;
	currentOutput = 0;
	MAX6675_Config();

	lastUpdatingTime = GetSystemTick();
}

void Extruder_Start_Heating(uint16_t _target)
{
	DBG_MSG("Target=%d", _target);
	PID_Init(&pid, EXTRUDER_PID_KP, EXTRUDER_PID_KI, EXTRUDER_PID_KD, EXTRUDER_PID_INIT_SUM);
	PWM_Channel(Ex1Heat_Ch, 90, false);
	currentOutput = 0;
	bHeating = true;
	targetTemp = _target;
}

void Extruder_Stop_Heating()
{
	PWM_Channel(Ex1Heat_Ch, 10, false);
	currentOutput = 0;
	bHeating = false;
}

bool Extruder_TempReached()
{
	int t;
	if(currentTemp < 0)
		return false;
	t = currentTemp - targetTemp;
	return t >= -5;
}

void Extruder_SetOutput(int output)
{
	if(output > 100)
		output = 100;
	else if(output < 0)
		output = 0;

	PWM_Channel(Ex1Heat_Ch, output, true);
	currentOutput = output;
}

void ExtruderTask(void)
{
	SysTick_t now = GetSystemTick();
	if(bHeating && now - lastUpdatingTime > EXTRUDER_UPDATE_PERIOD) {
		int output;
		currentTemp = MAX6675_Read_Value();

		lastUpdatingTime = now;

		if(currentTemp < 0) {
			ERR_MSG("Thermcouple not connected!", 0);
			return;
		}
		output = PID_Update(&pid, targetTemp - currentTemp);
		
		//转为百分比
		output /= 400;

		Extruder_SetOutput(output);
	}
}

void Extruder_GetState(int16_t *temp, int *output, bool *heating)
{
	*temp = currentTemp;
	*output = currentOutput;
	*heating = bHeating;
}