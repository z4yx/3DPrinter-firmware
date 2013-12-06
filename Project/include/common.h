/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  项目的公共函数和参数定义
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
#ifndef __COMMON__H__
#define __COMMON__H__

#include "stm32f10x.h"
#include "hardwareDef.h"
#include "configure.h"
#include <stdint.h>

#ifndef bool
#define bool uint8_t
#define true 1
#define false 0
#endif

extern void USART_printf(uint8_t *Data, ...);

#define DBG_MSG(format, ...) USART_printf("[Debug]%s: " format "\r\n", __func__, __VA_ARGS__)
#define ERR_MSG(format, ...) USART_printf("[Error]%s: " format "\r\n", __func__, __VA_ARGS__)
 
void RCC_GPIOClockCmd(GPIO_TypeDef* GPIOx, FunctionalState state);
void Timer_16bit_Calc(int freq, uint16_t *period, uint16_t *prescaler);

typedef void (*Task_t)(void);

//系统中所有的任务函数
void ExtruderTask(void);
void HeatBedTask(void);

#endif /* __COMMON__H__ */