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

//当前打开的文件对象
FIL * currentOpened;

//初始化文件系统及SD卡
void FileManager_Init(void)
{
	int res;

	currentOpened = 0;

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
}

//列举SD卡中的G代码文件
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN] 
{
	FRESULT res;
	FILINFO info;
	DIR rootDir;
	int cur_file;

	if(no_sd_card)
		return 0;

	if(f_mount(0, &fileSystem) != FR_OK){
		ERR_MSG("Failed to mount SD card!", 0);
		return 0;
	}

	res = f_opendir(&rootDir, SD_GFILES_DIR);
	if(FR_OK != res){
		ERR_MSG("Failed to open root dir! result=%d", (int)res);
		f_mount(0, 0);
		return 0;
	}

	for(cur_file = 0; cur_file < SD_MAX_ITEMS; ){
		res = f_readdir(&rootDir, &info);
		if(FR_OK == res && info.fname[0] != '\0') {
			int length;
			if(info.fattrib & AM_DIR)
				continue;
			length = strlen(info.fname);
			if(length > 2 && strcasecmp(info.fname+length-2, ".g") == 0) {
				strcpy(GCodeFiles[cur_file], info.fname);
				cur_file++;
			}
		}else {
			break;
		}
	}
	if(cur_file < SD_MAX_ITEMS)
		GCodeFiles[cur_file][0] = '\0';

	f_mount(0, 0);

	return &GCodeFiles;
}

bool FileManager_OpenGcode(const char *file)
{
	static FIL fileObj; //must be static

	if(no_sd_card)
		return false;

	if(f_mount(0, &fileSystem) != FR_OK){
		ERR_MSG("Failed to mount SD card!", 0);
		return false;
	}

	if(f_open(&fileObj, file, FA_OPEN_EXISTING|FA_READ) != FR_OK){
		ERR_MSG("Failed to open %s!", file);
		f_mount(0, 0);
		return false;
	}

	currentOpened = &fileObj; //fileObj is static

	return true;
}

void FileManager_Close(void)
{
	if(!currentOpened)
		return;

	f_close(currentOpened);
	currentOpened = 0;

	f_mount(0, 0);
}

static int readOneByte(void)
{
	uint8_t ch;
	uint32_t cnt;
	if(!currentOpened)
		return EOF;
	return (FR_OK == f_read(currentOpened, &ch, 1, &cnt)
		&& cnt == 1) ? ch : EOF;
}

//从当前打开文件中读取一行,包含换行符
int FileManager_GetLine(char *buf, int buf_size)
{
	int count = 0, ch;
	while(count<buf_size-1 && (ch = readOneByte())!=EOF){
		*(buf++) = ch;
		count++;
		if(ch == '\n')
			break;
	}
	if(count == buf_size-1)
		*buf = '\n';
	while(ch != '\n' && ch != EOF)
		ch = readOneByte();
	return count;
}