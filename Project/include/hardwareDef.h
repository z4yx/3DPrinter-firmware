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

#define Ex1Fan_Pin GPIO_Pin_8
#define Ex1Heat_Pin GPIO_Pin_0
#define HBP_Pin GPIO_Pin_1
#define Ex1_Port GPIOA
#define HBP_Port GPIOA

#define ThermcoupleSck_Pin GPIO_Pin_9
#define ThermcoupleCs_Pin GPIO_Pin_8
#define ThermcoupleDo_Pin GPIO_Pin_7
#define Thermcouple_Port GPIOC

#define HeaterBoardTherm_Pin GPIO_Pin_1
#define HeaterBoardTherm_Port GPIOC
#define HeaterBoardTherm_ADCChannel ADC_Channel_11

#define X_Axis_Start_Pin GPIO_Pin_11
#define X_Axis_Start_Port GPIOB
#define X_Axis_Step_Pin GPIO_Pin_1
#define X_Axis_Step_Port GPIOB
#define X_Axis_Dir_Pin GPIO_Pin_10
#define X_Axis_Dir_Port GPIOB

#define Y_Axis_Start_Pin GPIO_Pin_0
#define Y_Axis_Start_Port GPIOB
#define Y_Axis_Step_Pin GPIO_Pin_4
#define Y_Axis_Step_Port GPIOC
#define Y_Axis_Dir_Pin GPIO_Pin_5
#define Y_Axis_Dir_Port GPIOC

#define Z_Axis_Start_Pin GPIO_Pin_7
#define Z_Axis_Start_Port GPIOA
#define Z_Axis_Step_Pin GPIO_Pin_5
#define Z_Axis_Step_Port GPIOA
#define Z_Axis_Dir_Pin GPIO_Pin_6
#define Z_Axis_Dir_Port GPIOA

#define A_Axis_Start_Pin GPIO_Pin_4
#define A_Axis_Start_Port GPIOA
#define A_Axis_Step_Pin GPIO_Pin_2
#define A_Axis_Step_Port GPIOA
#define A_Axis_Dir_Pin GPIO_Pin_3
#define A_Axis_Dir_Port GPIOA

#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2
#define A_Axis 3

#define X_Axis_Min_Port GPIOC
#define X_Axis_Min_Pin GPIO_Pin_13
#define Y_Axis_Min_Port GPIOC
#define Y_Axis_Min_Pin GPIO_Pin_14
#define Z_Axis_Min_Port GPIOC
#define Z_Axis_Min_Pin GPIO_Pin_15

#define LCD_Port GPIOB
#define LCD_DB4 GPIO_Pin_6
#define LCD_DB5 GPIO_Pin_5
#define LCD_DB6 GPIO_Pin_4
#define LCD_DB7 GPIO_Pin_3
#define LCD_RS GPIO_Pin_9
#define LCD_RW GPIO_Pin_8
#define LCD_E GPIO_Pin_7

#define LED_Port GPIOC
#define LED_1 GPIO_Pin_3
#define LED_2 GPIO_Pin_2

#define Btn_Left_Pin GPIO_Pin_15
#define Btn_Left_Port GPIOA
#define Btn_Down_Pin GPIO_Pin_10
#define Btn_Down_Port GPIOC
#define Btn_Right_Pin GPIO_Pin_2
#define Btn_Right_Port GPIOD
#define Btn_Up_Pin GPIO_Pin_12
#define Btn_Up_Port GPIOC
#define Btn_Center_Pin GPIO_Pin_11
#define Btn_Center_Port GPIOC


#define STEPPER_COUNT 4

/// Character LCD screen geometry
#define LCD_SCREEN_WIDTH        20
#define LCD_SCREEN_HEIGHT       4


#endif
