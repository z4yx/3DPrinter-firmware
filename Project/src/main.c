/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  程序入口
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
#include "systick.h"

struct gpio_t{
	GPIO_TypeDef* port;
	uint16_t pin;
};

struct gpio_t TestIO[] = {
	{.port=X_Axis_Step_Port,.pin=X_Axis_Step_Pin},
	{.port=X_Axis_Dir_Port,.pin=X_Axis_Dir_Pin},
	{.port=Y_Axis_Start_Port,.pin=Y_Axis_Start_Pin},
	{.port=Y_Axis_Step_Port,.pin=Y_Axis_Step_Pin},
	{.port=Y_Axis_Dir_Port,.pin=Y_Axis_Dir_Pin},
	{.port=Z_Axis_Start_Port,.pin=Z_Axis_Start_Pin},
	{.port=GPIOC,.pin=GPIO_Pin_0},
	{.port=GPIOC,.pin=GPIO_Pin_1},
	{.port=GPIOB,.pin=GPIO_Pin_9},
	{.port=GPIOC,.pin=GPIO_Pin_14},
	{.port=GPIOB,.pin=GPIO_Pin_3},
	{.port=GPIOB,.pin=GPIO_Pin_5},
	{.port=GPIOB,.pin=GPIO_Pin_4},
	{.port=Z_Axis_Dir_Port,.pin=Z_Axis_Dir_Pin},
	{.port=Z_Axis_Step_Port,.pin=Z_Axis_Step_Pin},
	{.port=X_Axis_Start_Port,.pin=X_Axis_Start_Pin},
	{.port=A_Axis_Dir_Port,.pin=A_Axis_Dir_Pin},
	{.port=A_Axis_Step_Port,.pin=A_Axis_Step_Pin},
	{.port=A_Axis_Start_Port,.pin=A_Axis_Start_Pin},
	{.port=GPIOB,.pin=GPIO_Pin_6},
	{.port=GPIOB,.pin=GPIO_Pin_7},
	{.port=Z_Axis_Max_Port,.pin=Z_Axis_Max_Pin},
	{.port=Z_Axis_Min_Port,.pin=Z_Axis_Min_Pin},
	{.port=Y_Axis_Max_Port,.pin=Y_Axis_Max_Pin},
	{.port=Y_Axis_Min_Port,.pin=Y_Axis_Min_Pin},
	
	{.port=GPIOA,.pin=GPIO_Pin_10},
	{.port=GPIOA,.pin=GPIO_Pin_9},
	{.port=X_Axis_Max_Port,.pin=X_Axis_Max_Pin},
	{.port=X_Axis_Min_Port,.pin=X_Axis_Min_Pin},
	{.port=GPIOA,.pin=GPIO_Pin_1},
	{.port=GPIOA,.pin=GPIO_Pin_0},
	{.port=Ex1Fan_Port,.pin=Ex1Fan_Pin},
	{.port=LED_Port,.pin=LED_1},
};

int main(void)
{
	int i;
    GPIO_InitTypeDef GPIO_InitStructure;
	SystemCoreClockUpdate();
	SysTick_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	for(i = 0; i < sizeof(TestIO)/sizeof(TestIO[0]); i++){
	    GPIO_InitStructure.GPIO_Pin = TestIO[i].pin;
	    GPIO_Init(TestIO[i].port, &GPIO_InitStructure);
	}

	while (1)
	{
		for(i = 0; i < sizeof(TestIO)/sizeof(TestIO[0]); i++){
		    GPIO_SetBits(TestIO[i].port, TestIO[i].pin);
		    Delay_ms(100);
		}
		for(i = 0; i < sizeof(TestIO)/sizeof(TestIO[0]); i++){
		    GPIO_ResetBits(TestIO[i].port, TestIO[i].pin);
		    Delay_ms(100);
		}
	}
}

#ifdef  USE_FULL_ASSERT

/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}

#endif
