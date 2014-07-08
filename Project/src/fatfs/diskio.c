/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* by grqd_xp                                                            */
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "common.h"
#include "sdio.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */
#define ATA		1
#define MMC		0
#define USB		2
//#define SST25_SECTOR_SIZE 4096
//#define SST25_BLOCK_SIZE 512 

#define SECTOR_SIZE            512 /* Block Size in Bytes */
static u32 buff2[SECTOR_SIZE/4];

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0的操作
    }

    if (SD_Init() == SD_OK)
    {
        return RES_OK;
    }
    
    return STA_NOINIT;
    
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0操作
    }

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
    SD_Error res = SD_OK;
	
    if(count==1)
    {
        res = SD_ReadBlock(buff,sector << 9  ,SECTOR_SIZE);        
    }
    else
    {
        res =  SD_ReadMultiBlocks(buff,sector << 9 ,SECTOR_SIZE,count);        
    } 
    res = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK)
    {
    }
    // DBG_MSG("count=%d,sector=%d,res=%d", (int)count, sector, res);
    // DBG_MSG("%d,%d,%d,%d,%d,%d,%d,%d", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7]);
    
    if(res == SD_OK)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    } 
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
    SD_Error res = SD_OK;
	
    if(count==1)
    {          
        res =SD_WriteBlock((u8 *)buff,sector << 9 ,SECTOR_SIZE);
    }
    else
    {          
        res = SD_WriteMultiBlocks((u8 *)buff,sector <<9 ,SECTOR_SIZE,count);
    }
    res = SD_WaitWriteOperation();  
    while(SD_GetStatus() != SD_TRANSFER_OK);
        
    if(res == SD_OK)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    
    return RES_OK;

    //FATFS目前版本仅需处理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三个命令
    /*
    DRESULT res;
    switch(ctrl)
    {
    case CTRL_SYNC:
        SD_CS_ENABLE();
        if(SD_WaitReady()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        SD_CS_DISABLE();
        break;
        
    case GET_BLOCK_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD*)buff = SD_GetCapacity();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;*/
}


/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{
//    struct tm t;
//    DWORD date;
//    t = Time_GetCalendarTime();
//    t.tm_year -= 1980;		//年份改为1980年起
//    t.tm_mon++;         	//0-11月改为1-12月
//    t.tm_sec /= 2;      	//将秒数改为0-29
//    
//    date = 0;
//    date = (t.tm_year << 25)|(t.tm_mon<<21)|(t.tm_mday<<16)|\
//            (t.tm_hour<<11)|(t.tm_min<<5)|(t.tm_sec);
//
//    return date;
    return 0;
}

