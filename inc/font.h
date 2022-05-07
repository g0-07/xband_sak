/*
 * font.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef FONT_H
#define	FONT_H

#include "types.h"

asm(".align 4");
asm(".globl font_base");
asm("font_base:");
asm(".incbin \"res/font_base.bin\"");

extern const u32 font_base[];

#endif	/* FONT_H */
