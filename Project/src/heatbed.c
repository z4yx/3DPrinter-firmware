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
#include "systick.h"

/*
//实际温度与ADC采样值数据拟合
Fit[{
{3477,24},
{3200,41},
{3100,44},
{2800,56},
{2700,60},
{2600,63},
{2400,72},
{2250,80},
{2000,89},
{1900,96}
},{x,1},x]

//新加热板
Fit[{
{2262,21},
{1310,44},
{1000,53},
{800,64},
{650,73},
{550,79},
{450,85},
{350,94},
{300,98},
{255,107}
},{x^2,x,1},x]
*/

static bool bHeating;
static int16_t targetTemp = HEATBED_DEFAULT_TEMP;
static int16_t currentTemp;
static int currentOutput;
static struct PIDController pid;
static SysTick_t lastUpdatingTime;

void HeatBed_Init()
{
	bHeating = false;
	currentTemp = -1;
	currentOutput = 0;
	ADC_Config(1);
	ADC_Channel_Config(HeaterBoardTherm_Port, HeaterBoardTherm_Pin, HeaterBoardTherm_ADCChannel, 1);
	ADC_Start();

	lastUpdatingTime = GetSystemTick();
}

void HeatBed_Start_Heating()
{
	PID_Init(&pid, EXTRUDER_PID_KP, EXTRUDER_PID_KI, EXTRUDER_PID_KD, EXTRUDER_PID_INIT_SUM);
	PWM_Channel(HBP_Ch, 90, false);
	currentOutput = 0;
	bHeating = true;
}

void HeatBed_Stop_Heating()
{
	PWM_Channel(HBP_Ch, 10, false);
	currentOutput = 0;
	bHeating = false;
}

bool HeatBed_TempReached()
{
	int t;
	if(currentTemp < 0)
		return false;
	t = currentTemp - targetTemp;
	return t >= -2;
}

void HeatBed_SetOutput(int output)
{
	if(output > 100)
		output = 100;
	else if(output < 0)
		output = 0;

	PWM_Channel(HBP_Ch, output, true);
	currentOutput = output;
}

void HeatBedTask(void)
{
	SysTick_t now = GetSystemTick();
	if(bHeating && now - lastUpdatingTime > HEATBED_UPDATE_PERIOD) {
		int output;
		int16_t t = ADC_Read_Value();

		lastUpdatingTime = now;

		if(t < 0) {
			currentTemp = -1;
			ERR_MSG("No adc value available!", "err");
			return;
		}
		currentTemp = HEATBED_ADC_TO_TEMP(t);
		output = PID_Update(&pid, targetTemp - currentTemp);
		
		//转为百分比
		output /= 400;

		HeatBed_SetOutput(output);
	}
}
void HeatBed_GetState(int16_t *temp, int *output, bool *heating)
{
	*temp = currentTemp;
	*output = currentOutput;
	*heating = bHeating;
}