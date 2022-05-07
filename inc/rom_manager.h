/*
 * rom_manager.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef ROM_MANAGER_H
#define	ROM_MANAGER_H

#include "types.h"

void romManager();
void backupRom(u8 *mem_ptr, u32 length, char extension[3]);
void flashRom();

#endif	/* ROM_MANAGER_H */
