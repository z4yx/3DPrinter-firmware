#include "common.h"
uint8_t EMUMACH_GetLimitSwitch(uint8_t sw);
void EMUMACH_SetStepperDir(uint8_t motor, uint8_t dir);
void EMUMACH_SetStepperStepLow(uint8_t motor);
void EMUMACH_SetStepperStepHigh(uint8_t motor);
uint8_t EMUMACH_TIM_UpdateFlag(uint8_t tim);
uint8_t EMUMACH_TIM_CC1Flag(uint8_t tim);
void EMUMACH_TIM_ClearUpdateFlag(uint8_t tim);
void EMUMACH_TIM_Set(uint8_t tim, uint16_t period, uint16_t prescaler, uint16_t oc);
void EMUMACH_TIM_Cmd(uint8_t tim, uint8_t enable);
