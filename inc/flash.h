/*
 * flash.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef FLASH_H
#define	FLASH_H

#include "types.h"

#define MAX_TIMEOUT         0x07ffffff

#define FLASH_BASE          0x200000
#define FLASH_MFR_ID        0x0001
#define FLASH_AM29F400BB    0x22ab
#define FLASH_AM29F400BT    0x2223
#define FLASH_SIZE          524288

#define FLASH_ADDRESS(offset) ((volatile u16*)(FLASH_BASE + offset))

u8 checkDataPolling(u32 dst, u16 truedata);
u8 checkToggleReady(u32 dst);
u8 flashWriteWord(u16 *srcword, u32 dst);
u8 flashErase();
u8 flashExist();

#endif	/* FLASH_H */
