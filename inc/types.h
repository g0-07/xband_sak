/*
 * types.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef TYPES_H
#define	TYPES_H

#define s8      char
#define s16     short
#define s32     long

#define u8      unsigned char
#define u16     unsigned short
#define u32     unsigned long
#define u64     unsigned long

#if !defined(uint8_t) && !defined(__int8_t_defined)
#define uint8_t     u8
#define int8_t      s8
#endif
#if !defined(uint16_t) && !defined(__int16_t_defined)
#define uint16_t    u16
#define int16_t     s16
#endif
#if !defined(uint32_t) && !defined(__int32_t_defined)
#define uint32_t    u32
#define int32_t     s32
#endif

#define NULL    0
#define USED    1

#endif	/* TYPES_H */
