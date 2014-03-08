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
#include "sdio.h"


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

    if (NewState != DISABLE)
    {
        GPIO_ResetBits(USB_Port, USB_Conn);
    }
    else
    {
        GPIO_SetBits(USB_Port, USB_Conn);
    }
}

static bool InitMemoryInfo()
{
    SD_CardInfo card_info;
    if (SD_Init() != SD_OK)
    {
        ERR_MSG("Failed to init card.", 0);
        return false;
    }

    if (SD_GetCardInfo( &card_info ) != SD_OK){
        ERR_MSG("Failed to get card info.", 0);
        return false;
    }

    Mass_Block_Count = card_info.CardCapacity / card_info.CardBlockSize;
    Mass_Block_Size = card_info.CardBlockSize;

    Mass_Memory_Size = card_info.CardCapacity;
}

void USBDevice_Config()
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_GPIOClockCmd(USB_Port, ENABLE);

    GPIO_InitStructure.GPIO_Pin = USB_Conn;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      /* 开漏输出 */
    GPIO_Init(USB_Port, &GPIO_InitStructure);
    GPIO_SetBits(USB_Port, USB_Conn);

    GPIO_InitStructure.GPIO_Pin = USB_Det;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;         /* 下拉输入 */
    GPIO_Init(USB_Port, &GPIO_InitStructure);

    Set_USBClock();
}

bool USBDevice_PlugIn()
{
    // return false;///////////////////
    bool plugin = GPIO_ReadInputDataBit(USB_Port, USB_Det);
    return plugin;
}

void USBDevice_Connect()
{
    DBG_MSG("called", 0);
    if(!InitMemoryInfo())
        return;
    USB_Interrupts_Config(ENABLE);
    USB_Cable_Config(ENABLE);
    USB_Init();
}

void USBDevice_Disconnect()
{
    DBG_MSG("called", 0);
    USB_Interrupts_Config(DISABLE);
    USB_Cable_Config(DISABLE);
}