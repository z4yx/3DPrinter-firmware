/*
 * =====================================================================================
 *
 *       Filename:  motor.c
 *
 *    Description:  步进电机底层控制模块
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

#define NUM_MOTORS 4

static volatile bool isPowerOn;

//电机剩余步进数,-1表示无限
static volatile int Motor_PendingSteps[NUM_MOTORS];
//电机旋转方向,+/-1
static int8_t Motor_Direction[NUM_MOTORS];
//下一个时钟输出的电平
static uint8_t Motor_OutputLevel[NUM_MOTORS];

//用于产生电机脉冲的定时器
static TIM_TypeDef* (Motor_TIM[NUM_MOTORS]) = {TIM4, TIM5, TIM6, TIM7};
static uint8_t Motor_TIM_IRQ[NUM_MOTORS] = {TIM4_IRQn, TIM5_IRQn, TIM6_IRQn, TIM7_IRQn};
static uint32_t Motor_TIM_ClockSrc = 
	RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5 | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7;

static const uint16_t Motor_En_Pins[NUM_MOTORS] =
	{X_Axis_Start_Pin, Y_Axis_Start_Pin, Z_Axis_Start_Pin, A_Axis_Start_Pin};
static const uint16_t Motor_Dir_Pins[NUM_MOTORS] =
	{X_Axis_Dir_Pin, Y_Axis_Dir_Pin, Z_Axis_Dir_Pin, A_Axis_Dir_Pin};
static const uint16_t Motor_Step_Pins[NUM_MOTORS] =
	{X_Axis_Step_Pin, Y_Axis_Step_Pin, Z_Axis_Step_Pin, A_Axis_Step_Pin};
static const uint16_t Motor_OE_Pins[NUM_MOTORS] =
	{X_Axis_OE_Pin, Y_Axis_OE_Pin, Z_Axis_OE_Pin, A_Axis_OE_Pin};

static GPIO_TypeDef* Motor_En_Ports[NUM_MOTORS] =
	{X_Axis_Start_Port, Y_Axis_Start_Port, Z_Axis_Start_Port, A_Axis_Start_Port};
static GPIO_TypeDef* Motor_Dir_Ports[NUM_MOTORS] =
	{X_Axis_Dir_Port, Y_Axis_Dir_Port, Z_Axis_Dir_Port, A_Axis_Dir_Port};
static GPIO_TypeDef* Motor_Step_Ports[NUM_MOTORS] =
	{X_Axis_Step_Port, Y_Axis_Step_Port, Z_Axis_Step_Port, A_Axis_Step_Port};
static GPIO_TypeDef* Motor_OE_Ports[NUM_MOTORS] =
	{X_Axis_OE_Port, Y_Axis_OE_Port, Z_Axis_OE_Port, A_Axis_OE_Port};

static void Motor_Output_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	for(int i=0; i<NUM_MOTORS; i++) {

		RCC_GPIOClockCmd(Motor_En_Ports[i], ENABLE);
		GPIO_InitStructure.GPIO_Pin = Motor_En_Pins[i];
		GPIO_Init(Motor_En_Ports[i], &GPIO_InitStructure);
		GPIO_ResetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	}

	for(int i=0; i<NUM_MOTORS; i++) {
		RCC_GPIOClockCmd(Motor_Dir_Ports[i], ENABLE);
		GPIO_InitStructure.GPIO_Pin = Motor_Dir_Pins[i];
		GPIO_Init(Motor_Dir_Ports[i], &GPIO_InitStructure);

		RCC_GPIOClockCmd(Motor_Step_Ports[i], ENABLE);
		GPIO_InitStructure.GPIO_Pin = Motor_Step_Pins[i];
		GPIO_Init(Motor_Step_Ports[i], &GPIO_InitStructure);
		GPIO_ResetBits(Motor_Step_Ports[i], Motor_Step_Pins[i]);

		RCC_GPIOClockCmd(Motor_OE_Ports[i], ENABLE);
		GPIO_InitStructure.GPIO_Pin = Motor_OE_Pins[i];
		GPIO_Init(Motor_OE_Ports[i], &GPIO_InitStructure);
		GPIO_SetBits(Motor_OE_Ports[i], Motor_OE_Pins[i]);
	}

}

static void Motor_TIM_Config(uint32_t freq, TIM_TypeDef *tim)
{
	uint16_t period, prescaler;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_Cmd(tim, DISABLE);

	Timer_16bit_Calc(freq, &period, &prescaler);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	/* Time base configuration */		 
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	        //设置预分频

	TIM_TimeBaseInit(tim, &TIM_TimeBaseStructure);

	// TIM_ARRPreloadConfig(tim, ENABLE);

	TIM_SetCounter(tim, 0);

	TIM_Cmd(tim, ENABLE);
}

