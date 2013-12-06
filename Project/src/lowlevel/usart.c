/*
 * =====================================================================================
 *
 *       Filename:  usart.c
 *
 *    Description:  USART1串口收发与调试输出
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
#include "usart.h"
#include <stdarg.h>

/*
 * 初始化串口配置
 */
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* USART1 GPIO config */
    RCC_GPIOClockCmd(GPIOA, ENABLE);
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

/*
 * 通过串口发送一个字节
 */
int USART_putchar(int ch)
{

    USART_SendData(USART1, (unsigned char) ch);
    while (!(USART1->SR & USART_FLAG_TXE));

    return (ch);
}

/*
 * 通过串口接收一个字节
 */
int USART_getchar()
{
    uint8_t ch;
    while (SET != USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
    ch = USART_ReceiveData(USART1);
    USART_ClearFlag(USART1, USART_FLAG_RXNE);
    return (ch);
}

/*
 * 读入一个整数值
 */
int USART_ReadInt()
{
    uint8_t neg = 0;
    int c = USART_getchar(), ret = 0;
    while(c<'0'||c>'9'){
        if(c=='-')
            neg = 1;
        c = USART_getchar();
    }
    while(c>='0'&&c<='9'){
        ret = ret*10+(c-'0');
        c = USART_getchar();
    }
    return neg ? -ret : ret;
}

/*
 * 数值转字符串函数
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 1000000000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * 通过串口发送格式化字符串
 */
void USART_printf(uint8_t *Data, ...)
{
    const char *s;
    int d;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)
    {
        if ( *Data == '\\' )
        {
            switch ( *++Data )
            {
            case 'r':
                USART_SendData(USART1, 0x0d);
                Data ++;
                break;

            case 'n':
                USART_SendData(USART1, 0x0a);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            switch ( *++Data )
            {
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    USART_SendData(USART1, *s);
                    while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;

            case 'd':
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    USART_SendData(USART1, *s);
                    while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else USART_SendData(USART1, *Data++);
        while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
    }
}
