#include "spi.h"
#include "common.h"

void SPI_Config(SPI_TypeDef *spix)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef * spi_port;
    uint16_t pins_in, pins_out, pins_af;

    if(spix == SPI1){
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    	spi_port = GPIOA;
    	pins_in = GPIO_Pin_6;
    	pins_af = GPIO_Pin_5|GPIO_Pin_7;
    	pins_out= GPIO_Pin_4;
    }else if(spix == SPI2){
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    	spi_port = GPIOB;
    	pins_in = GPIO_Pin_14;
    	pins_af = GPIO_Pin_13|GPIO_Pin_15;
    	pins_out= GPIO_Pin_12;
    }else if(spix == SPI3){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        spi_port = GPIOB;
        pins_in = GPIO_Pin_4;
        pins_af = GPIO_Pin_3|GPIO_Pin_5;
        pins_out= 0;
    }else{
        ERR_MSG("Unknown SPI peripheral");
    	return;
    }

    GPIO_InitStructure.GPIO_Pin = pins_in;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = pins_af;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi_port, &GPIO_InitStructure);

    if(pins_out){
        GPIO_InitStructure.GPIO_Pin = pins_out;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(spi_port, &GPIO_InitStructure);
    }

    GPIO_SetBits(spi_port, pins_out);

    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler =
        SPI_BaudRatePrescaler_16;
    SPI_Init(spix, &SPI_InitStructure);
    SPI_Cmd(spix, ENABLE);
}

unsigned char SPIWriteRead(SPI_TypeDef *spix, unsigned char val)
{
    while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_TXE) == RESET);

    SPI_I2S_SendData(spix, val);

    while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_RXNE) == RESET);
    
    return SPI_I2S_ReceiveData(spix);
}