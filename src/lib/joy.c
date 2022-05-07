/*
 * joy.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "joy.h"

u16 JOY_readJoypad(u16 joy) {

    u8 i, j;

    if (joy < 2) {
        // check joy high pass
        JOY_DATA = 0x40;
        asm("nop");
        asm("nop");
        asm("nop");
        i = JOY_DATA & 0x3f;

        // check joy low pass
        JOY_DATA = 0;
        asm("nop");
        asm("nop");
        asm("nop");
        j = (JOY_DATA & 0x30) << 2;

        return (~(i | j)) & 0xff;
    } else {
        return 0;
    }
}
