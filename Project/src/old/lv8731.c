#include "stm32f10x.h"
#include "pwm.h"
#include "lv8731.h"

#define LV8731_GPIO_RCC RCC_APB2Periph_GPIOA
#define LV8731_GPIO GPIOA

/*
          Ch1         Ch2        Ch3        Ch4
FR        PA0         PA2        PA4        PA11
ST        PA1         PA3        PA5        PA12        
STEP      PA6         PA7        PB0        PB1

*/

const uint16_t LV8731_FR[4] = {GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_4, GPIO_Pin_11};
const uint16_t LV8731_ST[4] = {GPIO_Pin_1, GPIO_Pin_3, GPIO_Pin_5, GPIO_Pin_12};

enum MotorState{MotorStopped, MotorPending, MotorRunning};

volatile static struct MotorInfo_t{
	int state;
	int steps;
}Motors[4];

static void doStart(int ch)
{
	// GPIO_SetBits(LV8731_GPIO, LV8731_ST[ch - 1]);
	TIMx_OCx_Config(TIM3, ch, 500, TIM_OutputState_Enable);
}

static void doStop(int ch)
{
	TIMx_OCx_Config(TIM3, ch, 500, TIM_OutputState_Disable);
	// GPIO_ResetBits(LV8731_GPIO, LV8731_ST[ch - 1]);
}

void LV8731_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	int i;
	for(i = 0; i < 4; i++) {
		TIMx_OCx_Config(TIM3, i + 1, 500, TIM_OutputState_Disable);
		Motors[i].steps = 0;
		Motors[i].state = MotorStopped;
	}

	RCC_APB2PeriphClockCmd(LV8731_GPIO_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = 0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	for(i = 0; i < 4; i++) {
		GPIO_InitStructure.GPIO_Pin |= LV8731_ST[i];
		GPIO_InitStructure.GPIO_Pin |= LV8731_FR[i];
	}

	GPIO_Init(LV8731_GPIO, &GPIO_InitStructure);

	for(i = 0; i < 4; i++) {
		// GPIO_ResetBits(LV8731_GPIO, LV8731_ST[i]);
		GPIO_SetBits(LV8731_GPIO, LV8731_ST[i]);
	}
}

int LV8731_Start(int num, int steps, int fr)
{
	int i = num - 1;
	if(Motors[i].state == MotorStopped) {
		if(fr)
			GPIO_SetBits(LV8731_GPIO, LV8731_FR[i]);
		else
			GPIO_ResetBits(LV8731_GPIO, LV8731_FR[i]);

		// TIMx_OCx_Config(TIM3, num, 500, TIM_OutputState_Enable);
		
		Motors[i].steps = steps;
		// Motors[i].state = MotorRunning;
		Motors[i].state = MotorPending;
		return 0;
	}
	return -1;
}

void OCx_Interrupt_Handler(int OCx)
{
	int i = OCx - 1;
	if(Motors[i].state == MotorRunning) {
		if(--Motors[i].steps == 0) {
			doStop(OCx);
			Motors[i].state = MotorStopped;
		}
	}
}

void TIM_Overflow_Handler()
{
	int i;
	for(i = 0; i < 4; i++) {
		if(Motors[i].state == MotorPending) {
			doStart(i + 1);
			Motors[i].state = MotorRunning;
		}
	}
}