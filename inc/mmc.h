/*
 * mmc.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef MMC_H
#define	MMC_H

#include "types.h"
#include "hard.h"
#include "globals.h"

#define SD_V2 2
#define SD_HC 1
#define WAIT_LEN 2048

extern u8 csd_data[];
extern u8 ocr_data[];

u8 mmcCrc7(u8 *buff);
u8 mmcCmd(u8 cmd, u32 arg);
u8 mmcFastCmd(u8 cmd, u32 arg);
u8 mmcInit();
u8 mmcStop(u8 forced);
u8 mmcWrBlock(u32 mmc_addr, u8 *data_ptr);
u8 mmcRdBlock(u32 mmc_addr, u8 *stor);
u8 mmcReadOCR();
u8 mmcReadCSD();
u8 mmcGetDeviceSize(u32 *size);
u32 deg(u32 d);
u8 mmcInitRead(u32 mmc_addr);
void mmcFinishRead();
void mmcOff();

#endif	/* MMC_H */
