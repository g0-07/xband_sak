/*
 * select_file.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef SELECT_FILE_H
#define	SELECT_FILE_H

#include "types.h"
#include "ff.h"

#define MENU_ITEMS 24
#define MAX_DEPTH 8

u8 selectFile(u8 select_dir, char *selected_path);
u8 readDir(u8 select_dir, u16 start_index, FILINFO *inf);

#endif	/* SELECT_FILE_H */
