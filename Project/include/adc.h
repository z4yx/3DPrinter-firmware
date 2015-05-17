#ifndef __ADC_H___
#define __ADC_H___

#include "stm32f10x.h"

void ADC_Config(uint8_t nChannel);
void ADC_Channel_Config(GPIO_TypeDef* GPIOx, uint16_t pin, uint8_t channel, uint8_t rank);
void ADC_Prepare(void);
void ADC_DMA_Init(volatile uint16_t *ADCConvertedValues, uint32_t size);
void ADC_StartSample(void);

#endif