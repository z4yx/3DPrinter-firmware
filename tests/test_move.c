#include "common.h"
#include "systick.h"
#include "command.h"
#include "move.h"
#include "heatbed.h"
#include "extruder.h"

const Task_t SystemTasks[] = { LimitSwitch_Task, ExtruderTask, HeatBedTask, Command_Task};

int main(int argc, char const *argv[])
{
	EMUMACH_Init();
	SysTick_Init();
	FileManager_Init();
	Move_Init();
	Extruder_Init();
	HeatBed_Init();
	Command_Init();

	DBG_MSG("Peripheral init done.", 0);

	Command_StartPrinting("/Users/zhang/Downloads/gcode/slic3r_box.gcode");

	while (1)
	{

		//运行系统中声明的任务
		for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
			(SystemTasks[i])();
	}

	return 0;
}