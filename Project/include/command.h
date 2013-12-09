#ifndef __COMMAND__H__
#define __COMMAND__H__

enum { MACH_STATE_READY, MACH_STATE_HOMING, MACH_STATE_WAIT_HEAT, MACH_STATE_DRAWING, MACH_STATE_ENDED };

bool Command_StartPrinting(const char * file);
void Command_Init(void);

#endif /* __COMMAND__H__ */