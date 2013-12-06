#ifndef __GFILES__H__
#define __GFILES__H__

#include "common.h"

void FileManager_Init(void);
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN];


#endif /* __GFILES__H__ */