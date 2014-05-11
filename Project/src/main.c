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
#include "fanControl.h"
#include "adc.h"
#include "motor.h"
#include "usb.h"
#include "gfiles.h"
#include "move.h"
#include "hostctrl.h"

const Task_t SystemTasks[] = { LimitSwitch_Task, HostCtrl_Task};


static void periphInit()
{
	USART_Config(BT_USART, BT_BaudRate);
	FileManager_Init();
	PWM_Init(HEATER_PWM_FREQ);
	Fan_Config();

	ADC_Config(1);
	ADC_Channel_Config(HeaterBoardTherm_Port, HeaterBoardTherm_Pin, HeaterBoardTherm_ADCChannel, 1);
	ADC_Start();

	MAX6675_Config();
	Move_Init();
	USBDevice_Config();
	HostCtrl_Init();
}

//核心组件初始化,包括串口(用于打印调试信息)
static void coreInit()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	SystemCoreClockUpdate();
	SysTick_Init();
	LED_Config();
	USART_Config(Debug_USART, Debug_BaudRate);

	//enable remap for max6675 pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

int main(void)
{
	RCC_PCLK1Config(RCC_HCLK_Div1);

	coreInit();

	Delay_ms(2000);

	periphInit();

	DBG_MSG("Peripheral init done.", 0);

	while (1)
	{

		//运行系统中声明的任务
		for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
			(SystemTasks[i])();
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
