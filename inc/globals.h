/*
 * globals.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#include "types.h"
#include "ff.h"

#define DEBUG
#define APP_VERSION 1

extern u8 cy;
extern volatile u16 joy;
extern FIL file;
extern DIR dir;
extern FATFS FatFs;
extern u8 dirsize;
extern u8 card_type;
extern u32 timer;
extern u8 progress_step;

#endif	/* GLOBALS_H */
