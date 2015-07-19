#ifndef __MOVE__H__
#define __MOVE__H__

#include "common.h"

enum
{
    MOVE_DIR_X = 1,
    MOVE_DIR_Y = 2,
    MOVE_DIR_Z = 4
};
enum { Axis_State_Ready, Axis_State_Moving, Axis_State_Homing };
enum { Move_Dir_Back = -1, Move_Dir_Forward = 1 };

void Move_Init(void);
bool Move_Home(uint8_t selected_dir);
bool Move_MotorHome(uint8_t axis, int8_t dir);
void Move_MotorHomeDone(uint8_t axis);
bool Move_XYZ_Ready(void);
bool Move_RelativeMove(int xyza[4], int feedrate);
bool Move_AbsoluteMove(int xyza[4], int feedrate);
bool Move_SetCurrentPos(int xyza[4]);
void Move_ResetAxisA(void);
void Move_ResetDirection(uint8_t selected_dir);

void Move_LimitReached(uint8_t sw_num);
void Move_Axis_Eneded(uint8_t axis);

#endif /* __MOVE__H__ */