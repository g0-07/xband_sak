/*
 * sram_manager.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef SRAM_MANAGER_H
#define	SRAM_MANAGER_H

#define SRAM_BASE          0x0
#define SRAM_SIZE          65536

void sramManager();
void restoreSram();

#endif	/* SRAM_MANAGER_H */
