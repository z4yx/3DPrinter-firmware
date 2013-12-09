#ifndef __EXTRUDER__H__
#define __EXTRUDER__H__

void Extruder_Init(void);
void Extruder_Start_Heating(void);
void Extruder_Stop_Heating(void);
bool Extruder_TempReached(void);

#endif /* __EXTRUDER__H__ */