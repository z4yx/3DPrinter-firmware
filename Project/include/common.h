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

//for NULL and bool
#include "usb_type.h"
#define true TRUE
#define false FALSE

extern void USART_printf(USART_TypeDef* USARTx, char *Data, ...);

#define DBG_MSG(format, ...) USART_printf(Debug_USART, "[Debug]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define ERR_MSG(format, ...) USART_printf(Debug_USART, "[Error]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define REPORT(info_type, format, ...) USART_printf(BT_USART, "!I#%s#" format "\r\n", info_type, ##__VA_ARGS__)

#define INFO_LIST_FILES  "LIST"
#define INFO_EXTRUDER    "EX1"
#define INFO_HEATBED     "BED1"
#define INFO_PRINT       "PRT"
#define INFO_G_G0        "G0"
#define INFO_G_G1        "G1"
#define INFO_G_G92       "G92"
#define INFO_G_G161      "G161"
#define INFO_G_M6        "M6"
#define INFO_G_M18       "M18"
#define INFO_G_M73       "M73"
#define INFO_REPLY       "RE"
 
void RCC_GPIOClockCmd(GPIO_TypeDef* GPIOx, FunctionalState state);
void RCC_USARTClockCmd(USART_TypeDef* USARTx, FunctionalState state);
void Timer_16bit_Calc(int freq, uint16_t *period, uint16_t *prescaler);
float Distance3D(float dx, float dy, float dz);

typedef void (*Task_t)(void);

//系统中所有的任务函数
void ExtruderTask(void);
void HeatBedTask(void);
void Command_Task(void);
void LimitSwitch_Task(void);
void HostCtrl_Task(void);

#endif /* __COMMON__H__ */