#ifndef __COMMON__H__
#define __COMMON__H__

#include "configure.h"
#include "hardwareDef.h"
#include <stdio.h>
#include <stdint.h>

#define SystemCoreClock 72000000

typedef int TIM_TypeDef;

typedef enum bool_t{FALSE = 0, TRUE = !FALSE}bool;
#define true TRUE
#define false FALSE

#define DBG_MSG(format, ...) printf("[Debug]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define ERR_MSG(format, ...) printf("[Error]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define REPORT(info_type, format, ...) printf("!I#%s#" format "\r\n", info_type, ##__VA_ARGS__)

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

#define RCC_GPIOClockCmd(x) ((void)0)
#define RCC_USARTClockCmd(x) ((void)0)
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