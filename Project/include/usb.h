#ifndef __USB__H__
#define __USB__H__

#include "common.h"

void USBDevice_Config(void);
bool USBDevice_PlugIn(void);
void USBDevice_Connect(void);
void USBDevice_Disconnect(void);

#endif /* __USB__H__ */