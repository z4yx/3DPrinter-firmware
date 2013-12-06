#ifndef __MOTOR__H__
#define __MOTOR__H__

void Motor_Stop(int motor_enum);
void Motor_Start(int motor_enum, int steps, int8_t dir);
void Motor_PowerOff(void);
void Motor_PowerOn(void);
void Motor_Init(void);

void Motor_Interrupt(void);


#endif /* __MOTOR__H__ */