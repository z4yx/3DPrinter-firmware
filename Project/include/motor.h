#ifndef __MOTOR__H__
#define __MOTOR__H__

enum { Move_Dir_Back = -1, Move_Dir_Forward = 1 };

void Motor_Stop(int motor_enum);
void Motor_InfinitelyRun(int motor_enum, int dir);
void Motor_MoveCmd(int xyza[4]);
void Motor_PowerOff(void);
void Motor_PowerOn(void);
bool Motor_isPowerOn(void);
void Motor_Init(void);



#endif /* __MOTOR__H__ */