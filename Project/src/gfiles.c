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
#include "sdio.h"
#include "ff.h"
#include <string.h>

static uint8_t card_status;
FATFS fileSystem;
static SD_CardInfo SDCardInfo;

char GCodeFiles[SD_MAX_ITEMS][SD_MAX_FILENAME_LEN];

//当前打开的文件对象
FIL * currentOpened;

//初始化文件系统及SD卡
void FileManager_Init(void)
{
	SD_Error Status = SD_OK;

	currentOpened = 0;
	card_status   = FM_NO_CARD;

	for(int i=0;i<SD_INIT_RETRY_TIMES;i++)
	{
		if((Status = SD_Init()) != SD_OK){
			DBG_MSG("SD Card Init Failed! Retrying...", 0);
		}
		else
		{
			break;
		}
	}
	if(Status != SD_OK) {
		ERR_MSG("No SD card found!", 0);
		return;
	}

	Status = SD_GetCardInfo( &SDCardInfo );
    if(Status != SD_OK)
        return;

    DBG_MSG("CardCapacity: %dK, %d Blocks, Block Size %d",
    	SDCardInfo.CardCapacity/1024,
    	SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize,
    	SDCardInfo.CardBlockSize);

	card_status = FM_CARD_FREE;
	DBG_MSG("SD Card Init OK!", 0);
}

const SD_CardInfo* FileManager_GetCardInfo(void)
{
	return &SDCardInfo;
}

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
	FRESULT res;
	FILINFO info;
	DIR rootDir;
	int cur_file;

	if(card_status != FM_CARD_FREE){
		ERR_MSG("Failed. card_status=%d", (int)card_status);
		return 0;
	}

	if(f_mount(0, &fileSystem) != FR_OK){
		ERR_MSG("Failed to mount SD card!", 0);
		return 0;
	}

	card_status = FM_CARD_MOUNTED;

	res = f_opendir(&rootDir, SD_GFILES_DIR);
	if(FR_OK != res){
		ERR_MSG("Failed to open root dir! result=%d", (int)res);
		f_mount(0, 0);
		card_status = FM_CARD_FREE;
		return 0;
	}

	for(cur_file = 0; cur_file < SD_MAX_ITEMS; ){
		info.lfname = GCodeFiles[cur_file];
		info.lfsize = SD_MAX_FILENAME_LEN;
		res = f_readdir(&rootDir, &info);
		if(FR_OK == res && info.fname[0] != '\0') {
			int length;
			if(info.fattrib & AM_DIR)
				continue;
			DBG_MSG("File: %s", info.fname);
			DBG_MSG("LFN: %s", info.lfname);
			length = strlen(info.lfname);
			if(length == 0) {
				//LFN not available, using DOS 8.3 instead
				length = strlen(info.fname);
				strcpy(GCodeFiles[cur_file], info.fname);
			}
			if(length > 2 && strcasecmp(GCodeFiles[cur_file]+length-2, ".g") == 0) {
				cur_file++;
			}
		}else {
			break;
		}
	}
	if(cur_file < SD_MAX_ITEMS)
		GCodeFiles[cur_file][0] = '\0';

	f_mount(0, 0);
	card_status = FM_CARD_FREE;

	return &GCodeFiles;
}

bool FileManager_OpenGcode(const char *file)
{
	static FIL fileObj; //must be static

	if(card_status != FM_CARD_FREE){
		ERR_MSG("Failed. card_status=%d", (int)card_status);
		return false;
	}

	if(f_mount(0, &fileSystem) != FR_OK){
		ERR_MSG("Failed to mount SD card!", 0);
		return false;
	}

	card_status = FM_CARD_MOUNTED;

	if(f_open(&fileObj, file, FA_OPEN_EXISTING|FA_READ) != FR_OK){
		ERR_MSG("Failed to open %s!", file);
		f_mount(0, 0);
		card_status = FM_CARD_FREE;
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