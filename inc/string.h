/*
 * string.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef STRING_H
#define	STRING_H

#include "types.h"

u8 STR_intToDecString(u32 val, u8 *str);
void STR_intToDecStringMin(u32 val, u8 *str, u8 min_size);
u8 STR_intToHexString(u32 val, u8 *str);
void STR_intToHexStringMin(u32 val, u8 *str, u8 min_size);
char *strchr(const char *s, int c);
u16 strlen(const char *str);
char *strcpy(char *to, const char *from);

#endif	/* STRING_H */
