/*
 * =====================================================================================
 *
 *       Filename:  systick.c
 *
 *    Description:  系统滴答定时器，产生系统标准时间
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "stm32f10x.h"
#include "systick.h"

static volatile SysTick_t systemTickCounter = 0;
static volatile uint16_t systemTickCounter_us = 0;

void SysTick_Init(void)
{
	if (SysTick_Config(SystemCoreClock / 1000000))
	{
		/* Capture error */
		while (1);
	}

	// 关闭滴答定时器
	// SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void IncSysTickCounter(void)
{
	if(++systemTickCounter_us >= 1000) {
		systemTickCounter_us = 0;
		systemTickCounter ++;
	}
}

SysTick_t GetSystemTick(void)
{
	SysTick_t tmp;
	
	do {
		tmp = systemTickCounter;
		//防止读取变量过程中被中断更改
	}while(tmp != systemTickCounter);
	
	return tmp;
}

/*
 * 毫秒级延时，误差±1ms
 */
void Delay_ms(unsigned int ms)
{
	SysTick_t t = GetSystemTick();
	t += ms;
	while(GetSystemTick() < t){
		// SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPONEXIT); // Reset SLEEPONEXIT
  //       SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);   // Clear SLEEPDEEP bit
  //       __WFI();                                                // Request Wait For Interrupt
	}
}

#define MOD_1G 1000000000
static uint32_t getCurrentUsMod1G()
{
	SysTick_t ms;
	uint16_t us;

	do {
		ms = systemTickCounter;
		us = systemTickCounter_us;
		//防止读取变量过程中被中断更改
	}while(ms != systemTickCounter ||
		us != systemTickCounter_us);

	return (ms % MOD_1G * 1000 + us) % MOD_1G;
}
/*
 * 微秒级延时
 */
void Delay_us(unsigned int us)
{
	uint32_t start = getCurrentUsMod1G();

	for(;;) {
		uint32_t cur = getCurrentUsMod1G();
		if(cur >= start) {
			if(cur - start >= us)
				return;
		}else { //发生回绕
			if(MOD_1G - start >= us)
				return;
			else {
				us -= (MOD_1G - start);
				start = 0;
			}
		}
	}
}