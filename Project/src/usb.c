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
#include "usbcommon.h"
#include "usbstorage.h"

void USBDevice_Config()
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_GPIOClockCmd(USB_Port, ENABLE);

    /* USB Detection Pin Init */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = USB_Det;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;         /* 下拉输入 */
    GPIO_Init(USB_Port, &GPIO_InitStructure);

    USBCommon_Init();
}

bool USBDevice_PlugIn()
{
    // return false;///////////////////
    bool plugin = GPIO_ReadInputDataBit(USB_Port, USB_Det);
    return plugin;
}

void USBDevice_Connect()
{
    /* MAL configuration */
    MAL_Config();
    USB_Init();
}

void USBDevice_Disconnect()
{
    USB_Cable_Config(DISABLE);
}