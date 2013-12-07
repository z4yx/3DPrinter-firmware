#ifndef __EXTRUDER__H__
#define __EXTRUDER__H__

void Extruder_Init(void);
void Extruder_Start_Heating(void);
void Extruder_Stop_Heating(void);
bool Extruder_TempReached(void);
void Extruder_Extrude(int volume);

#endif /* __EXTRUDER__H__ */