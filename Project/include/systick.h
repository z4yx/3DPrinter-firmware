#ifndef __SYSTICK_H
#define	__SYSTICK_H

typedef uint64_t SysTick_t;

void SysTick_Init(void);
void IncSysTickCounter(void);
SysTick_t GetSystemTick(void);
void Delay_ms(unsigned int ms);
void Delay_us(unsigned int us);

#endif