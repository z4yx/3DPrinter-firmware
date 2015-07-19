#ifndef ROBOT_ARM_H__
#define ROBOT_ARM_H__

#include "common.h"

void RobotArm_Init(void);
void RobotArm_Coordinate2MotorPos(int coord[3], int motor[3]);
bool RobotArm_HomeOperation(uint8_t selected_dir);
void RobotArm_Home_LimitSwitchTrig(uint8_t sw);


#endif