#include "common.h"
#include "systick.h"
#include <time.h>
void SysTick_Init(void)
{
	//产生1ms间隔的中断
	// if (SysTick_Config(SystemCoreClock / 1000))
	// {
	// 	/* Capture error */
	// 	while (1);
	// }

	// 关闭滴答定时器
	// SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}
SysTick_t GetSystemTick(void)
{
	return 1000 * clock() / CLOCKS_PER_SEC;
}