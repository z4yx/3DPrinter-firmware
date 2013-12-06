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


static bool bHeating;
static int16_t targetTemp = EXTRUDER_DEFAULT_TEMP;
static struct PIDController pid;

void Extruder_Init()
{
	bHeating = false;
	Fan_Config();
	MAX6675_Config();
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

void ExtruderTask(void)
{
	if(bHeating) {
		int output;
		int16_t cur = MAX6675_Read_Value();
		if(cur < 0) {
			ERR_MSG("Thermcouple not connected!", 0);
			return;
		}
		output = PID_Update(&pid, targetTemp - cur);
		
		//转为百分比
		output /= 400;
		if(output > 100)
			output = 100;
		else if(output < 0)
			output = 0;

		PWM_Channel(1, output, true);

		DBG_MSG("temp: %d, output: %d", (int)cur, output);
	}
}