/*
 * tools.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "vdp.h"
#include "hard.h"
#include "mmc.h"
#include "joy.h"
#include "string.h"
#include "memory.h"
#include "tools.h"
#include "globals.h"
#include "select_file.h"

u8 str_buff[41];

void drawHexNum(u8 *comment, u32 num, u8 min, u16 base, u8 x, u8 y) {

    u8 comment_len = 0;
    while (comment[comment_len] != 0) {
        str_buff[comment_len] = comment[comment_len];
        comment_len++;
    }
    STR_intToHexStringMin(num, &str_buff[comment_len], min);
    VDP_drawText(APLAN, str_buff, base, x, y);
}

void drawNum(u8 *comment, u32 num, u16 base, u8 x, u8 y) {

    u8 comment_len = 0;
    while (comment[comment_len] != 0) {
        str_buff[comment_len] = comment[comment_len];
        comment_len++;
    }
    STR_intToDecString(num, &str_buff[comment_len]);
    VDP_drawText(APLAN, str_buff, base, x, y);
}

void drawDir(u8 x, u8 y, u16 selector, u8 items, FILINFO *inf) {

    u8 i;
    u16 base;
    u16 page = selector / items;
    selector %= items;
    u16 item_idx = page * items;

    if (dirsize == 0) {
        VDP_drawText(APLAN, "Empty folder found.", 0, 1, 1);
    } else {
        for (i = 0; i < items && item_idx < dirsize; i++) {
            base = TILE_ATTR(1, 0, 0, 0);
            if (selector == i) {
                base = 0;
            } else if ((inf[i].fattrib & AM_DIR)) {
                base = TILE_ATTR(2, 0, 0, 0);
            }
            STR_intToDecStringMin(item_idx++, str_buff, 3);
            VDP_drawText(APLAN, str_buff, base, x, y);
            VDP_drawText(APLAN, inf[i].fname, base, x + 4, y);
            
            if ((inf[i].fattrib & AM_DIR)) {
                VDP_drawText(APLAN, "DIR", base, x + 34, y);
            } else {
                drawNum("", inf[i].fsize >> 10, base, x + 34, y);
            }
            y++;
        }
    }
}

void viewMem(u32 addr) {

    u8 len = 24;
    u16 key_friz = 0;

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 2, 0, 27, 40, 1);
    VDP_drawTextAnc(APLAN, "(up/down)select page, (B)back", 0, 20, 27, CENTER);

    for (;;) {

        drawMem(addr, len);
        while (joy != 0 && key_friz < 50) {
            VDP_waitVSync();
            key_friz++;
        };
        while (joy != 0) {
            VDP_waitVSync();
            key_friz++;
            if (key_friz % 4 != 0)continue;
            break;
        }
        while (joy == 0)key_friz = 0;

        if ((joy & BUTTON_UP))addr -= len * 8;
        if ((joy & BUTTON_DOWN))addr += len * 8;
        if ((joy & BUTTON_B))return;
    }
}

void drawMem(u32 mem, u16 len) {

    u8 i;
    u8 y = 0;
    u8 x = 0;
    u16 val;

    while (len--) {

        x = 0;
        str_buff[x++] = ':';
        STR_intToHexStringMin(mem, &str_buff[x], 6);
        x += 6;

        str_buff[x++] = ':';
        for (i = 0; i < 4; i++) {

            STR_intToHexStringMin(*((volatile u16 *) mem), &str_buff[x], 4);
            mem += 2;
            x += 4;
            str_buff[x++] = 0x20;
        }
        mem -= 8;
        str_buff[x - 1] = ':';

        for (i = 0; i < 4; i++) {

            val = *((volatile u16 *) mem);
            mem += 2;
            str_buff[x++] = val >> 8 & 0xff;
            str_buff[x++] = val & 0xff;
        }
        str_buff[x++] = ':';
        for (i = 0; i < x; i++)if (str_buff[i] == 0)str_buff[i] = 0x20;
        str_buff[x] = 0;

        VDP_drawText(APLAN, str_buff, 0, 1, y++);
        if (y > 27)return;
    }
}

void drawProgress(u32 value, u32 total, u8 x, u8 y) {

    char percentage[3];

    if (progress_step == 0)VDP_fillTileRect(BPLAN, 2, x, y, 10, 1);
    VDP_fillTileRect(BPLAN, 1, x, y, progress_step+1, 1);
    STR_intToDecStringMin(((value*100)/total), (char *)&percentage, 1);
    VDP_drawTextAnc(APLAN, (char *)&percentage, TILE_ATTR(0, 0, 0, 0), x+14, y, RIGHT);
    VDP_drawText(APLAN, "%", 0, x+15, y);
    progress_step = ((value*100)/total)/10;
}
