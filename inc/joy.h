/*
 * joy.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef JOY_H
#define	JOY_H

#include "types.h"

#define JOY_1           0x0000
#define JOY_2           0x0001

#define BUTTON_UP       0x0001
#define BUTTON_DOWN     0x0002
#define BUTTON_LEFT     0x0004
#define BUTTON_RIGHT    0x0008
#define BUTTON_A        0x0040
#define BUTTON_B        0x0010
#define BUTTON_C        0x0020
#define BUTTON_START    0x0080
#define BUTTON_X        0x0100
#define BUTTON_Y        0x0200
#define BUTTON_Z        0x0400
#define BUTTON_MODE     0x0800

#define JOY_DATA *((volatile u8 *) 0xa10003)
#define JOY_CTRL *((volatile u8 *) 0xa10009)

u16 JOY_readJoypad(u16 joy);

#endif	/* JOY_H */
