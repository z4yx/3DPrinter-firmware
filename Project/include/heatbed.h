#ifndef __HEATBED__H__
#define __HEATBED__H__

void HeatBed_Init(void);
void HeatBed_Start_Heating(void);
void HeatBed_Stop_Heating(void);
bool HeatBed_TempReached(void);


#endif /* __HEATBED__H__ */