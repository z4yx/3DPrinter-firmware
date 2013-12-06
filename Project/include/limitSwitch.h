#ifndef __LIMIT_SWITCH_H
#define	__LIMIT_SWITCH_H

#include "common.h"

enum { LimitSwitch_XMin, LimitSwitch_YMin, LimitSwitch_ZMin };

void LimitSwitch_Config(void);
bool LimitSwitch_Pressed(uint8_t sw);

#endif /* __LIMIT_SWITCH_H */