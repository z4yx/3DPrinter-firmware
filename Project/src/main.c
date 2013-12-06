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
#include "led.h"
#include "usart.h"
#include "systick.h"
#include "pwmOutput.h"
#include "lcdSerial.h"
#include "fanControl.h"
#include "adc.h"
#include "motor.h"

extern int test_var;

const Task_t SystemTasks[] = { ExtruderTask, HeatBedTask };


static void periphInit()
{
	lcdSerialInit();
	FileManager_Init();
	PWM_Init(HEATER_PWM_FREQ);
	Move_Init();
	Extruder_Init();
	HeatBed_Init();

}

void clockTest()
{
	// RCC_HSEConfig(RCC_HSE_ON);
	// while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)//等待HSE使能成功
	// {
	// }
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);//等待HSI使能成功

	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_10);

	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
}

//核心组件初始化,包括串口(用于打印调试信息)
static void coreInit()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	SystemCoreClockUpdate();
	SysTick_Init();
	LED_Config();
	USART_Config();
}

int main(void)
{
	RCC_ClocksTypeDef clocks;
	clockTest();
	RCC_GetClocksFreq(&clocks);

	coreInit();

	USART_printf("\r\n\r\n");
	USART_printf("Clock Source: %d\r\n", RCC_GetSYSCLKSource());
	USART_printf("SYSCLK: %d, H: %d, P1: %d, P2: %d\r\n",
		clocks.SYSCLK_Frequency,
		clocks.HCLK_Frequency,
		clocks.PCLK1_Frequency,
		clocks.PCLK2_Frequency);

	periphInit();

	lcdSerialClear();
	lcdSerialSetCursor(0,0);
	lcdSerialWriteString("Hello World!");

	Motor_PowerOn();

	Move_Home(X_Axis);
	Move_Home(Y_Axis);
	Move_Home(Z_Axis);

	Extruder_Start_Heating();
	HeatBed_Start_Heating();

	do{
		char (*files)[][SD_MAX_FILENAME_LEN]
			= FileManager_ListGFiles();
		if(files != 0){
			for(int i=0; i<SD_MAX_ITEMS; i++){
				if(!(*files)[i][0])
					break;
				USART_printf("GCode file: %s\r\n", (*files)[i]);
			}
		}
	}while(0);

	while (1)
	{
		int c = USART_ReadInt();


		//运行系统中声明的任务
		for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
			(SystemTasks[i])();
		
		// int ch = USART_getchar();

		LED_Enable(LED1, LED_OFF);
		LED_Enable(LED2, LED_ON);

		// Delay_ms(1000);
		// Delay_us(1000000);

		// LED_Enable(LED1, LED_ON);
		// LED_Enable(LED2, LED_OFF);

		// Delay_us(1000000);
		// Delay_ms(1000);

		// USART_putchar(ch);
		// USART_printf("adc %d %d %d\r\n", (int)(GetSystemTick()), (int)ADC_Read_Value(), (int)MAX6675_Read_Value());
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
