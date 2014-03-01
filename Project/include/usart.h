#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"

void USART_Config(USART_TypeDef* USARTx, u32 USART_BaudRate);
void USART_RxInt_Config(bool bEnabled);
int USART_ReadInt(USART_TypeDef* USARTx);
int USART_putchar(USART_TypeDef* USARTx, int ch);
int USART_getchar(USART_TypeDef* USARTx);
void USART_printf(USART_TypeDef* USARTx, char *Data, ...);

#endif
