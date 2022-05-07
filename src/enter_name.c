/*
 * enter_name.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "enter_name.h"
#include "vdp.h"
#include "joy.h"
#include "globals.h"

const u8 charlist[] = {
    '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

u8 ch_buff[13];

u8 enterName(u8 *str, u8 max_len) {

    u8 len = 0;
    u8 x = (40 - 12) / 2;
    u8 y = (28 - 12) / 2;
    u8 i;
    u8 u;
    s8 selector = 0;
    ch_buff[12] = 0;

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);

    VDP_drawText(BPLAN, "enter file name", 0, (40 - sizeof("enter file name")) / 2, 2);

    for (i = 0; i < 6; i++) {
        for (u = 0; u < 12; u += 2) {
            ch_buff[u] = charlist[(u >> 1) + i * 6 ];
            ch_buff[u + 1] = 0x20;
        }
        VDP_drawText(BPLAN, ch_buff, TILE_ATTR(1, 0, 0, 0), x, y + i * 2);
    }

    for (;;) {

        if ((joy & BUTTON_C)) {
            return 0;
        }

        if ((joy & BUTTON_UP)) {
            selector -= 6;
            if (selector < 0)selector += sizeof(charlist);
        }

        if ((joy & BUTTON_DOWN)) {
            selector += 6;
            selector %= sizeof(charlist);
        }

        if ((joy & BUTTON_LEFT)) {
            if (selector % 6 == 0) {
                selector += 5;
            } else {
                selector--;
            }
        }

        if ((joy & BUTTON_RIGHT)) {
            if (selector % 6 == 5) {
                selector -= 5;
            } else {
                selector++;
            }
        }

        if ((joy & BUTTON_A) && len < max_len) {
            str[len++] = charlist[(u8) selector];
        }

        if ((joy & BUTTON_B) && len > 0) {
            str[--len] = 0x20;
        }

        if ((joy & BUTTON_START) && len > 0) {
            VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
            VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
            return len;
        }

        ch_buff[0] = charlist[(u8) selector];
        ch_buff[1] = 0;
        VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);

        VDP_drawText(APLAN, ch_buff, 0, x + selector % 6 * 2, y + selector / 6 * 2);
        VDP_drawText(APLAN, str, 0, x, y + 13);

        while (joy != 0);
        while (joy == 0);
    }
    return 0;
}
