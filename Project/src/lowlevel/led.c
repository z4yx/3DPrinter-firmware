/*
 * =====================================================================================
 *
 *       Filename:  led.c
 *
 *    Description:  实现板载LED的控制
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
#include "led.h"

void LED_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_GPIOClockCmd(LED_Port, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_2 | LED_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_Port, &GPIO_InitStructure);

    GPIO_SetBits(LED_Port, LED_2 | LED_1);
}

void LED_Enable(int LEDx, bool bEnabled)
{
    if (!bEnabled)
        GPIO_ResetBits(LED_Port, (LEDx == LED2 ? LED_2 : LED_1));
    else
        GPIO_SetBits(LED_Port, (LEDx == LED2 ? LED_2 : LED_1));
}
