#include "common.h"
#include "motor.h"
#include "move.h"
#include "emu_machinery.h"

#define NUM_MOTORS 4

static volatile bool isPowerOn;

//电机剩余步进数,-1表示无限
static volatile int Motor_PendingSteps[NUM_MOTORS];
//步进脉冲计数,用于调速
static volatile int Motor_PulseCount[NUM_MOTORS];
//跳过步进脉冲数量,用于减速
static int Motor_PulseSkip[NUM_MOTORS];
//电机旋转方向,+/-1
static int8_t Motor_Direction[NUM_MOTORS];


static void Motor_TIM3_Config(void)
{
	uint16_t period, prescaler;
	// TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// TIM_OCInitTypeDef  TIM_OCInitStructure;

	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

	Timer_16bit_Calc(MOTOR_STEP_FREQ, &period, &prescaler);

	DBG_MSG("period:%d, prescaler:%d", period, prescaler);

	EMUMACH_TIM_Set(3, period, prescaler, (period+1)/2-1);
	/* Time base configuration */	
	/*	 
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	        //设置预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                //设置时钟分频系数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;       //配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;   
    TIM_OCInitStructure.TIM_Pulse = (period+1)/2-1;      //设置跳变值，当计数器计数到这个值时，电平发生跳变
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	*/
}

// static void TIM3_SetInterrupt(FunctionalState state)
// {  
//     NVIC_InitTypeDef NVIC_InitStructure;
//     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //指定中断源
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 指定响应优先级别
//     NVIC_InitStructure.NVIC_IRQChannelCmd = state;
//     NVIC_Init(&NVIC_InitStructure);
// }

void Motor_Interrupt(void)
{
	if(EMUMACH_TIM_UpdateFlag(3)/*SET == TIM_GetITStatus(TIM3, TIM_IT_Update)*/) {
		// TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		//在定时器重装中断里STEP置0
		for(int i=0; i<NUM_MOTORS; i++) {
			EMUMACH_SetStepperStepLow(i);
			// GPIO_ResetBits(Motor_Step_Ports[i], Motor_Step_Pins[i]);
		}

	}else if(EMUMACH_TIM_CC1Flag(3)/*SET == TIM_GetITStatus(TIM3, TIM_IT_CC1)*/) {
		// TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		//在比较匹配中断里STEP置1
		for(int i=0; i<NUM_MOTORS; i++) {
			if(Motor_PendingSteps[i]) {

				Motor_PulseCount[i]++;
				if(Motor_PulseCount[i] % Motor_PulseSkip[i])
					continue;

				EMUMACH_SetStepperStepHigh(i);
				// GPIO_SetBits(Motor_Step_Ports[i], Motor_Step_Pins[i]);
				if(Motor_PendingSteps[i] > 0){
					Motor_PendingSteps[i] --;
					if(!Motor_PendingSteps[i]){
						//attenuate holding current
						// GPIO_SetBits(Motor_OE_Ports[i], Motor_OE_Pins[i]);
						Move_Axis_Eneded(i);
					}
				}
			}
		}

	}
}

//强行停止电机运行,不会触发运行完成事件
void Motor_Stop(int motor_enum)
{
	Motor_PendingSteps[motor_enum] = 0;
}

void Motor_Start(int motor_enum, int steps, int skip, int8_t dir)
{
	// Motor_Stop();
	// DBG_MSG("steps=%d, skip=%d", (int)steps, (int)skip);

	Motor_Direction[motor_enum] = dir;
	EMUMACH_SetStepperDir(motor_enum, dir);
	// GPIO_WriteBit(Motor_Dir_Ports[motor_enum],
	// 	Motor_Dir_Pins[motor_enum],
	// 	(dir > 0 ? Bit_SET : Bit_RESET));

	// set current to 100%
	// GPIO_ResetBits(Motor_OE_Ports[motor_enum], Motor_OE_Pins[motor_enum]);

	Motor_PulseSkip[motor_enum] = skip;
	Motor_PulseCount[motor_enum] = 0;
	Motor_PendingSteps[motor_enum] = steps;
}

void Motor_PowerOn()
{
	DBG_MSG("on",0);
	// for(int i=0; i<NUM_MOTORS; i++) {
	// 	GPIO_SetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	// }
	isPowerOn = true;
}

void Motor_PowerOff()
{
	DBG_MSG("off",0);
	// for(int i=0; i<NUM_MOTORS; i++) {
	// 	GPIO_ResetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	// }
	isPowerOn = false;
}

bool Motor_isPowerOn()
{
	return isPowerOn;
}

void Motor_Init()
{
	// Motor_Output_Config();
	Motor_TIM3_Config();
	// TIM3_SetInterrupt(ENABLE);

	// TIM_Cmd(TIM3, ENABLE);
	EMUMACH_TIM_Cmd(3, 1);
}
