#include "common.h"

//根据频率(Hz)计算TIM的参数
void Timer_16bit_Calc(int freq, uint16_t *period, uint16_t *prescaler)
{
	uint32_t tmp = SystemCoreClock / freq;
	*prescaler = 1;
	while(tmp > 0xffff){
		*prescaler <<= 1;
		tmp >>= 1;
	}
	(*prescaler) --;
	*period = tmp-1;
}

float Distance3D(float dx, float dy, float dz)
{
	return sqrt(dx*dx + dy*dy + dz*dz);
}
