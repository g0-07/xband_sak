/*
 * vdp.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "vdp.h"

u8 vdp_reg_0x0c;
u16 vdp_plan_w;
u16 vdp_plan_h;

const u8 vdp_init_reg_values[] = {
    0x04, /* reg. 0 - Enable HBL */
    0x74, /* reg. 1 - Enable display, VBL, DMA + VCell size */
    0x30, /* reg. 2 - Plane A =$30*$400=$C000 */
    0x2C, /* reg. 3 - Window  =$2C*$400=$B000 */
    0x07, /* reg. 4 - Plane B =$7*$2000=$E000 */
    0x5E, /* reg. 5 - sprite table begins at $BC00=$5E*$200 */
    0x00, /* reg. 6 - not used */
    0x00, /* reg. 7 - Background Color number*/
    0x00, /* reg. 8 - not used */
    0x00, /* reg. 9 - not used */
    0x01, /* reg 10 - HInterrupt timing */
    0x00, /* reg 11 - $0000abcd a=extr.int b=vscr cd=hscr */
    0x81, /* reg 12 - hcell mode + shadow/highight + interlaced mode (40 cell, no shadow, no interlace)*/
    0x2E, /* reg 13 - HScroll Table = $B800 */
    0x00, /* reg 14 - not used */
    0x02, /* reg 15 - auto increment data */
    0x11, /* reg 16 - scrl screen v&h size (64x64) */
    0x00, /* reg 17 - window hpos */
    0xFF, /* reg 18 - window vpos */
};

void VDP_init() {

    u8 i;
    vdp_reg_0x0c = vdp_init_reg_values[0x0c];
    for (i = 0x00; i < 0x13; i++) *((volatile u16 *) GFX_CTRL_PORT) = 0x8000 | (i << 8) | vdp_init_reg_values[i];
    vdp_plan_w = ((vdp_init_reg_values[0x10] & 0xF) + 1) << 5;
    vdp_plan_h = ((vdp_init_reg_values[0x10] >> 4) + 1) << 5;
    VDP_setHInterrupt(0);
}

void VDP_setReg(u16 reg, u8 value) {

    *((volatile u16 *) GFX_CTRL_PORT) = 0x8000 | (reg << 8) | value;
    if (reg == 0x10) {
        vdp_plan_w = ((value & 0xF) + 1) << 5;
        vdp_plan_h = ((value >> 4) + 1) << 5;
    }
}

void VDP_setWidth256() {

    vdp_reg_0x0c &= ~0x81;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x8C00 | vdp_reg_0x0c;
}

void VDP_setWidth320() {

    vdp_reg_0x0c |= ~0x81;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x8C00 | vdp_reg_0x0c;
}

void VDP_setPlanSize(u16 w, u16 h) {

    u16 val = (((h >> 5) - 1) << 4) | (((w >> 5) - 1) << 0);
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9000 | val;
}

void VDP_setAutoInc(u8 value) {

    *((volatile u16 *) GFX_CTRL_PORT) = 0x8F00 | value;
}

void VDP_setHInterrupt(u8 value) {

    if (value) {
        value = 0x14 | 0x10;
    } else {
        value = 0x14 & ~0x10;
    }
    *((volatile u16 *) GFX_CTRL_PORT) = 0x8000 | value;
}

void VDP_setHilightShadow(u8 value) {

    if (value) {
        vdp_reg_0x0c |= 0x08;
    } else {
        vdp_reg_0x0c &= ~0x08;
    }
    *((volatile u16 *) GFX_CTRL_PORT) = 0x8C00 | vdp_reg_0x0c;
}

void VDP_waitDMACompletion() {

    while (GETVDPSTATUS(VDP_DMABUSY_FLAG));
}

void VDP_waitFIFOEmpty() {

    while (!GETVDPSTATUS(VDP_FIFOEMPTY_FLAG));
}

void VDP_waitVSync() {

    u16 vdp_state;
    vdp_state = VDP_VBLANK_FLAG;
    while (vdp_state & VDP_VBLANK_FLAG) vdp_state = *((volatile u16 *) GFX_CTRL_PORT);
    while (!(vdp_state & VDP_VBLANK_FLAG)) vdp_state = *((volatile u16 *) GFX_CTRL_PORT);
}

