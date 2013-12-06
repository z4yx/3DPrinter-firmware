/*
 * =====================================================================================
 *
 *       Filename:  configure.h
 *
 *    Description:  所有可配置的参数汇总
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

//X轴步进电机旋转一周对应的直线位移(um)
#define X_DISTANCE_PER_CYCLE 5000
//X轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define X_PULSES_PER_CYCLE   (16*200)
//X轴运行方向调整(取值+/-1)
#define X_DIRECTION_ADJ      1 

//Y轴步进电机旋转一周对应的直线位移(um)
#define Y_DISTANCE_PER_CYCLE 5000
//Y轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define Y_PULSES_PER_CYCLE (16*200)
//Y轴运行方向调整(取值+/-1)
#define Y_DIRECTION_ADJ      1 

//Z轴步进电机旋转一周对应的直线位移(um)
#define Z_DISTANCE_PER_CYCLE 5000
//Z轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define Z_PULSES_PER_CYCLE (16*200)
//Z轴运行方向调整(取值+/-1)
#define Z_DIRECTION_ADJ      1

//限位开关最小触发间隔时间(ms)
#define LIMIT_SWITCH_MIN_TOGGLE_PERIOD 30

//加热器PWM输出频率(单位Hz)
#define HEATER_PWM_FREQ 50

//挤出器默认温度
#define EXTRUDER_DEFAULT_TEMP 240
//挤出器温度PID控制参数
#define EXTRUDER_PID_KP 800
#define EXTRUDER_PID_KI 25
#define EXTRUDER_PID_KD 700
#define EXTRUDER_PID_INIT_SUM 800

//加热板默认温度
#define HEATBED_DEFAULT_TEMP 100

//步进电机脉冲频率(单位Hz)
#define MOTOR_STEP_FREQ 2000

