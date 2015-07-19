#ifndef __LED_H
#define	__LED_H

#include "common.h"

#define LED1 1

#define LED_ON  true
#define LED_OFF false

void LED_Config(void);
void LED_Enable(int LEDx, bool bEnabled);

#endif