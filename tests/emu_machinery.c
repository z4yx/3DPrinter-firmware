#include "common.h"
#include "limitSwitch.h"

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

struct stepper_t
{
    int64_t steps;
    uint8_t dir;
    uint8_t level;
}emu_steppers[4];

struct tim_t
{
    uint16_t prescaler, period, oc;
    uint16_t count;
    uint8_t en, update_flag, oc_flag;
}tims[8];

uint8_t EMUMACH_GetLimitSwitch(uint8_t sw)
{
    uint8_t motor;
    switch(sw) {
        case LimitSwitch_XMin:
            motor = 0;
            break;
        case LimitSwitch_YMin:
            motor = 1;
            break;
        case LimitSwitch_ZMin:
            motor = 2;
            break;
        default:
            ERR_MSG("Invalid switch number %d", sw);
            asm("int $3");
            return false;
    }
    // printf("limitSwitch %d: state: %d\n", motor, emu_steppers[motor].steps > 0);
    return emu_steppers[motor].steps > 0;
}
void EMUMACH_SetStepperDir(uint8_t motor, uint8_t dir)
{
    emu_steppers[motor].dir = dir;
}
void EMUMACH_SetStepperStepLow(uint8_t motor)
{
    emu_steppers[motor].level = 0;
}
void EMUMACH_SetStepperStepHigh(uint8_t motor)
{
    if(emu_steppers[motor].level == 0){
        emu_steppers[motor].level = 1;
        if(emu_steppers[motor].dir > 0)
            emu_steppers[motor].steps--;
        else
            emu_steppers[motor].steps++;
        // printf("motor %d: steps: %lld\n", motor, emu_steppers[motor].steps);
    }else{
        printf("error: %s\n", "not raise edge");
        asm("int $3");
    }
}
void EMUMACH_TIM_Set(uint8_t tim, uint16_t period, uint16_t prescaler, uint16_t oc)
{
    tims[tim].period = period;
    tims[tim].prescaler = prescaler;
    tims[tim].oc = oc;
}
void EMUMACH_TIM_Cmd(uint8_t tim, uint8_t enable)
{
    tims[tim].en = enable;
}
uint8_t EMUMACH_TIM_UpdateFlag(uint8_t tim)
{
    return tims[tim].update_flag;
}
void EMUMACH_TIM_ClearUpdateFlag(uint8_t tim)
{
    tims[tim].update_flag = 0;
}
uint8_t EMUMACH_TIM_CC1Flag(uint8_t tim)
{
    return tims[tim].oc_flag;
}
void sigroutine(int signo)
{ 
    for (int i = 0; i < 8; ++i)
    {
        if(tims[i].en) {
            // tims[i].count++;
            // if(tims[i].count==1){
            //     tims[i].oc_flag = 1;
            //     tims[i].update_flag=0;
            //     Motor_Interrupt();
            // }else if(tims[i].count==2){
            //     tims[i].oc_flag = 0;
            //     tims[i].update_flag=1;
            //     tims[i].count = 0;
            //     Motor_Interrupt();
            // }
            tims[i].update_flag=1;
            Motor_Interrupt_emu(i);
        }
    }
}
void *thread_tim(void *arg)
{
    for(;;){
        sigroutine(SIGALRM);
        pthread_yield_np();
        // usleep(1000);
    }
    pthread_exit(NULL);
}
void EMUMACH_Init()
{
    for (int i = 0; i < 4; ++i)
    {
        emu_steppers[i].steps = 137; //rand()
    }
    // signal(SIGALRM, sigroutine);
    // ualarm(1000*10,w 1000*5);
    pthread_t thread;
    pthread_create(&thread, NULL, thread_tim, NULL);
}