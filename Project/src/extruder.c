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

/*
Temperature to Resistance:
0.08*e^(4226.7257/(x+273.15))
{60,24000},
{77,13800},
{100,7000},
{150,1800},
{175,1000},
{190,750},
{200,620}

Resistance to ADC value:
(5-5/((x*8200)/(x+8200)+4700)*4700)/3.3*4096

Temperature to ADC value:
(5-5/((0.08*e^(4226.7257/(x+273.15))*8200)/(0.08*e^(4226.7257/(x+273.15))+8200)+4700)*4700)/3.3*4096
{23,3814},
{29,3783},
{35,3743},
{42,3695},
{55,3560},
{70,3340},
{85,3060},
{94,2860},
{108,2525},
{121,2178},
{132,1894},
{154,1388},
{172,1035},
{184,850},
{195,703},
{216,498},
{227,410},
{235,367},
{244,320},
{250,290}
*/

#include "common.h"
#include "pid.h"
#include "max6675.h"
#include "fanControl.h"
#include "pwmOutput.h"
#include "analog.h"
#include "extruder.h"
#include "systick.h"
#include "motor.h"
#include <math.h>

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
#if EXTRUDER_THERMO_USING_ADC
	Analog_SetChannel(Extruder1Therm_ADC_Ch, true);
#else
	MAX6675_Config();
#endif

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
#if EXTRUDER_THERMO_USING_ADC
		uint16_t adc_val = Analog_GetChannelValue(Extruder1Therm_ADC_Ch);
		currentTemp = EXTRUDER_ADC_TO_TEMP(adc_val);
		// DBG_MSG("ADC Val: %d %d", adc_val, currentTemp);
#else
		currentTemp = MAX6675_Read_Value();
#endif

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