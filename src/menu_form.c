/*
 * menu_form.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "vdp.h"
#include "joy.h"
#include "globals.h"
#include "tools.h"

u8 menuFormShow(u8 **items, u8 size, u8 step, s8 selector, u8 root) {

    u8 x = 20;
    u8 y = 27 / 2 - size * step / 2;
    u8 i;

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 2, 0, 26, 40, 1);

    VDP_drawTextAnc(APLAN, "(A/START)open, (B)back", 0, 20, 26, CENTER);
    if (root) {
        VDP_drawTextAnc(APLAN, "xband_sak", TILE_ATTR(1, 0, 0, 0), 1, 1, LEFT);
        drawNum("v", APP_VERSION, TILE_ATTR(1, 0, 0, 0), 11, 1);
    }

    for (;;) {

        for (i = 0; i < size; i++) {
            if (selector == i) {
                VDP_drawTextAnc(APLAN, items[i], 0, x, y + i * step, CENTER);
            } else {
                VDP_drawTextAnc(APLAN, items[i], TILE_ATTR(1, 0, 0, 0), x, y + i * step, CENTER);
            }
        }

        while (joy != 0);
        while (joy == 0);

        if ((joy & BUTTON_UP)) {
            selector--;
            if (selector < 0) {
                selector = size - 1;
            }
            continue;
        }

        if ((joy & BUTTON_DOWN)) {
            selector++;
            if (selector >= size) {
                selector = 0;
            }
            continue;
        }

        if (!(joy & (BUTTON_A | BUTTON_B | BUTTON_START)))continue;

        if ((joy & (BUTTON_A | BUTTON_START))) {
            while (joy != 0);
            return selector;
        }

        if ((joy & BUTTON_B) && !root) {
            while (joy != 0);
            return selector + 64;
        }
    }
    return 0;
}
