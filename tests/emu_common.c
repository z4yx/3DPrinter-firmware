#include "common.h"

//根据频率(Hz)计算TIM的参数
void Timer_16bit_Calc(int freq, uint16_t *period, uint16_t *prescaler)
{
	*period = SystemCoreClock / freq;

	if(*period > 60000) {
		*period = SystemCoreClock / 5000 / freq - 1;
		*prescaler = 5000 - 1;
	}
	else {
		(*period)--;
		*prescaler = 0;
	}
}