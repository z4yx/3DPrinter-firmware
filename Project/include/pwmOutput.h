#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

void PWM_Init(int freq);
void PWM_Channel(int channel, int percent, uint8_t bEnabled);
void PWM_TIM2_Interrupt(void);

#endif
