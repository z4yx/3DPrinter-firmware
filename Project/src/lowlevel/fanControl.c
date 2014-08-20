/*
 * =====================================================================================
 *
 *       Filename:  fanControl.c
 *
 *    Description:  挤出器散热风扇控制
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
#include "fanControl.h"


void Fan_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_GPIOClockCmd(Ex1Fan_Port, ENABLE); 

	GPIO_InitStructure.GPIO_Pin =  Ex1Fan_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(Ex1Fan_Port, &GPIO_InitStructure);
	Fan_Enable(false);

}

void Fan_Enable(bool bEnabled)
{
	DBG_MSG("Enable=%d", bEnabled);
#ifdef DRIVER_BOARD_POLARITY
	bEnabled = !bEnabled;
#endif

    if (bEnabled)
        GPIO_SetBits(Ex1Fan_Port, Ex1Fan_Pin);
    else
        GPIO_ResetBits(Ex1Fan_Port, Ex1Fan_Pin);
}
