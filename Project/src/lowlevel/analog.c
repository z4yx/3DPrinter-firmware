#include "analog.h"
#include "adc.h"

static bool calibrated;
static bool enabled_channels[16];
static uint8_t channel_rank[16]; //1-based
static volatile uint16_t values[16];

static const uint16_t chan2io[16] = {
    GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7,GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5
};

static void getIOByChannel(uint8_t channel, GPIO_TypeDef** GPIOx, uint16_t* pin)
{
    if(channel <= 7)
        *GPIOx = GPIOA;
    else if(channel <= 9)
        *GPIOx = GPIOB;
    else if(channel <= 15)
        *GPIOx = GPIOC;
    *pin = chan2io[channel];
}

static void refresh_adc_channels()
{
    uint8_t n = 0, t = 0;
    for (uint8_t i = 0; i < sizeof(enabled_channels); ++i)
    {
        if(enabled_channels[i])
            n++;
    }
    ADC_Config(n);

    if(n == 0)
        return;

    for (uint8_t i = 0; i < sizeof(enabled_channels); ++i)
    {
        if(enabled_channels[i]){
            GPIO_TypeDef* gpio;
            uint16_t pin;
            getIOByChannel(i, &gpio, &pin);
            ADC_Channel_Config(gpio, pin, i, ++t);
            channel_rank[i] = t;
            DBG_MSG("(%d) ADC channel %d", t, i);
        }
    }
    ADC_DMA_Init(values, n);
    if(!calibrated){
        ADC_Prepare();
        calibrated = true;
    }
    ADC_StartSample();
    Delay_us(100);
}

uint16_t Analog_GetChannelValue(uint8_t channel)
{
    if(channel >= sizeof(enabled_channels))
        return 0;
    return values[channel_rank[channel]-1];
}

void Analog_SetChannel(uint8_t channel, bool enabled)
{
    if(channel >= sizeof(enabled_channels))
        return;
    if(enabled_channels[channel] == enabled) //Not changed
        return;
    enabled_channels[channel] = enabled;
    refresh_adc_channels();
}

