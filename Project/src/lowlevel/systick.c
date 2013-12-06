/*
 * =====================================================================================
 *
 *       Filename:  systick.c
 *
 *    Description:  系统滴答定时器，产生系统标准时间，实现延时等待
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

void SysTick_Init(void)
{
	//产生1ms间隔的中断
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}

	// 关闭滴答定时器
	// SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void IncSysTickCounter(void)
{
	systemTickCounter ++;
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
		SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPONEXIT); // Reset SLEEPONEXIT
        SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);   // Clear SLEEPDEEP bit
        __WFI();                                                // Request Wait For Interrupt
	}
}


/*
 * 微秒级延时
 */
void Delay_us(unsigned int us)
{
	uint32_t val, last;
	int32_t tmp = us*(SystemCoreClock/1000000);

	last = SysTick->VAL;
	while(tmp > 0) {
		val = SysTick->VAL;
		if(val <= last)
			tmp -= last - val;
		else
			tmp -= last + SysTick->LOAD - val;
		last = val;
	}
}
