/*
 * hard.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef HARD_H
#define	HARD_H

#define _SS 0
#define _FULL_SPEED 1
#define _SPI16 2
#define _GAME_MODE 3
#define _SMS_MODE 4
#define _HARD_RESET 5
#define _RAM_MODE_1 6
#define _RAM_ON 7
#define _VBL_CATCH 8
#define _MEGAKEY_ON 9
#define _MEGAKEY_REGION_1 10
#define _SSF_MODE_ON 11
#define _RAM_FS 12
#define _CART 13

#define REGION_NTSC 0
#define REGION_PAL 1
#define REGION_MULTI 2

#define CONSOLE_VMOD (*((volatile u16*) (0xa10000)) >> 6 & 1)
#define CONSOLE_EXPORT (*((volatile u16*) (0xa10000)) >> 7 & 1)

#define _SPI_READY 0
#define _RY 1
#define _SMS_KEY 2
#define _SD_CART 3

#define SPI_PORT *((volatile u16*) (0xa13000))
#define CFG_PORT *((volatile u16*) (0xa13002))
#define VBL_PORT *((volatile u16*) (0xa13004))
#define SRAM_BANK_PORT *((volatile u16*) (0xa13006))
#define VER_PORT *((volatile u16*) (0xa13008))
#define ROM_MAP_PORT *((volatile u16*) (0xa1300a))

#define CFGC(bit)(cfg &= ~(1 << bit), CFG_PORT = cfg)
#define CFGS(bit)(cfg |= (1 << bit), CFG_PORT = cfg)

#define SPI_HI_SPEED_ON CFGS(_FULL_SPEED)
#define SPI_HI_SPEED_OFF CFGC(_FULL_SPEED)

#define SPI16_ON CFGS(_SPI16);
#define SPI16_OFF CFGC(_SPI16);

#define SS_ON CFGC(_SS)
#define SS_OFF CFGS(_SS)

#define CART_ON CFGC(_CART)
#define CART_OFF CFGS(_CART)

#define IS_RY (CFG_PORT & (1 << _RY))
#define IS_SPI_READY (CFG_PORT & (1 << _SPI_READY))
#define IS_SMS_KEY_PRESSED (CFG_PORT & (1 << _SMS_KEY))
#define IS_SD_SLOT_EMPTY (CFG_PORT & (1 << _SD_CART))

#define SPI_BUSY while(!IS_SPI_READY)
#define EPR_BUSY while(!IS_RY)

#define RAM_ON CFGS(_RAM_ON);
#define RAM_OFF CFGC(_RAM_ON);

#define VBL_CATCH_ON CFGS(_VBL_CATCH);
#define VBL_CATCH_OFF CFGC(_VBL_CATCH);

#define SPI16_ON CFGS(_SPI16);
#define SPI16_OFF CFGC(_SPI16);

u16 cfg;

#endif	/* HARD_H */
