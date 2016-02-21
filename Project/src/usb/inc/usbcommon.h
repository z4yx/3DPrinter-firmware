#ifndef _USB_COMMON_H__
#define _USB_COMMON_H__
#include "usb_type.h"
#include "common.h"

#define USB_DISCONNECT                      USB_Port
#define USB_DISCONNECT_PIN                  USB_Conn
#define RCC_APB2Periph_GPIO_DISCONNECT      USB_Port_CLK

void USB_Disconnect_Config(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);

void USBCommon_Init(void);
#endif