void VDP_setHorizontalScroll(u16 plan, u16 line, u16 value) {

    u16 addr;
    addr = HSCRL + ((line & 0xFF) * 4);
    if (plan == BPLAN) addr += 2;

    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);
    *((volatile u16 *) GFX_DATA_PORT) = value;
}

void VDP_setVerticalScroll(u16 plan, u16 cell, u16 value) {

    u16 addr;
    addr = (cell & 0x1F) * 4;
    if (plan == BPLAN) addr += 2;

    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VSRAM_ADDR(addr);
    *((volatile u16 *) GFX_DATA_PORT) = value;
}

void VDP_setPaletteColor(u16 numpal, u16 numcol, u16 value) {

    u16 addr;
    addr = (numpal * 32) + (numcol * 2);
    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_CRAM_ADDR(addr);
    *((volatile u16 *) GFX_DATA_PORT) = value;
}

void VDP_setPalette(u16 num, const u16 *pal) {

    const u16 *src;
    u16 i;
    u16 addr;

    VDP_setAutoInc(2);
    src = pal;
    addr = num * 32;

    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_CRAM_ADDR(addr);
    i = 16;
    while (i--) {
        *((volatile u16 *) GFX_DATA_PORT) = *src++;
    }
}

void VDP_getPalette(u16 num, u16 *pal) {

    u16 *dest;
    u16 i;
    u16 addr;

    VDP_setAutoInc(2);
    dest = pal;
    addr = num * 32;
    *((volatile u32 *) GFX_CTRL_PORT) = GFX_READ_CRAM_ADDR(addr);

    i = 16;
    while (i--) *dest++ = *((volatile u16 *) GFX_DATA_PORT);
}

u16 VDP_getPaletteColor(u16 numpal, u16 numcol) {

    u16 addr;

    addr = (numpal * 32) + (numcol * 2);
    *((volatile u32 *) GFX_CTRL_PORT) = GFX_READ_CRAM_ADDR(addr);

    return *((volatile u16 *) GFX_DATA_PORT);
}

void VDP_DoDMA(u8 area, u32 from, u16 to, u16 len) {

    VDP_setAutoInc(2);

    /* Setup DMA length (in word here) */
    len >>= 1;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9300 + (len & 0xff);
    len >>= 8;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9400 + (len & 0xff);

    /* Setup DMA address */
    from >>= 1;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9500 + (from & 0xff);
    from >>= 8;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9600 + (from & 0xff);
    from >>= 8;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9700 + (from & 0xff);


    /* Enable DMA */
    switch (area) {
        case VDP_DMA_VRAM:
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_VRAM_ADDR(to);
            break;

        case VDP_DMA_CRAM:
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_CRAM_ADDR(to);
            break;

        case VDP_DMA_VSRAM:
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_VSRAM_ADDR(to);
            break;
    }
}

void VDP_DoDMAFill(u16 to, u16 len, u16 value) {

    /* Setup DMA length */
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9300 + (len & 0xff);
    len >>= 8;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9400 + (len & 0xff);

    /* Setup DMA operation (VRAM FILL) */
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9780;

    /* Enable DMA */
    VDP_setAutoInc(1);
    *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_VRAM_ADDR(to);

    /* set up value to fill */
    *((volatile u16 *) GFX_DATA_PORT) = value << 8;
}

void VDP_DoDMACopy(u8 area, u16 from, u16 to, u16 len) {

    /* Setup DMA length */
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9300 + (len & 0xff);
    len >>= 8;
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9400 + (len & 0xff);

    /* Setup DMA address */
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9500 + (from & 0xff);
    *((volatile u16 *) GFX_CTRL_PORT) = 0x9600 + ((from >> 8) & 0xff);

    /* Setup DMA operation (VRAM COPY) */
    *((volatile u16 *) GFX_CTRL_PORT) = 0x97C0;

    /* Enable DMA */
    switch (area) {
        case VDP_DMA_VRAM:
            VDP_setAutoInc(1);
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_VRAM_ADDR(to);
            break;

        case VDP_DMA_CRAM:
            VDP_setAutoInc(2);
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_CRAM_ADDR(to);
            break;

        case VDP_DMA_VSRAM:
            VDP_setAutoInc(2);
            *((volatile u32 *) GFX_CTRL_PORT) = GFX_DMA_VSRAM_ADDR(to);
            break;
    }
}

