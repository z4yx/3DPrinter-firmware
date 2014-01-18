#ifndef __HEATBED__H__
#define __HEATBED__H__

void HeatBed_Init(void);
void HeatBed_Start_Heating(void);
void HeatBed_Stop_Heating(void);
bool HeatBed_TempReached(void);
void HeatBed_GetState(int16_t *temp, int *output, bool *heating);


#endif /* __HEATBED__H__ */