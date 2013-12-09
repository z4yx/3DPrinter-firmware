#ifndef __GFILES__H__
#define __GFILES__H__

#include "common.h"

void FileManager_Init(void);
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN];
int FileManager_GetLine(char *buf, int buf_size);
bool FileManager_OpenGcode(const char *file);
void FileManager_Close(void);


#endif /* __GFILES__H__ */