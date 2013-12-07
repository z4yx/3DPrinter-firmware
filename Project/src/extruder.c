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
static int16_t targetTemp = EXTRUDER_DEFAULT_TEMP;
static int16_t currentTemp;
static struct PIDController pid;
static SysTick_t lastUpdatingTime;

void Extruder_Init()
{
	bHeating = false;
	currentTemp = -1;
	Fan_Config();
	MAX6675_Config();

	lastUpdatingTime = GetSystemTick();
}

void Extruder_Start_Heating()
{
	Fan_Enable(true);
	PID_Init(&pid, EXTRUDER_PID_KP, EXTRUDER_PID_KI, EXTRUDER_PID_KD, EXTRUDER_PID_INIT_SUM);
	PWM_Channel(1, 90, true);
	bHeating = true;
}

void Extruder_Stop_Heating()
{
	Fan_Enable(false);
	PWM_Channel(1, 10, false);
	bHeating = false;
}

void Extruder_Extrude(int volume)
{
	DBG_MSG("volume=%d", volume);
	Motor_Start(A_Axis, volume * EXTRUDER_VOLUME_ADJ, EXTRUDER_SPEED_ADJ, EXTRUDER_MOTOR_DIR);
}

bool Extruder_TempReached()
{
	int t;
	if(currentTemp < 0)
		return false;
	t = currentTemp - targetTemp;
	return t >= -2;
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
		if(output > 100)
			output = 100;
		else if(output < 0)
			output = 0;

		PWM_Channel(1, output, true);

		DBG_MSG("temp: %d, output: %d", (int)currentTemp, output);
	}
}