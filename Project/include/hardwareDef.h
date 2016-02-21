/*
 * =====================================================================================
 *
 *       Filename:  hardwareDef.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013/1/18 10:28:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  madcc (good lucky), madcc09@163.com
 *                  zhangyuxiang
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __HARDWARE_DEF__HH__
#define __HARDWARE_DEF__HH__

#define Debug_USART USART1
#define Debug_BaudRate 115200
#define BT_USART USART3
#define BT_BaudRate 9600

#define Ex1Fan_Pin GPIO_Pin_15
#define Ex1Fan_Port GPIOC

#define PWM2_Port    GPIOA
#define PWM2_Ch1_Pin GPIO_Pin_0
#define PWM2_Ch2_Pin GPIO_Pin_1
#define PWM2_Ch3_Pin GPIO_Pin_2
#define PWM2_Ch4_Pin GPIO_Pin_3

#define Ex1Heat_Ch   1
#define HBP_Ch       2

/*Defined by PWM*/
// #define Ex1Heat_Pin GPIO_Pin_0
// #define HBP_Pin GPIO_Pin_1
// #define HBP_Port GPIOA

#define ThermcoupleSck_Pin GPIO_Pin_5
#define ThermcoupleCs_Pin GPIO_Pin_4
#define ThermcoupleDo_Pin GPIO_Pin_3
#define Thermcouple_Port GPIOB

#define HeaterBoardTherm_ADC_Ch 11
#define Extruder1Therm_ADC_Ch   10

#define X_Axis_Start_Pin GPIO_Pin_6
#define X_Axis_Start_Port GPIOA
#define X_Axis_Step_Pin GPIO_Pin_5
#define X_Axis_Step_Port GPIOC
#define X_Axis_Dir_Pin GPIO_Pin_4
#define X_Axis_Dir_Port GPIOC

#define Y_Axis_Start_Pin GPIO_Pin_2
#define Y_Axis_Start_Port GPIOA
#define Y_Axis_Step_Pin GPIO_Pin_5
#define Y_Axis_Step_Port GPIOA
#define Y_Axis_Dir_Pin GPIO_Pin_4
#define Y_Axis_Dir_Port GPIOA

#define Z_Axis_Start_Pin GPIO_Pin_13
#define Z_Axis_Start_Port GPIOC
#define Z_Axis_Step_Pin GPIO_Pin_3
#define Z_Axis_Step_Port GPIOC
#define Z_Axis_Dir_Pin GPIO_Pin_2
#define Z_Axis_Dir_Port GPIOC

#define A_Axis_Start_Pin GPIO_Pin_0
#define A_Axis_Start_Port GPIOB
#define A_Axis_Step_Pin GPIO_Pin_13
#define A_Axis_Step_Port GPIOB
#define A_Axis_Dir_Pin GPIO_Pin_12
#define A_Axis_Dir_Port GPIOB

#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2
#define A_Axis 3

#define X_Axis_Min_Port GPIOB
#define X_Axis_Min_Pin GPIO_Pin_14
#define Y_Axis_Min_Port GPIOB
#define Y_Axis_Min_Pin GPIO_Pin_15
#define Z_Axis_Min_Port GPIOC
#define Z_Axis_Min_Pin GPIO_Pin_6

#define X_Axis_Max_Port GPIOA
#define X_Axis_Max_Pin GPIO_Pin_3
#define Y_Axis_Max_Port GPIOA
#define Y_Axis_Max_Pin GPIO_Pin_7
#define Z_Axis_Max_Port GPIOB
#define Z_Axis_Max_Pin GPIO_Pin_1

#define LED_Port GPIOB
#define LED_1 GPIO_Pin_8

#define USB_Port GPIOA
#define USB_Det  GPIO_Pin_15
#define USB_Conn GPIO_Pin_8
#define USB_Port_CLK RCC_APB2Periph_GPIOA

#define STEPPER_COUNT 4

#define SD_DETECT_PIN                    GPIO_Pin_7
#define SD_DETECT_GPIO_PORT              GPIOC
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOC
#endif
