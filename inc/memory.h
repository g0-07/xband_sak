/*
 * memory.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef MEMORY_H
#define	MEMORY_H

#include "types.h"

#define WMEM8(addr, data)*((volatile u8*) (addr)) = data
#define WMEM16(addr, data)*((volatile u16*) (addr)) = data
#define WMEM32(addr, data)*((volatile u32*) (addr)) = data

#define RMEM8(addr, data)data = *((volatile u8*) (addr))
#define RMEM16(addr, data)data = *((volatile u16*) (addr))
#define RMEM32(addr, data)data = *((volatile u32*) (addr))

#define malloc(x)       MEM_alloc(x)
#define free(x)         MEM_free(x)

void MEM_free(void *ptr);
void *MEM_alloc(u16 size);
void memset(void *to, u8 value, u16 len);
void memsetU16(u16 *to, u16 value, u16 len);
void memsetU32(u32 *to, u32 value, u16 len);
void memcpy(void *to, const void *from, u16 len);
void memcpyU16(u16 *to, const u16 *from, u16 len);
void memcpyU32(u32 *to, const u32 *from, u16 len);
int8_t memcmp(const char *m1, const char *m2, int16_t len);

#endif	/* MEMORY_H */
