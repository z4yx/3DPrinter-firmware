#ifndef __GFILES__H__
#define __GFILES__H__

#include "common.h"

void FileManager_Init(void);
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN];
int FileManager_GetLine(char *buf, int buf_size);
bool FileManager_OpenGcode(const char *file);
void FileManager_Close(void);
bool FileManager_SDCardAvailable();
bool FileManager_SetInUSBMode(bool usbMode);

enum {
	FM_NO_CARD,      //未检测到存储卡
	FM_CARD_FREE,    //存储卡空闲
	FM_CARD_MOUNTED, //已挂载文件系统
	FM_CARD_BUSY     //存储卡被其他设备（USB）占用
};

#endif /* __GFILES__H__ */