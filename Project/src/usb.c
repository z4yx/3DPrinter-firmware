/*
 * =====================================================================================
 *
 *       Filename:  usb.c
 *
 *    Description:  usb相关操作封装
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
#include "usb.h"
#include "usb_lib.h"
#include "sd.h"


/* 被usb库使用 */
u32 Mass_Memory_Size;
u32 Mass_Block_Size;
u32 Mass_Block_Count;

/*
 * 函数名：Set_USBClock
 * 描述  ：配置USB时钟(48M)
 * 输入  ：无
 * 输出  ：无
 */
static void Set_USBClock(void)
{
    /* USBCLK = PLLCLK */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

    /* Enable USB clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/* 配置USB中断 */
static void USB_Interrupts_Config(FunctionalState cmd)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
#error stub
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;
    NVIC_Init(&NVIC_InitStructure);
}

/* 选择是否连接1.5K电阻以被主机检测*/
void USB_Cable_Config (FunctionalState NewState)
{

    //  if (NewState != DISABLE)
    //  {
    //    GPIO_ResetBits(GPIOE, GPIO_Pin_3);                    /* USB全速模式 */
    //  }
    //  else
    //  {
    //    GPIO_SetBits(GPIOE, GPIO_Pin_3);                      /* 普通模式 */
    //  }
}

static void InitMemoryInfo()
{
    Mass_Block_Count = SD_GetCapacity();
    Mass_Block_Size = 512;

    Mass_Memory_Size = (Mass_Block_Count * Mass_Block_Size);
}

void USBDevice_Config()
{

    // GPIO_InitTypeDef GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      /* 开漏输出 */
    // GPIO_Init(GPIOE, &GPIO_InitStructure);

    // GPIO_SetBits(GPIOE, GPIO_Pin_3);

    Set_USBClock();
}

bool USBDevice_PlugIn()
{
    return true;
}

void USBDevice_Connect()
{
    InitMemoryInfo();
    USB_Interrupts_Config(ENABLE);
    USB_Cable_Config(ENABLE);
    USB_Init();
}

void USBDevice_Disconnect()
{
    USB_Interrupts_Config(DISABLE);
    USB_Cable_Config(DISABLE);
}