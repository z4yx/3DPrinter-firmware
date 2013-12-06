/*
 * =====================================================================================
 *
 *       Filename:  adc.c
 *
 *    Description:  A/D转换器操作封装(使用ADC1)
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
#include "adc.h"

/*
 * ADC1初始化，指定通道数。注意，初始化之后必须分别配置每个通道。
 */
void ADC_Config(uint8_t nChannel)
{
	ADC_InitTypeDef  ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_CFGR_ADCPRE_DIV6);//ADC时钟PLCK2的6分频 12M。ADC最大时钟不能超过14M!
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//有软件控制转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = nChannel;

	ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC
}

void ADC_Channel_Config(GPIO_TypeDef* GPIOx, uint16_t pin, uint8_t channel, uint8_t rank)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_GPIOClockCmd(GPIOx, ENABLE);  //GPIO时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = pin;

	GPIO_Init(GPIOx, &GPIO_InitStructure);

	ADC_RegularChannelConfig(ADC1, channel, rank, ADC_SampleTime_71Cycles5);
}

void ADC_Start()
{
	ADC_Cmd(ADC1, ENABLE);//使能ADC1
	
	ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
	ADC_StartCalibration(ADC1);//开始ADC1校准
	while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换
}

/*
 * 获取最近一次转换的结果，如果没有，返回-1
 */
int16_t ADC_Read_Value()
{
	int16_t adc = -1;
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET)
	{
		adc = ADC_GetConversionValue(ADC1);
	}
	return adc;
}