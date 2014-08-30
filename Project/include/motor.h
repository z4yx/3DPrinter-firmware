#ifndef __MOTOR__H__
#define __MOTOR__H__

void Motor_Stop(int motor_enum);
void Motor_Start(int motor_enum, int steps, int8_t dir, uint32_t freq);
void Motor_PowerOff(void);
void Motor_PowerOn(void);
bool Motor_isPowerOn(void);
void Motor_Init(void);

void Motor_Interrupt(TIM_TypeDef *tim);


#endif /* __MOTOR__H__ */