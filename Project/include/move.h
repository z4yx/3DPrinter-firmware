#ifndef __MOVE__H__
#define __MOVE__H__

enum { Axis_State_Ready, Axis_State_Moving, Axis_State_Homing };

void Move_Init(void);
bool Move_Home(uint8_t axis);


void Move_LimitReached(uint8_t sw_num);


#endif /* __MOVE__H__ */