#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

void SPI_Config(SPI_TypeDef *spix);
unsigned char SPIWriteRead(SPI_TypeDef *spix, unsigned char val);

#endif /* __SPI_H */
