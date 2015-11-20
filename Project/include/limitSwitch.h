#ifndef __LIMIT_SWITCH_H
#define	__LIMIT_SWITCH_H

#include "common.h"

#define NUM_SWITCHS 6
enum { LimitSwitch_XMin, LimitSwitch_YMin, LimitSwitch_ZMin, LimitSwitch_XMax, LimitSwitch_YMax, LimitSwitch_ZMax};

void LimitSwitch_Config(void);
bool LimitSwitch_Pressed(uint8_t sw);

#endif /* __LIMIT_SWITCH_H */