static void Motor_TIM_SetInterrupt(void)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 指定响应优先级别
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	for (int i = 0; i < NUM_MOTORS; ++i)
	{
		NVIC_InitStructure.NVIC_IRQChannel = Motor_TIM_IRQ[i]; //指定中断源
		NVIC_Init(&NVIC_InitStructure);
		TIM_ITConfig(Motor_TIM[i], TIM_IT_Update, ENABLE);
	}
}

void Motor_Interrupt(TIM_TypeDef *tim)
{
	if(SET == TIM_GetITStatus(tim, TIM_IT_Update)) {
		TIM_ClearITPendingBit(tim, TIM_IT_Update);

		for(int i=0; i<NUM_MOTORS; i++) {
			if(Motor_TIM[i] == tim) {

				if(Motor_PendingSteps[i]) {

					GPIO_WriteBit(Motor_Step_Ports[i], Motor_Step_Pins[i], Motor_OutputLevel[i]);
					Motor_OutputLevel[i] ^= 1;

					if(Motor_PendingSteps[i] > 0){
						Motor_PendingSteps[i] --;
						if(!Motor_PendingSteps[i]){
							//attenuate holding current
							GPIO_SetBits(Motor_OE_Ports[i], Motor_OE_Pins[i]);
							GPIO_ResetBits(Motor_Step_Ports[i], Motor_Step_Pins[i]);
							TIM_Cmd(tim, DISABLE);
							Move_Axis_Eneded(i);
						}
					}
				}
				break;
			}
		}
	}
}

//强行停止电机运行,不会触发运行完成事件
void Motor_Stop(int motor_enum)
{
	GPIO_SetBits(Motor_OE_Ports[motor_enum], Motor_OE_Pins[motor_enum]);
	GPIO_ResetBits(Motor_Step_Ports[motor_enum], Motor_Step_Pins[motor_enum]);
	TIM_Cmd(Motor_TIM[motor_enum], DISABLE);
	Motor_PendingSteps[motor_enum] = 0;
}

void Motor_Start(int motor_enum, int steps, int8_t dir, uint32_t freq)
{
	// Motor_Stop();
	// DBG_MSG("steps=%d, skip=%d", (int)steps, (int)skip);

	Motor_Direction[motor_enum] = dir;
	GPIO_WriteBit(Motor_Dir_Ports[motor_enum],
		Motor_Dir_Pins[motor_enum],
		(dir > 0 ? Bit_SET : Bit_RESET));

	// set current to 100%
	GPIO_ResetBits(Motor_OE_Ports[motor_enum], Motor_OE_Pins[motor_enum]);
	GPIO_ResetBits(Motor_Step_Ports[motor_enum], Motor_Step_Pins[motor_enum]);

	Motor_OutputLevel[motor_enum] = 1;

	//double steps and frequency for rising and falling edge
	Motor_PendingSteps[motor_enum] = steps << 1;
	Motor_TIM_Config((freq << 1), Motor_TIM[motor_enum]);
}

void Motor_PowerOn()
{
	for(int i=0; i<NUM_MOTORS; i++) {
		GPIO_SetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	}
	isPowerOn = true;
}

void Motor_PowerOff()
{
	for(int i=0; i<NUM_MOTORS; i++) {
		GPIO_ResetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	}
	isPowerOn = false;
}

bool Motor_isPowerOn()
{
	return isPowerOn;
}

void Motor_Init()
{
	RCC_APB1PeriphClockCmd(Motor_TIM_ClockSrc, ENABLE);
	Motor_Output_Config();
	Motor_TIM_SetInterrupt();
}
