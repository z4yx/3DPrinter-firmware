#ifndef __MOTOR__H__
#define __MOTOR__H__

void Motor_Stop(int motor_enum);
void Motor_MoveCmd(int xyza[4]);
void Motor_PowerOff(void);
void Motor_PowerOn(void);
bool Motor_isPowerOn(void);
void Motor_Init(void);



#endif /* __MOTOR__H__ */