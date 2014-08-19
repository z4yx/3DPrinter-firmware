#include "emu_machinery.h"
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

static uint8_t (Motor_TIM[NUM_MOTORS]) = {4, 5, 6, 7};

static void Motor_TIM_Config(uint32_t freq,uint8_t tim)
{
	uint16_t period, prescaler;

	EMUMACH_TIM_Cmd(tim, 0);

	Timer_16bit_Calc(freq, &period, &prescaler);

	EMUMACH_TIM_Set(tim, period, prescaler, 0);


	// TIM_ARRPreloadConfig(tim, ENABLE);

	// TIM_SetCounter(tim, 0);

	EMUMACH_TIM_Cmd(tim, 1);
}

void Motor_Interrupt_emu(uint8_t tim)
{
	if(EMUMACH_TIM_UpdateFlag(tim)) {
		EMUMACH_TIM_ClearUpdateFlag(tim);

		for(int i=0; i<NUM_MOTORS; i++) {
			if(Motor_TIM[i] == tim) {

				if(Motor_PendingSteps[i]) {

					if(Motor_OutputLevel[i])
						EMUMACH_SetStepperStepHigh(i);
					else
						EMUMACH_SetStepperStepLow(i);
					// GPIO_WriteBit(Motor_Step_Ports[i], Motor_Step_Pins[i], Motor_OutputLevel[i]);
					Motor_OutputLevel[i] ^= 1;

					if(Motor_PendingSteps[i] > 0){
						Motor_PendingSteps[i] --;
						if(!Motor_PendingSteps[i]){
							//attenuate holding current
							// GPIO_SetBits(Motor_OE_Ports[i], Motor_OE_Pins[i]);
							EMUMACH_SetStepperStepLow(i);
							EMUMACH_TIM_Cmd(tim, 0);
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
	// GPIO_SetBits(Motor_OE_Ports[motor_enum], Motor_OE_Pins[motor_enum]);
	EMUMACH_SetStepperStepLow(motor_enum);
	EMUMACH_TIM_Cmd(Motor_TIM[motor_enum], 0);
	Motor_PendingSteps[motor_enum] = 0;
}

void Motor_Start(int motor_enum, int steps, int8_t dir, uint32_t freq)
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
	EMUMACH_SetStepperStepLow(motor_enum);

	Motor_OutputLevel[motor_enum] = 1;

	//double steps and frequency for rising and falling edge
	Motor_PendingSteps[motor_enum] = steps << 1;
	Motor_TIM_Config((freq << 1), Motor_TIM[motor_enum]);
}

void Motor_PowerOn()
{
	// for(int i=0; i<NUM_MOTORS; i++) {
	// 	GPIO_SetBits(Motor_En_Ports[i], Motor_En_Pins[i]);
	// }
	isPowerOn = true;
}

void Motor_PowerOff()
{
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
	// RCC_APB1PeriphClockCmd(Motor_TIM_ClockSrc, ENABLE);
	// Motor_Output_Config();
	// Motor_TIM_SetInterrupt();
}
