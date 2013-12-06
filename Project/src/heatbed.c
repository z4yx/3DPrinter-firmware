/*
 * =====================================================================================
 *
 *       Filename:  heatbed.c
 *
 *    Description:  加热板温度控制
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
#include "adc.h"
#include "pwmOutput.h"
#include "heatbed.h"

static bool bHeating;
static int16_t targetTemp = HEATBED_DEFAULT_TEMP;
static struct PIDController pid;

void HeatBed_Init()
{
	bHeating = false;
	ADC_Config(1);
	ADC_Channel_Config(HeaterBoardTherm_Port, HeaterBoardTherm_Pin, HeaterBoardTherm_ADCChannel, 1);
	ADC_Start();
}

void HeatBed_Start_Heating()
{
	// PID_Init(&pid, EXTRUDER_PID_KP, EXTRUDER_PID_KI, EXTRUDER_PID_KD, EXTRUDER_PID_INIT_SUM);
	PWM_Channel(2, 90, true);
	bHeating = true;
}

void HeatBed_Stop_Heating()
{
	PWM_Channel(2, 10, false);
	bHeating = false;
}

void HeatBedTask(void)
{
	if(bHeating) {
		int output;
		int16_t cur = ADC_Read_Value();
		if(cur < 0) {
			ERR_MSG("No adc value available!", "err");
			return;
		}
		// output = PID_Update(&pid, targetTemp - cur);
		
		// //转为百分比
		// output /= 400;
		// if(output > 100)
		// 	output = 100;
		// else if(output < 0)
		// 	output = 0;

		// PWM_Channel(1, output, true);

		DBG_MSG("temp: %d, output: %d", (int)cur, output);
	}
}