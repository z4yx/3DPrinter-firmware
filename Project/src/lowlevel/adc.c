#include "adc.h"
#include "common.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

/*
 * ADC1初始化，指定通道数。注意，初始化之后必须分别配置每个通道。
 */
void ADC_Config(uint8_t nChannel)
{
    ADC_InitTypeDef  ADC_InitStructure;
    ADC_StructInit(&ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_CFGR_ADCPRE_DIV6);//ADC时钟PLCK2的6分频 12M。ADC最大时钟不能超过14M!
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_Cmd(ADC1,DISABLE);

    if(nChannel == 0)
        return;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件控制转换
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

    ADC_RegularChannelConfig(ADC1, channel, rank, ADC_SampleTime_13Cycles5);
}

void ADC_DMA_Init(volatile uint16_t *ADCConvertedValues, uint32_t size)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValues;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
}

void ADC_Prepare()
{
    ADC_Cmd(ADC1, ENABLE);//使能ADC1
    
    ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
    ADC_StartCalibration(ADC1);//开始ADC1校准
    while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
}

void ADC_StartSample()
{
    ADC_Cmd(ADC1, ENABLE);//使能ADC1
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ADC1软件转换
}
