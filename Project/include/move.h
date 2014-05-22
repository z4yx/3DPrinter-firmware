#ifndef __MOVE__H__
#define __MOVE__H__

enum { Axis_State_Ready, Axis_State_Moving, Axis_State_Homing };
enum { Move_Dir_Back = -1, Move_Dir_Forward = 1 };

void Move_Init(void);
bool Move_Home(uint8_t axis);
bool Move_XYZ_Ready(void);
bool Move_RelativeMove(int xyza[4]);
bool Move_AbsoluteMove(int xyza[4]);
bool Move_SetCurrentPos(int xyza[4]);

void Move_LimitReached(uint8_t sw_num);
void Move_Axis_Eneded(uint8_t axis);

#endif /* __MOVE__H__ */