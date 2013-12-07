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
*/

static bool bHeating;
static int16_t targetTemp = HEATBED_DEFAULT_TEMP;
static struct PIDController pid;
static SysTick_t lastUpdatingTime;

void HeatBed_Init()
{
	bHeating = false;
	ADC_Config(1);
	ADC_Channel_Config(HeaterBoardTherm_Port, HeaterBoardTherm_Pin, HeaterBoardTherm_ADCChannel, 1);
	ADC_Start();

	lastUpdatingTime = GetSystemTick();
}

void HeatBed_Start_Heating()
{
	PID_Init(&pid, EXTRUDER_PID_KP, EXTRUDER_PID_KI, EXTRUDER_PID_KD, EXTRUDER_PID_INIT_SUM);
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
	SysTick_t now = GetSystemTick();
	if(bHeating && now - lastUpdatingTime > HEATBED_UPDATE_PERIOD) {
		int output, temp;
		int16_t cur = ADC_Read_Value();

		lastUpdatingTime = now;

		if(cur < 0) {
			ERR_MSG("No adc value available!", "err");
			return;
		}
		temp = HEATBED_ADC_TO_TEMP(cur);
		output = PID_Update(&pid, targetTemp - temp);
		
		//转为百分比
		output /= 400;
		if(output > 100)
			output = 100;
		else if(output < 0)
			output = 0;

		PWM_Channel(2, output, true);

		DBG_MSG("temp: %d, output: %d", (int)temp, output);
	}
}