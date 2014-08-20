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

#define G0_RAPID_MOVE      0
#define G1_CONTROLLED_MOVE 1
#define G28_MOVE_TO_ORIGIN 28
#define G90_ABSOLUTE_COORD 90
#define G91_RELATIVE_COORD 91
#define G92_SET_POSITION   92
#define G161_HOME_MINIMUM  161
#define G162_HOME_MAXIMUM  162

#define M6_WAIT_FOR_TOOL   6
#define M18_DISABLE_MOTORS 18
#define M73_SET_PROGRESS   73
#define M84_STOP_IDLE_HOLD 84
