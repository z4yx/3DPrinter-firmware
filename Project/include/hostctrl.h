#ifndef __HOSTCTRL_H
#define	__HOSTCTRL_H

#include "stm32f10x.h"

void HostCtrl_Init(void);
bool HostCtrl_GetCmd(char **p_cmd, char **p_param);
void HostCtrl_Interrupt(void);
void HostCtrl_CmdProcessed(void);

#endif
