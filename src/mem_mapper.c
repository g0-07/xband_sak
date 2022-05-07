/*
 * mem_mapper.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "vdp.h"
#include "joy.h"
#include "hard.h"
#include "globals.h"
#include "tools.h"
#include "mem_mapper.h"

void memMapper() {

    while (joy != 0);

    for (;;) {

        while (joy != 0);

        VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
        VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
        VDP_drawTextAnc(APLAN, "Flip the XBAND switch and", 0, 20, 13, CENTER);
        VDP_drawTextAnc(APLAN, "press any button!", 0, 20, 14, CENTER);

        while (joy != 0);
        while (joy == 0);

        for (;;) {

            mapXband();
            
            u8 not_found = 0;
            for (int i = 0; i < 4; i++) {
                if ("LODR"[i] != (*((u8 *)XBAND_LODR_LOCATION+i) & 0xff))
                 {
                    not_found = 1;
                    break;
                }
            }
            if (not_found == 0)break;

            VDP_drawTextAnc(APLAN, "WARNING", TILE_ATTR(3, 0, 0, 0), 19, 11, CENTER);
            VDP_drawTextAnc(APLAN, "XBAND not found! Press (A) to retry", 0, 19, 13, CENTER);
            VDP_drawTextAnc(APLAN, "or (START) to continue anyway.", 0, 19, 14, CENTER);

            while (joy != 0);
            while ((joy & BUTTON_A) == 0 && (joy & BUTTON_START) == 0);
            if ((joy & BUTTON_START))break; 
        }
        break;
    }
}

void mapXband() {

    asm("reset");
    XBAND_LEDENPORT = XBAND_LED_ENABLE;
    XBAND_CTLPORT = XBAND_ROMHI;
    HBLANK_HANDLER = RAM_HB_LOCATION;
    VBLANK_HANDLER = RAM_VB_LOCATION;
    XBAND_LEDPORT = XBAND_LED_4;
}
