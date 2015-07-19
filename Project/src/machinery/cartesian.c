#include "robotArm.h"
#include "limitSwitch.h"
#include "move.h"
#include "common.h"

#if MACHINERY_TYPE==MACHINERY_CARTESIAN

void RobotArm_Init()
{
    
}

void RobotArm_Coordinate2MotorPos(int coord[3], int motor[3])
{
    for (int i = 0; i < 3; ++i)
    {
        motor[i] = coord[i];
    }
}

bool RobotArm_HomeOperation(uint8_t selected_dir)
{
    if(selected_dir & MOVE_DIR_X){
        if(LimitSwitch_Pressed(LimitSwitch_XMin)){
            Move_ResetDirection(MOVE_DIR_X);
            return true;
        }else{
            return Move_MotorHome(X_Axis, Move_Dir_Back);
        }
    }
    if(selected_dir & MOVE_DIR_Y){
        if(LimitSwitch_Pressed(LimitSwitch_YMin)){
            Move_ResetDirection(MOVE_DIR_Y);
            return true;
        }else{
            return Move_MotorHome(Y_Axis, Move_Dir_Back);
        }
    }
    if(selected_dir & MOVE_DIR_Z){
        if(LimitSwitch_Pressed(LimitSwitch_ZMin)){
            Move_ResetDirection(MOVE_DIR_Z);
            return true;
        }else{
            return Move_MotorHome(Z_Axis, Move_Dir_Back);
        }
    }
    return false;
}

void RobotArm_Home_LimitSwitchTrig(uint8_t sw)
{
    switch(sw){
    case LimitSwitch_XMin:
        Move_MotorHomeDone(X_Axis);
        Move_ResetDirection(MOVE_DIR_X);
        break;
    case LimitSwitch_YMin:
        Move_MotorHomeDone(Y_Axis);
        Move_ResetDirection(MOVE_DIR_Y);
        break;
    case LimitSwitch_ZMin:
        Move_MotorHomeDone(Z_Axis);
        Move_ResetDirection(MOVE_DIR_Z);
        break;
    }
}


#endif
