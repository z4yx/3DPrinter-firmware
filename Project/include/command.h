#ifndef __COMMAND__H__
#define __COMMAND__H__

enum { MACH_STATE_READY, MACH_STATE_HOMING, 
	MACH_STATE_WAIT_HEAT, MACH_STATE_DRAWING, 
	MACH_STATE_ENDED,
	MACH_STATE_WAIT_EXTRUDER, MACH_STATE_WAIT_HEATBED };
enum { MACH_MODE_STANDBY, MACH_MODE_PRINT, MACH_MODE_USB};

bool Command_StartPrinting(const char * file);
bool Command_StopPrinting();
void Command_Init(void);
void Command_GetState(uint8_t *mode, uint16_t *state, uint8_t *progress);
bool Command_IsStandBy(void);
bool Command_ManuallyHome(uint8_t selected_dir);

#endif /* __COMMAND__H__ */