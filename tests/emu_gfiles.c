#include "common.h"
#include "systick.h"
#include "gfiles.h"
#include "sdio.h"
#include <string.h>
#include <dirent.h>

static uint8_t card_status;

char GCodeFiles[SD_MAX_ITEMS][SD_MAX_FILENAME_LEN];

//当前打开的文件对象
FILE * currentOpened;

//初始化文件系统及SD卡
void FileManager_Init(void)
{
	SD_Error Status = SD_OK;

	currentOpened = 0;
	card_status   = FM_NO_CARD;

	Status = SD_OK;
	if(Status != SD_OK) {
		ERR_MSG("No SD card found!", 0);
		return;
	}

	// Status = SD_GetCardInfo( &SDCardInfo );
 //    if(Status != SD_OK)
 //        return;

    // DBG_MSG("CardCapacity: %dK, %d Blocks, Block Size %d",
    // 	SDCardInfo.CardCapacity/1024,
    // 	SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize,
    // 	SDCardInfo.CardBlockSize);

	card_status = FM_CARD_FREE;
	DBG_MSG("SD Card Init OK!", 0);
}

// const SD_CardInfo* FileManager_GetCardInfo(void)
// {
// 	return &SDCardInfo;
// }

//SD可用
bool FileManager_SDCardAvailable()
{
	return card_status == FM_CARD_FREE;
}

bool FileManager_SetInUSBMode(bool usbMode)
{
	if(card_status == FM_CARD_FREE && usbMode) {
		card_status = FM_CARD_BUSY;
		return true;
	}
	if(card_status == FM_CARD_BUSY && !usbMode) {
		card_status = FM_CARD_FREE;
		return true;
	}
	ERR_MSG("Failed. card_status=%d but usbMode=%d", (int)card_status, (int)usbMode);
	return false;
}

//列举SD卡中的G代码文件
char (*FileManager_ListGFiles(void))[][SD_MAX_FILENAME_LEN] 
{
	SD_Error res;
	DIR *rootDir;
	struct dirent *ep;
	int cur_file;

	if(card_status != FM_CARD_FREE){
		ERR_MSG("Failed. card_status=%d", (int)card_status);
		return 0;
	}

	// if(f_mount(0, &fileSystem) != FR_OK){
	// 	ERR_MSG("Failed to mount SD card!", 0);
	// 	return 0;
	// }

	card_status = FM_CARD_MOUNTED;

	rootDir = opendir(SD_GFILES_DIR);
	if(!rootDir){
		ERR_MSG("Failed to open root dir! result=%d", (int)res);
		// f_mount(0, 0);
		card_status = FM_CARD_FREE;
		return 0;
	}

	for(cur_file = 0; cur_file < SD_MAX_ITEMS; ){
		ep = readdir(rootDir);
		if(ep && ep->d_name[0] != '\0') {
			int length;
			if(ep->d_type & DT_DIR)
				continue;
			DBG_MSG("File: %s", ep->d_name);
			length = strlen(ep->d_name);
			if(length > 2 && strcasecmp(ep->d_name+length-2, ".g") == 0) {
				strcpy(GCodeFiles[cur_file], ep->d_name);
				cur_file++;
			}
		}else {
			break;
		}
	}
	if(cur_file < SD_MAX_ITEMS)
		GCodeFiles[cur_file][0] = '\0';

	// f_mount(0, 0);
	card_status = FM_CARD_FREE;

	return &GCodeFiles;
}

bool FileManager_OpenGcode(const char *file)
{
	if(card_status != FM_CARD_FREE){
		ERR_MSG("Failed. card_status=%d", (int)card_status);
		return false;
	}

	// if(f_mount(0, &fileSystem) != FR_OK){
	// 	ERR_MSG("Failed to mount SD card!", 0);
	// 	return false;
	// }

	card_status = FM_CARD_MOUNTED;

	if(!(currentOpened = fopen(file, "r") )){
		ERR_MSG("Failed to open %s!", file);
		// f_mount(0, 0);
		card_status = FM_CARD_FREE;
		return false;
	}

	return true;
}

void FileManager_Close(void)
{
	if(!currentOpened)
		return;

	fclose(currentOpened);
	currentOpened = 0;

	// f_mount(0, 0);
	card_status = FM_CARD_FREE;
}

/*
注意：每一次调用f_read读取内容不能超过一个sector(512bytes)，否则fatfs会使用direct transfer方式，
直接使用传入的缓冲区，如果传入的缓冲区非4字节对齐，SDIO中的DMA就会出错。
f_read的官方说明：
The memory address specified by buff is not that always aligned to word boundary 
because the type of argument is defined as BYTE*. The misaligned read/write request 
can occure at direct transfer. If the bus architecture, especially DMA controller, 
does not allow misaligned memory access, it should be solved in this function. 
There are some workarounds described below to avoid this issue.

** Convert word transfer to byte transfer in this function. - Recommended.
** For f_read(), avoid long read request that includes a whole of sector. - Direct transfer will never occure.
** For f_read(fp, buff, btr, &br), make sure that (((UINT)buff & 3) == (f_tell(fp) & 3)) is true. - Word aligned direct transfer is guaranteed.

Generally, a multiple sector transfer request must not be split into single sector transactions to the storage device, or you will not get good read throughput.
*/

static int readOneByte(void)
{
	uint8_t ch;
	if(!currentOpened)
		return EOF;
	return (1 == fread(&ch, 1, 1, currentOpened)) ? ch : EOF;
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