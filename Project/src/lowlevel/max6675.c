/*
 * =====================================================================================
 *
 *       Filename:  max6675.c
 *
 *    Description:  MAX6675热电偶转换芯片读取
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
#include "max6675.h"
#include "systick.h"

/*
 * MAX6675引脚初始化
 */
void MAX6675_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_GPIOClockCmd(Thermcouple_Port, ENABLE);    //GPIO时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //口线翻转速度为50MHz
    GPIO_InitStructure.GPIO_Pin = ThermcoupleSck_Pin | ThermcoupleCs_Pin;

    GPIO_Init(Thermcouple_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = ThermcoupleDo_Pin;

    GPIO_Init(Thermcouple_Port, &GPIO_InitStructure);
}

/*
 * 获取温度，如果没有传感器，返回-1
 */
int16_t MAX6675_Read_Value()
{
    uint16_t data = 0;
    uint32_t i;
    bool noSensor = false;

    GPIO_ResetBits(Thermcouple_Port, ThermcoupleSck_Pin);
    GPIO_ResetBits(Thermcouple_Port, ThermcoupleCs_Pin);
    
    Delay_us(20);

    for (i = 0; i < 16; i++)
    {
        GPIO_SetBits(Thermcouple_Port, ThermcoupleSck_Pin);
        Delay_us(1);

        // 温度值整数部分10bit
        if (i >= 1 && i < 11)
        {
            data = data << 1;
            if (GPIO_ReadInputDataBit(Thermcouple_Port, ThermcoupleDo_Pin))
            {
                data |= 0x01;
            }
        }
        if (i == 13)
        {
            if (GPIO_ReadInputDataBit(Thermcouple_Port, ThermcoupleDo_Pin))
                noSensor = true;
        }
        Delay_us(1);

        GPIO_ResetBits(Thermcouple_Port, ThermcoupleSck_Pin);
        Delay_us(2);
    }

    GPIO_SetBits(Thermcouple_Port, ThermcoupleCs_Pin);
    Delay_us(20);
    GPIO_ResetBits(Thermcouple_Port, ThermcoupleSck_Pin);

    return noSensor ? -1 : data;
}