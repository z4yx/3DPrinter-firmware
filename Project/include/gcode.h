/*
 * =====================================================================================
 *
 *       Filename:  gcode.h
 *
 *    Description:  程序支持的G代码
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

#ifndef GCODE_FLAVOR
#error GCODE_FLAVOR should be defined before this file
#endif

#define G0_RAPID_MOVE      0
#define G1_CONTROLLED_MOVE 1
#define G28_MOVE_TO_ORIGIN 28
#define G90_ABSOLUTE_COORD 90
#define G91_RELATIVE_COORD 91
#define G92_SET_POSITION   92
#if GCODE_FLAVOR == FLAVOR_REPLICATORG
#define G161_HOME_MINIMUM  161
#define G162_HOME_MAXIMUM  162
#endif

#if GCODE_FLAVOR == FLAVOR_REPLICATORG
#define M6_WAIT_FOR_TOOL   6
#define M73_SET_PROGRESS   73
#endif
#define M18_DISABLE_MOTORS 18
#define M84_STOP_IDLE_HOLD 84
#if GCODE_FLAVOR == FLAVOR_REPLICATORG
#define M104_EXTRUDER_SET  104
#define M109_HEATBED_SET   109
#else
#define M104_EXTRUDER_SET  104
#define M109_EXTRUDER_WAIT 109
#define M140_HEATBED_SET   140
#define M190_HEATBED_WAIT  190
#endif
