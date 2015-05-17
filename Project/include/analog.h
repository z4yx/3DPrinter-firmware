#ifndef ANALOG_H__
#define ANALOG_H__

#include "common.h"

uint16_t Analog_GetChannelValue(uint8_t channel);
void Analog_SetChannel(uint8_t channel, bool enabled);

#endif