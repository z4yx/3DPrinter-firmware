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
#include "command.h"
#include "gfiles.h"
#include "move.h"
#include "heatbed.h"
#include "extruder.h"

const Task_t SystemTasks[] = { LimitSwitch_Task, ExtruderTask, HeatBedTask, KeyBoard_Task, Command_Task };


static void periphInit()
{
	FileManager_Init();
	PWM_Init(HEATER_PWM_FREQ);
	Move_Init();
	Extruder_Init();
	HeatBed_Init();
	KeyBoard_Init();
	Command_Init();
}

void useHSIClock()
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
	// useHSIClock();
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_GetClocksFreq(&clocks);

	coreInit();

	Delay_ms(2000);

	DBG_MSG("\r\n\r\n", 0);
	DBG_MSG("Clock Source: %d", RCC_GetSYSCLKSource());
	DBG_MSG("SYSCLK: %d, H: %d, P1: %d, P2: %d",
		clocks.SYSCLK_Frequency,
		clocks.HCLK_Frequency,
		clocks.PCLK1_Frequency,
		clocks.PCLK2_Frequency);

	periphInit();

	do{
		char (*files)[][SD_MAX_FILENAME_LEN]
			= FileManager_ListGFiles();
		if(files != 0){
			for(int i=0; i<SD_MAX_ITEMS; i++){
				if(!(*files)[i][0])
					break;
				REPORT(INFO_LIST_FILES, "%s", (*files)[i]);
			}
		}
	}while(0);

	Command_StartPrinting("box.g");

	uint8_t led_state = LED_ON;
	SysTick_t last_report = 0;
	while (1)
	{

		//运行系统中声明的任务
		for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
			(SystemTasks[i])();

		SysTick_t now = GetSystemTick();
		if(now - last_report > REPORT_PERIOD){
			int16_t temp;
			uint16_t state;
			uint8_t progress;
			int output;
			bool b;

			last_report = now;

			Command_GetState(&b, &state, &progress);
			REPORT(INFO_PRINT, "%d,%d,%d", (int)b, (int)state, (int)progress);

			Extruder_GetState(&temp, &output, &b);
			REPORT(INFO_EXTRUDER, "%d,%d,%d", (int)temp, (int)output, (int)b);

			HeatBed_GetState(&temp, &output, &b);
			REPORT(INFO_HEATBED, "%d,%d,%d", (int)temp, (int)output, (int)b);

			LED_Enable(LED2, led_state);
			led_state = (led_state == LED_ON ? LED_OFF : LED_ON);
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
