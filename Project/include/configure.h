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

#define MACHINERY_CARTESIAN   1
#define MACHINERY_COREXY      2
#define MACHINERY_LINEARDELTA 3

#define FLAVOR_REPLICATORG 1
#define FLAVOR_CURA        2
#define FLAVOR_SLIC3R      3

//机械结构,选择一个
// #define MACHINERY_TYPE MACHINERY_CARTESIAN
// #define MACHINERY_TYPE MACHINERY_COREXY
#define MACHINERY_TYPE MACHINERY_LINEARDELTA

//G代码生成工具,选择一个
// #define GCODE_FLAVOR FLAVOR_REPLICATORG
// #define GCODE_FLAVOR FLAVOR_CURA
#define GCODE_FLAVOR FLAVOR_SLIC3R

//三轴最大行程(um)
#define X_MAX_LIMIT 400000
#define Y_MAX_LIMIT 400000
#define Z_MAX_LIMIT 400000

#if MACHINERY_TYPE == MACHINERY_LINEARDELTA
//机械臂半径(um)
#define ROBOTIC_ARM_RADIUS 124000
//机械臂长度(um)
#define ROBOTIC_ARM_LENGTH 250000
#endif

//从G代码坐标到实际坐标的偏移(um)
#if GCODE_FLAVOR == FLAVOR_REPLICATORG
#define X_OFFSET (X_MAX_LIMIT/2-15000)
#define Y_OFFSET (Y_MAX_LIMIT/2)
#define Z_OFFSET 1720
#else
#define X_OFFSET 0
#define Y_OFFSET 0
#define Z_OFFSET 0
#endif

//X轴步进电机旋转一周对应的直线位移(um)
#define X_DISTANCE_PER_CYCLE 31415
//X轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define X_PULSES_PER_CYCLE   (16*200)
//X轴运行方向调整(取值+/-1)
#define X_DIRECTION_ADJ      1 

//Y轴步进电机旋转一周对应的直线位移(um)
#define Y_DISTANCE_PER_CYCLE 31415
//Y轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define Y_PULSES_PER_CYCLE (16*200)
//Y轴运行方向调整(取值+/-1)
#define Y_DIRECTION_ADJ      1 

//Z轴步进电机旋转一周对应的直线位移(um)
#define Z_DISTANCE_PER_CYCLE 31415
//Z轴步进电机旋转一周需要的脉冲数量(考虑细分)
#define Z_PULSES_PER_CYCLE (16*200)
//Z轴运行方向调整(取值+/-1)
#define Z_DIRECTION_ADJ      1

//挤出器步进电机旋转一周对应的原料丝运动长度(um)
#define A_DISTANCE_PER_CYCLE 34540
//挤出器步进电机旋转一周需要的脉冲数量(考虑细分)
#define A_PULSES_PER_CYCLE (16*200)
//挤出器运行方向调整(取值+/-1)
#define A_DIRECTION_ADJ      1

//限位开关最小触发间隔时间(ms)
#define LIMIT_SWITCH_MIN_TOGGLE_PERIOD 50
//限位开关接通有效的阈值(ms)
#define LIMIT_SWITCH_VALID_TIME 40

//键盘最小触发间隔时间(ms)
#define KEYBOARD_MIN_TOGGLE_PERIOD 50
//键盘开始自动重复时间(ms)
#define KEYBOARD_START_REPEAT_TIME 900
//键盘自动重复间隔(ms)
#define KEYBOARD_REPEAT_PERIOD 250

//加热器PWM输出频率(单位Hz)
#define HEATER_PWM_FREQ 90

//挤出器输出更新频率限制(ms)
#define EXTRUDER_UPDATE_PERIOD 500
//挤出器默认温度
#define EXTRUDER_DEFAULT_TEMP 200
//挤出器温度PID控制参数
#define EXTRUDER_PID_KP 800
#define EXTRUDER_PID_KI 25
#define EXTRUDER_PID_KD 700
#define EXTRUDER_PID_INIT_SUM 800

//挤出器使用热电阻测温而不是热电偶
#define EXTRUDER_THERMO_USING_ADC 0
//挤出器热电阻传感值与温度换算
#define EXTRUDER_ADC_TO_TEMP(x) (123.678-0.0893583*(x)+0.0000195876*(x)*(x))

//加热板输出更新频率限制(ms)
#define HEATBED_UPDATE_PERIOD 500
//加热板默认温度
#define HEATBED_DEFAULT_TEMP 96
//加热板传感器值与温度换算
#define HEATBED_ADC_TO_TEMP(x) (123.678-0.0893583*(x)+0.0000195876*(x)*(x))
//加热板PID参数
#define HEATBED_PID_KP 500
#define HEATBED_PID_KI 15
#define HEATBED_PID_KD 400
#define HEATBED_PID_INIT_SUM 100

//步进电机驱动使能端低有效
#define MOTOR_DRIVER_ACTIVE_LOW

//功率输出板极性反转,影响PWM,风扇等
// #define DRIVER_BOARD_POLARITY

//默认给进速度(um/min)
#define DEFAULT_FEEDRATE (500*1000)

//SD卡初始化重试次数限制
#define SD_INIT_RETRY_TIMES 3
//SD卡存放打印文件的目录
#define SD_GFILES_DIR "/"
//支持的最长文件名(包含\0)
#define SD_MAX_FILENAME_LEN (31+1)
//最多列举的G文件数量
#define SD_MAX_ITEMS 32

//每次向上位机报告的间隔(ms)
#define REPORT_PERIOD 1000