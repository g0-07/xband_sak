/*
 * globals.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "globals.h"

u8 cy;
volatile u16 joy;
FIL file;
DIR dir;
FATFS FatFs;
u8 dirsize;
u8 card_type;
u32 timer;
u8 progress_step;
