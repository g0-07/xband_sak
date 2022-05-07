/*
 * mem_mapper.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef MEM_MAPPER_H
#define	MEM_MAPPER_H

#define XBAND_ROMHI             0x04
#define XBAND_LED_ENABLE        0xff
#define XBAND_LED_4             0x08
#define XBAND_LODR_LOCATION     0x200204

#define XBAND_CTLPORT *((volatile u8*) (0x3bfe03))
#define XBAND_LEDPORT *((volatile u8*) (0x3bc169))
#define XBAND_LEDENPORT *((volatile u8*) (0x3bc16b))

#define RAM_HB_LOCATION         0x00ff0010
#define RAM_VB_LOCATION         0x00ff0020
#define RAM_RTE_LOCATION        0x00ff0030

#define HBLANK_HANDLER *((volatile u32*) (0x000070))
#define VBLANK_HANDLER *((volatile u32*) (0x000078))

void memMapper();
void mapXband();

#endif	/* MEM_MAPPER_H */
