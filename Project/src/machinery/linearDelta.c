#include "robotArm.h"
#include "limitSwitch.h"
#include "move.h"
#include "common.h"
#include <math.h>

#if MACHINERY_TYPE==MACHINERY_LINEARDELTA

static float delta_tower1_x;
static float delta_tower1_y;
static float delta_tower2_x;
static float delta_tower2_y;
static float delta_tower3_x;
static float delta_tower3_y;
static float arm_length_squared;

#define SQ(x) powf((x), 2)
#define PIOVER180   0.01745329251994329576923690768489F

void RobotArm_Init()
{
    float delta_radius = ROBOTIC_ARM_RADIUS;
    arm_length_squared = SQ(ROBOTIC_ARM_LENGTH);

    delta_tower1_x = (delta_radius) * cosf((210.0F) * PIOVER180); // front left tower
    delta_tower1_y = (delta_radius) * sinf((210.0F) * PIOVER180);
    delta_tower2_x = (delta_radius) * cosf((330.0F) * PIOVER180); // front right tower
    delta_tower2_y = (delta_radius) * sinf((330.0F) * PIOVER180);
    delta_tower3_x = (delta_radius) * cosf((90.0F ) * PIOVER180); // back middle tower
    delta_tower3_y = (delta_radius) * sinf((90.0F ) * PIOVER180);
}

void RobotArm_Coordinate2MotorPos(int coord[3], int motor[3])
{
 
    motor[0] = sqrtf(arm_length_squared
               - SQ(delta_tower1_x - coord[X_Axis])
               - SQ(delta_tower1_y - coord[Y_Axis])
              ) + coord[Z_Axis];
    motor[1] = sqrtf(arm_length_squared
               - SQ(delta_tower2_x - coord[X_Axis])
               - SQ(delta_tower2_y - coord[Y_Axis])
              ) + coord[Z_Axis];
    motor[2] = sqrtf(arm_length_squared
               - SQ(delta_tower3_x - coord[X_Axis])
               - SQ(delta_tower3_y - coord[Y_Axis])
              ) + coord[Z_Axis];
}

bool RobotArm_HomeOperation(uint8_t selected_dir)
{
    if((selected_dir & MOVE_DIR_X) || (selected_dir & MOVE_DIR_Y)){
        bool success = true, all_pressed = true;
        if(!LimitSwitch_Pressed(LimitSwitch_XMax)){
            all_pressed = false;
            success &= Move_MotorHome(X_Axis, Move_Dir_Forward);
        }
        if(!LimitSwitch_Pressed(LimitSwitch_YMax)){
            all_pressed = false;
            success &= Move_MotorHome(Y_Axis, Move_Dir_Forward);
        }
        if(!LimitSwitch_Pressed(LimitSwitch_ZMax)){
            all_pressed = false;
            success &= Move_MotorHome(Z_Axis, Move_Dir_Forward);
        }
        if(all_pressed){
            Move_ResetDirection(MOVE_DIR_X|MOVE_DIR_Y);
        }
        return success;
    }else if(selected_dir & MOVE_DIR_Z){
        if(LimitSwitch_Pressed(LimitSwitch_XMax) &&
            LimitSwitch_Pressed(LimitSwitch_YMax) &&
            LimitSwitch_Pressed(LimitSwitch_ZMax)){

            return Move_MotorHome(X_Axis, Move_Dir_Back) &&
                Move_MotorHome(Y_Axis, Move_Dir_Back) &&
                Move_MotorHome(Z_Axis, Move_Dir_Back);
        }else{
            ERR_MSG("Please home XY first");
            return false;
        }
    }
    return false;
}

void RobotArm_Home_LimitSwitchTrig(uint8_t sw)
{
    switch(sw){
    case LimitSwitch_XMax:
        Move_MotorHomeDone(X_Axis);
        break;
    case LimitSwitch_YMax:
        Move_MotorHomeDone(Y_Axis);
        break;
    case LimitSwitch_ZMax:
        Move_MotorHomeDone(Z_Axis);
        break;
    case LimitSwitch_ZMin:
        Move_MotorHomeDone(X_Axis);
        Move_MotorHomeDone(Y_Axis);
        Move_MotorHomeDone(Z_Axis);
        break;
    }
    switch(sw){
    case LimitSwitch_XMax:
    case LimitSwitch_YMax:
    case LimitSwitch_ZMax:
        if(LimitSwitch_Pressed(LimitSwitch_XMax) &&
            LimitSwitch_Pressed(LimitSwitch_YMax) &&
            LimitSwitch_Pressed(LimitSwitch_ZMax)){
            //三个机械臂均触碰限位，挤出头一定在中心

            Move_ResetDirection(MOVE_DIR_X|MOVE_DIR_Y);
        }
        break;
    case LimitSwitch_ZMin:
        Move_ResetDirection(MOVE_DIR_Z);
        break;
    }
}

#endif
