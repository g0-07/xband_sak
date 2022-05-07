/*
 * main.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "globals.h"
#include "font.h"
#include "joy.h"
#include "hard.h"
#include "vdp.h"
#include "tools.h"
#include "mem_mapper.h"
#include "main_menu.h"

void in() {

}

void hb() {

}

void vb() {

    timer++;
    joy = 0;
    joy = JOY_readJoypad(JOY_1);
}

u8 init();

u8 state;

const u32 color1[] = {
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
    0x11111111,
};

const u32 color2[] = {
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
};

int main() {

    cfg = 0;

    VDP_setTile(APLAN, 0 | TILE_ATTR(0, 0, 0, 0), 0, 0);
    VDP_setTile(APLAN, 0 | TILE_ATTR(1, 0, 0, 0), 0, 0);

    init();
    memMapper();
    mainMenu();

    return 0;
}

u8 init() {

    VBL_CATCH_OFF;
    VBL_PORT = 0x0020;
    cy = 1;
    joy = 0;

    VDP_init();
    VDP_setHInterrupt(0);
    VDP_loadFont(font_base, 0);
    VDP_setPaletteColor(0, 0, 0x000);
    VDP_setPaletteColor(0, 15, 0xfff);
    VDP_setPaletteColor(0, 1, 0xaaa);
    VDP_setPaletteColor(0, 2, 0x444);
    VDP_setPaletteColor(1, 15, 0x444);
    VDP_setPaletteColor(2, 15, 0x2aa);
    VDP_setPaletteColor(3, 15, 0x00f);
    VDP_loadTileTo(color1, 1, 1, 0);
    VDP_loadTileTo(color2, 2, 1, 0);

    JOY_CTRL = 0x40;

    return 0;
}
