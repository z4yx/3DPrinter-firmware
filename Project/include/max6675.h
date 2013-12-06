#ifndef __MAX6675_H__
#define	__MAX6675_H__

#include "stm32f10x.h"

void MAX6675_Config(void);
int16_t MAX6675_Read_Value(void);

#endif