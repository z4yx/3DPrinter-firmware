#ifndef __COMMAND__H__
#define __COMMAND__H__

enum { MACH_STATE_READY, MACH_STATE_HOMING, MACH_STATE_WAIT_HEAT, MACH_STATE_DRAWING, MACH_STATE_ENDED };

bool Command_StartPrinting(const char * file);
bool Command_StopPrinting();
void Command_Init(void);
void Command_GetState(bool *printing, uint16_t *state, uint8_t *progress, bool *usb);

#endif /* __COMMAND__H__ */