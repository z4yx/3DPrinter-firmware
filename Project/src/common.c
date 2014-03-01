/*
 * =====================================================================================
 *
 *       Filename:  common.c
 *
 *    Description:  项目的公共函数实现
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
#include "common.h"

void RCC_GPIOClockCmd(GPIO_TypeDef* GPIOx, FunctionalState state)
{
	uint32_t rcc;
	switch((uint32_t)GPIOx) {
		case (uint32_t)GPIOA:
			rcc = RCC_APB2Periph_GPIOA;
			break;
		case (uint32_t)GPIOB:
			rcc = RCC_APB2Periph_GPIOB;
			break;
		case (uint32_t)GPIOC:
			rcc = RCC_APB2Periph_GPIOC;
			break;
		case (uint32_t)GPIOD:
			rcc = RCC_APB2Periph_GPIOD;
			break;
		case (uint32_t)GPIOE:
			rcc = RCC_APB2Periph_GPIOE;
			break;
		default:
			/* Invalid argument! */
			return;
	}
	RCC_APB2PeriphClockCmd(rcc, state);
}

void RCC_USARTClockCmd(USART_TypeDef* USARTx, FunctionalState state)
{
	switch((uint32_t)USARTx) {
		case (uint32_t)USART1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, state);
			break;
		case (uint32_t)USART2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, state);
			break;
		case (uint32_t)USART3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, state);
			break;
		default:
			/* Invalid argument! */
			return;
	}
}

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