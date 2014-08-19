/*
 * =====================================================================================
 *
 *       Filename:  command.c
 *
 *    Description:  G代码解析与执行
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
#include "common.h"
#include "command.h"
#include "move.h"
#include "motor.h"
#include "heatbed.h"
#include "extruder.h"
#include "gfiles.h"
#include "gcode.h"
#include "usb.h"

//最大的代码行长度,超过部分将被丢弃
#define MAX_LINE_LENGTH 63
//G代码中使用mm,而程序中使用um
#define UNIT_CONV(x) (1000*(x))

enum {AbsolutePositioning, RelativePositioning};

//G代码行缓冲
static char linebuf[MAX_LINE_LENGTH+1];
//G代码中的几个参数
static int X, Y, Z, F, E;
//是否正在打印，或连接着USB
static uint8_t currentMode;
//机器当前状态（在打印中时）
static uint16_t currentState;
//完成百分比
static uint8_t Progress;
//当前坐标模式(绝对/相对)
static uint8_t PositioningMode;

static int codeLine;

static void Command_doNext(void);

static void resetGcodeParams(void)
{
	X = Y = Z = E = F = 0;
	F = DEFAULT_FEEDRATE;
	PositioningMode = AbsolutePositioning;
}

void Command_Init(void)
{
	currentMode = MACH_MODE_STANDBY;
	currentState = MACH_STATE_READY;
	Progress = 0;
}

bool Command_IsStandBy(void)
{
	return currentMode == MACH_MODE_STANDBY;
}

void Command_GetState(uint8_t *mode, uint16_t *state, uint8_t *progress)
{
	*mode = currentMode;
	*state = currentState;
	*progress = Progress;
}

bool Command_StartPrinting(const char * file)
{
	if(currentMode == MACH_MODE_USB){
		ERR_MSG("In USB Mode", 0);
		return false;
	}
	if(currentMode == MACH_MODE_PRINT){
		ERR_MSG("Printing now", 0);
		return false;
	}
	
	if(!FileManager_OpenGcode(file)){
		ERR_MSG("Failed to open G code file!", 0);
		return false;
	}
	DBG_MSG("Start printing %s", file);
	currentMode = MACH_MODE_PRINT;
	resetGcodeParams();
	codeLine = 0;

	return true;
}

bool Command_StopPrinting()
{
	if(currentMode != MACH_MODE_PRINT){
		ERR_MSG("NOT printing now", 0);
		return false;
	}

	Motor_PowerOff();
	Extruder_Stop_Heating();
	HeatBed_Stop_Heating();
	currentState = MACH_STATE_ENDED;

	return true;
}

void Command_Task(void)
{
	if(currentMode != MACH_MODE_PRINT){
		if(currentMode==MACH_MODE_USB && !USBDevice_PlugIn()){
			DBG_MSG("Disconnect USB host", 0);
			USBDevice_Disconnect();
			FileManager_SetInUSBMode(false);
			currentMode = MACH_MODE_STANDBY;
		}else if(currentMode!=MACH_MODE_USB && USBDevice_PlugIn() && FileManager_SDCardAvailable()){
			DBG_MSG("Try to connect USB host...", 0);
			FileManager_SetInUSBMode(true);
			USBDevice_Connect();
			currentMode = MACH_MODE_USB;
		}
		return;
	}

	switch(currentState) {
		case MACH_STATE_READY:
			Command_doNext();
			break;
		case MACH_STATE_HOMING:
			if(Move_XYZ_Ready()){
				//三轴回原点后，设置挤出器坐标为0
				Move_ResetAxisA();
				DBG_MSG("Operation \"Homing\" Done!", 0);
				currentState = MACH_STATE_READY;
				Motor_PowerOff();
			}
			break;
		case MACH_STATE_WAIT_HEAT:
			// DBG_MSG("MACH_STATE_WAIT_HEAT", 0);
			if(Extruder_TempReached() && HeatBed_TempReached()) {
				DBG_MSG("Temperature Reached!", 0);
				currentState = MACH_STATE_READY;
			}
			break;
		case MACH_STATE_DRAWING:
			if(Move_XYZ_Ready()){
				// DBG_MSG("Operation \"Drawing\" Done!", 0);
				currentState = MACH_STATE_READY;
			}
			break;
		case MACH_STATE_ENDED:
			DBG_MSG("Printing Finished!", 0);
			currentMode = MACH_MODE_STANDBY;
			currentState = MACH_STATE_READY;
			Progress = 0;
			FileManager_Close();
			break;
	}
}

static void doDrawingCmd()
{	
	int xyza[4];
	xyza[0] = X + X_OFFSET;
	xyza[1] = Y + Y_OFFSET;
	xyza[2] = Z + Z_OFFSET;
	xyza[3] = E;

	Move_AbsoluteMove(xyza, F);
}
static void setCurrentPos()
{	
	int xyza[4];
	xyza[0] = X + X_OFFSET;
	xyza[1] = Y + Y_OFFSET;
	xyza[2] = Z + Z_OFFSET;
	xyza[3] = E;

	Move_SetCurrentPos(xyza);
} 

//用于解析指令的三个辅助函数
static int getnum(char **p);
static float getfloat(char **p);
static char getletter(char **p);
static bool getparam(char **p, char *sym, float *value);

//解析并执行下一条指令
void Command_doNext()
{
	char *p;
	char sym;
	float value;
	int cmd;

	if(!FileManager_GetLine(linebuf, MAX_LINE_LENGTH)){
		currentState = MACH_STATE_ENDED;
		return;
	}
	codeLine++;
	p = linebuf;
	if(*p == 'G'){
		p++;
		cmd = getnum(&p);
		switch(cmd){
			case G0_RAPID_MOVE:
			case G1_CONTROLLED_MOVE:
				while(getparam(&p, &sym, &value)){
					int *t = NULL;
					if(sym == 'X')
						t = &X;
					else if(sym == 'Y')
						t = &Y;
					else if(sym == 'Z')
						t = &Z;
					else if(sym == 'F')
						F = UNIT_CONV(value);
					else if(sym == 'E')
						t = &E;
					if(t){
						*t = (PositioningMode == AbsolutePositioning ?
								UNIT_CONV(value) : *t + UNIT_CONV(value));
					}
				}
				if(cmd == G0_RAPID_MOVE){
					F = DEFAULT_FEEDRATE;
					DBG_MSG("G0_RAPID_MOVE %d,%d,%d", X, Y, Z);
				}else{
					DBG_MSG("G1_CONTROLLED_MOVE %d,%d,%d,%d,%d", X, Y, Z, F, E);
				}
				// REPORT(INFO_G_G1,"%d,%d,%d,%d,%d", X, Y, Z, F, E);
				Motor_PowerOn();
				doDrawingCmd();
				currentState = MACH_STATE_DRAWING;
				break;
			case G28_MOVE_TO_ORIGIN:
				Motor_PowerOn();
				sym=getletter(&p);
				if(sym < 'X' || sym > 'Z'){
					//if no axis is specified, then home all
					Move_Home(X_Axis);
					Move_Home(Y_Axis);
					Move_Home(Z_Axis);
					DBG_MSG("G28_MOVE_TO_ORIGIN all axes", 0);
				}
				for(; 'X'<=sym && sym<='Z'; sym=getletter(&p)){
					switch(sym) {
						case 'X':
							Move_Home(X_Axis);
							break;
						case 'Y':
							Move_Home(Y_Axis);
							break;
						case 'Z':
							Move_Home(Z_Axis);
							break;
					}
					char out[] = {sym, '\0'};
					DBG_MSG("G28_MOVE_TO_ORIGIN %s", out);
					REPORT(INFO_G_G28, "%s", out);
				}
				currentState = MACH_STATE_HOMING;
				break;
			case G90_ABSOLUTE_COORD:
				DBG_MSG("G90_ABSOLUTE_COORD", 0);
				PositioningMode = AbsolutePositioning;
				break;
			case G91_RELATIVE_COORD:
				DBG_MSG("G91_RELATIVE_COORD", 0);
				PositioningMode = RelativePositioning;
				break;
			case G92_SET_POSITION:
				while(getparam(&p, &sym, &value)){
					if(sym == 'X')
						X = UNIT_CONV(value);
					else if(sym == 'Y')
						Y = UNIT_CONV(value);
					else if(sym == 'Z')
						Z = UNIT_CONV(value);
					else if(sym == 'E')
						E = UNIT_CONV(value);
				}
				DBG_MSG("G92_SET_POSITION %d,%d,%d,%d", X, Y, Z, E);
				REPORT(INFO_G_G92,"%d,%d,%d,%d", X, Y, Z, E);
				setCurrentPos();
				break;
			case G162_HOME_MAXIMUM:
			case G161_HOME_MINIMUM:
				Motor_PowerOn();
				while(true){
					sym=getletter(&p);
					if(sym == 'X'){
						Move_Home(X_Axis);
						DBG_MSG("G161/162 X", 0);
						REPORT(INFO_G_G161,"X", 0);
					}
					else if(sym == 'Y'){
						Move_Home(Y_Axis);
						DBG_MSG("G161/162 Y", 0);
						REPORT(INFO_G_G161,"Y", 0);
					}
					else if(sym == 'Z'){
						Move_Home(Z_Axis);
						DBG_MSG("G161/162 Z", 0);
						REPORT(INFO_G_G161,"Z", 0);
					}
					else
						break;
				}
				currentState = MACH_STATE_HOMING;
				break;
			default:
				break;
		}
	}else if(*p == 'M'){
		int T = 0, P = 0;
		p++;
		cmd = getnum(&p);
		switch(cmd){
			case M6_WAIT_FOR_TOOL:
				if(getparam(&p, &sym, &value) && sym == 'T')
					T = value;
				DBG_MSG("M6_WAIT_FOR_TOOL %d", T);
				REPORT(INFO_G_M6,"%d", T);

				Extruder_Start_Heating(EXTRUDER_DEFAULT_TEMP);
				HeatBed_Start_Heating(HEATBED_DEFAULT_TEMP);
				Motor_PowerOff();
				currentState = MACH_STATE_WAIT_HEAT;
				break;
			case M18_DISABLE_MOTORS:
				DBG_MSG("M18_DISABLE_MOTORS", 0);
				REPORT(INFO_G_M18,"", 0);

				Motor_PowerOff();
				break;
			case M84_STOP_IDLE_HOLD:
				DBG_MSG("M84_STOP_IDLE_HOLD", 0);
				Motor_PowerOff();
				break;
			case M73_SET_PROGRESS:
				if(getparam(&p, &sym, &value) && sym == 'P')
					P = value;
				Progress = P;
				DBG_MSG("M73_SET_PROGRESS %d", P);
				REPORT(INFO_G_M73,"%d", P);
				break;
			default:
				break;
		}
	}else{
		//may be comments or empty line, do nothing

	}
}


static int getnum(char **p)
{
	int ret = 0;
	if(**p < '0' || **p > '9')
		return -1;
	while(**p >= '0' && **p <= '9'){
		ret = ret*10 + (**p) - '0';
		(*p)++;
	}
	return ret;
}

static float getfloat(char **p)
{
	float ret = 0;
	if((**p < '0' || **p > '9') && **p != '.')
		return -1;
	while(**p >= '0' && **p <= '9'){
		ret = ret*10 + (**p) - '0';
		(*p)++;
	}
	if(**p == '.') {
		float t = 10;
		(*p)++;
		while(**p >= '0' && **p <= '9'){
			ret += ((**p) - '0')/t;
			t *= 10;
			(*p)++;
		}
	}
	return ret;
}

static char getletter(char **p)
{
	char ret;
	while((**p) == ' ')
		(*p)++;
	if(**p < 'A' || **p > 'Z')
		return -1;
	ret = **p;
	(*p)++;
	return ret;
}

static bool getparam(char **p, char *sym, float *value)
{
	float tmp;
	bool neg = false;
	char letter = getletter(p);
	if(letter == -1)
		return false;
	*sym = letter;
	if(**p == '-'){
		neg = true;
		(*p)++;
	}
	tmp = getfloat(p);
	if(tmp < 0)
		return false;
	*value = neg ? -tmp : tmp;

	return true;
}