void VDP_spriteBuffToVram(u16 *sprite_buff, u16 sprite_num) {

    VDP_DoDMA(VDP_DMA_VRAM, (u32) sprite_buff, SLIST, sprite_num << 3);
}

void VDP_addSprite(u16 *buff_ptr, u16 x, u16 y, u8 size, u16 tile_attr, u8 link) {

    buff_ptr[0] = 0x80 + y;
    // size & link
    buff_ptr[1] = (size << 8) | link;
    // tile attribut
    buff_ptr[2] = tile_attr;
    // x position
    buff_ptr[3] = 0x80 + x;
}

void VDP_loadTileTo(const u32 *data, u16 index, u16 num, u8 use_dma) {

    u16 addr;

    addr = index << 5;

    if (use_dma) VDP_DoDMA(VDP_DMA_VRAM, (u32) data, addr, num << 5);
    else {

        const u32 *src;
        u16 i;

        VDP_setAutoInc(2);

        *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);

        src = data;

        i = num;
        while (i--) {

            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
            *((volatile u32 *) GFX_DATA_PORT) = *src++;
        }
    }
}

void VDP_loadFont(const u32 *font, u8 use_dma) {

    VDP_loadTileTo(font, TILE_FONTINDEX, 128, use_dma);
}

void VDP_setTile(u16 plan, u16 tile, u16 x, u16 y) {

    u32 addr = plan + ((x + (vdp_plan_w * y)) << 1);

    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);
    *((volatile u16 *) GFX_DATA_PORT) = tile;
}

void VDP_fillTileRect(u16 plan, u16 tile, u16 x, u16 y, u16 w, u16 h) {

    u32 addr;
    u32 planwidth;
    u16 i, j;

    VDP_setAutoInc(2);

    planwidth = vdp_plan_w;
    addr = plan + ((x + (planwidth * y)) << 1);

    i = h;
    while (i--) {

        *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);
        j = w;
        while (j--) *((volatile u16 *) GFX_DATA_PORT) = tile;
        addr += planwidth << 1;
    }
}

void VDP_fillTileRectInc(u16 plan, u16 basetile, u16 x, u16 y, u16 w, u16 h) {

    u32 addr;
    u32 planwidth;
    u16 tile;
    u16 i, j;

    VDP_setAutoInc(2);


    planwidth = vdp_plan_w;
    addr = plan + ((x + (planwidth * y)) << 1);
    tile = basetile;

    i = h;
    while (i--) {

        *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);
        j = w;
        while (j--) *((volatile u16 *) GFX_DATA_PORT) = tile++;
        addr += planwidth << 1;
    }
}

void VDP_setTileRect(u16 plan, const u16 *data, u16 basetile, u16 x, u16 y, u16 w, u16 h) {

    const u16 *src;
    u32 addr;
    u32 planwidth;
    u16 i, j;

    VDP_setAutoInc(2);

    planwidth = vdp_plan_w;
    addr = plan + ((x + (planwidth * y)) << 1);
    src = data;

    i = h;
    while (i--) {

        *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);
        j = w;
        while (j--) *((volatile u16 *) GFX_DATA_PORT) = basetile | *src++;
        addr += planwidth << 1;
    }
}

void VDP_drawText(u16 plan, const char *str, u16 basetile, u16 x, u16 y) {

    u8 len = 0;
    u32 addr;
    const u8 *src = str;

    while (str[len] && len++ < 40);
    if (len > (vdp_plan_w - x)) len = vdp_plan_w - x;

    VDP_setAutoInc(2);
    addr = plan + ((x + (vdp_plan_w * y)) << 1);
    *((volatile u32 *) GFX_CTRL_PORT) = GFX_WRITE_VRAM_ADDR(addr);

    while (len--) *((volatile u16 *) GFX_DATA_PORT) = basetile | ((*src++) - 32 + TILE_FONTINDEX);
}

void VDP_drawTextAnc(u16 plan, const char *str, u16 basetile, u16 x, u16 y, u8 anchor) {

    u8 len = 0;
    s16 xx = x;

    while (str[len] != 0 && len < 255)len++;

    if ((anchor & CENTER)) {
        xx -= len / 2;
    } else if ((anchor & RIGHT)) {
        xx -= len;
    }
    if (xx < 0)xx = 0;
    VDP_drawText(plan, str, basetile, xx, y);
}
