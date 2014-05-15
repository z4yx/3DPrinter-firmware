#ifndef __HEATBED__H__
#define __HEATBED__H__

void HeatBed_Init(void);
void HeatBed_Start_Heating(uint16_t _target);
void HeatBed_Stop_Heating(void);
bool HeatBed_TempReached(void);
void HeatBed_GetState(int16_t *temp, int *output, bool *heating);

void HeatBed_SetOutput(int output);

#endif /* __HEATBED__H__ */