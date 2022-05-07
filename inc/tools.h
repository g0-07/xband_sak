/*
 * tools.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef TOOLS_H
#define	TOOLS_H

#include "types.h"
#include "ff.h"

extern u8 str_buff[];

void drawHexNum(u8 *comment, u32 num, u8 min, u16 base, u8 x, u8 y);
void drawNum(u8 *comment, u32 num, u16 base, u8 x, u8 y);
void drawDir(u8 x, u8 y, u16 selector, u8 items, FILINFO *inf);
void viewMem(u32 addr);
void drawMem(u32 mem, u16 len);
void drawProgress(u32 value, u32 total, u8 x, u8 y);

#endif	/* TOOLS_H */
