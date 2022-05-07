/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "mmc.h"
#include "globals.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

DSTATUS dstat;
BYTE dresp;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    dresp = mmcInit();
    dstat = 0;
    if (dresp)dstat = STA_NOINIT;

    return dstat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u16 j, bsize;
	u32 addr;
	
	bsize = 1;
	if ((card_type & SD_HC) == 0)bsize = 512;

	for (j = 0; j < count; j += 1) {
		addr = (DWORD)sector * bsize + j * 512;
		if (mmcRdBlock(addr, buff + j * 512) != 0)return RES_ERROR;
	}

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u16 j, bsize;
	u32 addr;
	
	bsize = 1;
	if ((card_type & SD_HC) == 0)bsize = 512;

	for (j = 0; j < count; j += 1) {
		addr = (DWORD)sector * bsize + j * 512;
		if (mmcWrBlock(addr, (BYTE *)buff + j * 512) != 0)return RES_ERROR;
	}

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;

    switch (cmd) {
        case CTRL_SYNC:
			res = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*) buff = 0;
            res = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(DWORD*) buff = 512;
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD*) buff = 512;
            res = RES_OK;
            break;
    }

    return res;
}
