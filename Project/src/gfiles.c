/*
 * =====================================================================================
 *
 *       Filename:  gfiles.c
 *
 *    Description:  G代码文件管理
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
#include "systick.h"
#include "gfiles.h"
#include "ff.h"
#include <string.h>

static bool no_sd_card = true;
FATFS fileSystem;

char GCodeFiles[SD_MAX_ITEMS][SD_MAX_FILENAME_LEN];

//初始化文件系统及SD卡
void FileManager_Init(void)
{
	int res;
	FRESULT fresult;
	SD_SPI_Configuration();

	for(int i=0;i<SD_INIT_RETRY_TIMES;i++)
	{
		if((res = SD_Init()) != 0){
			DBG_MSG("SD Card Init Failed! Retrying...", 0);
		}
		else
		{
			DBG_MSG("SD Card Init OK!", 0);
			break;
		}
	}
	if(res != 0) {
		ERR_MSG("No SD card found!", 0);
		no_sd_card = true;
		return;
	}

	no_sd_card = false;

	fresult = f_mount(0, &fileSystem);
	if(FR_OK != fresult)
		ERR_MSG("Failed to mount SD card!", 0);
}

//列举SD卡中的G代码文件
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN] 
{
	FRESULT res;
	FILINFO info;
	DIR rootDir;
	int cur_file;

	res = f_opendir(&rootDir, SD_GFILES_DIR);
	if(FR_OK != res){
		ERR_MSG("Failed to open root dir! result=%d", (int)res);
		return 0;
	}

	for(cur_file = 0; cur_file < SD_MAX_ITEMS; ){
		res = f_readdir(&rootDir, &info);
		if(FR_OK == res && info.fname[0] != '\0') {
			int length;
			if(info.fattrib & AM_DIR)
				continue;
			length = strlen(info.fname);
			if(length > 4 && strcasecmp(info.fname+length-4, ".s3g") == 0) {
				strcpy(GCodeFiles[cur_file], info.fname);
				cur_file++;
			}
		}else {
			break;
		}
	}
	if(cur_file < SD_MAX_ITEMS)
		GCodeFiles[cur_file][0] = '\0';

	return &GCodeFiles;
}