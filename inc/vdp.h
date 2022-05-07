/*
 * vdp.h
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#ifndef VDP_H
#define	VDP_H

#include "types.h"

#define GFX_DATA_PORT           0xC00000
#define GFX_CTRL_PORT           0xC00004
#define GFX_HVCOUNTER_PORT      0xC00008

#define VDP_FIFOEMPTY_FLAG      (1 << 9)
#define VDP_FIFOFULL_FLAG       (1 << 8)
#define VDP_VINTPENDING_FLAG    (1 << 7)
#define VDP_SPROVERFLOW_FLAG    (1 << 6)
#define VDP_SPRCOLLISION_FLAG   (1 << 5)
#define VDP_ODDFRAME_FLAG       (1 << 4)
#define VDP_VBLANK_FLAG         (1 << 3)
#define VDP_FBLANK_FLAG         (1 << 2)
#define VDP_DMABUSY_FLAG        (1 << 1)
#define VDP_PALMODE_FLAG        (1 << 0)

#define FONT_LEN                128

#define BITMAP_WIDTH            32
#define BITMAP_HEIGHT           20
#define BITMAP_LEN              (BITMAP_WIDTH * BITMAP_HEIGHT)

#define TILE_SPACE              0xB000
#define TILE_MAXNUM             (TILE_SPACE / 32)
#define TILE_FONT               (TILE_SPACE - (FONT_LEN * 32))
#define TILE_FONTINDEX          (TILE_FONT / 32)
#define TILE_BITMAP             (TILE_FONT - (BITMAP_LEN * 32))
#define TILE_BITMAPINDEX        (TILE_BITMAP / 32)

#define WPLAN                   (TILE_SPACE + 0x0000)
#define HSCRL                   (TILE_SPACE + 0x0800)
#define SLIST                   (TILE_SPACE + 0x0C00)
#define APLAN                   (TILE_SPACE + 0x1000)
#define BPLAN                   (TILE_SPACE + 0x3000)

#define GFX_READ_VRAM_ADDR(adr)     ((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_READ_CRAM_ADDR(adr)     ((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x20)
#define GFX_READ_VSRAM_ADDR(adr)    ((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x10)

#define GFX_WRITE_VRAM_ADDR(adr)    ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_WRITE_CRAM_ADDR(adr)    ((0xC000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_WRITE_VSRAM_ADDR(adr)   ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x10)

#define GFX_DMA_VRAM_ADDR(adr)      ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x80)
#define GFX_DMA_CRAM_ADDR(adr)      ((0xC000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x80)
#define GFX_DMA_VSRAM_ADDR(adr)     ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x90)

#define GFX_VERT_SCROLL(adr)        GFX_WRITE_VSRAM_ADDR(adr)
#define GFX_HORZ_SCROLL(adr)        GFX_WRITE_VRAM_ADDR(adr)

#define GETVDPSTATUS(flag)          ((*(volatile u16*)(GFX_CTRL_PORT)) & (flag))
#define ISPALSYSTEM                 GETVDPSTATUS(VDP_PALMODE_FLAG)

#define GET_HVCOUNTER               (*(volatile u16*)(GFX_HVCOUNTER_PORT))
#define GET_HCOUNTER                (GET_HVCOUNTER & 0xFF)
#define GET_VCOUNTER                (GET_HVCOUNTER >> 8)

#define LEFT 0
#define RIGHT 1
#define CENTER 2

#define VDP_DMA_VRAM    0
#define VDP_DMA_CRAM    1
#define VDP_DMA_VSRAM   2

#define SPRITE_SIZE(w, h)   ((((w) - 1) << 2) | ((h) - 1))
#define SPRITE_POS(pos) (0x80 + pos)

#define TILE_ATTR(pal, pri, flipV, flipH)   (((flipH) << 11) + ((flipV) << 12) + ((pal) << 13) + ((pri) << 15))

extern u16 vdp_plan_w;
extern u16 vdp_plan_h;

typedef struct {
    s16 y;
    u8 size;
    u8 link;
    u16 attr;
    s16 x;
} Sprite;

void VDP_init();
void VDP_setReg(u16 reg, u8 value);
void VDP_setWidth256();
void VDP_setWidth320();
void VDP_setPlanSize(u16 w, u16 h);
void VDP_setAutoInc(u8 value);
void VDP_setHInterrupt(u8 value);
void VDP_setHilightShadow(u8 value);
void VDP_waitDMACompletion();
void VDP_waitFIFOEmpty();
void VDP_waitVSync();
void VDP_setHorizontalScroll(u16 plan, u16 line, u16 value);
void VDP_setVerticalScroll(u16 plan, u16 cell, u16 value);
void VDP_setPaletteColor(u16 numpal, u16 numcol, u16 value);
void VDP_setPalette(u16 num, const u16 *pal);
void VDP_getPalette(u16 num, u16 *pal);
u16 VDP_getPaletteColor(u16 numpal, u16 numcol);
void VDP_DoDMA(u8 area, u32 from, u16 to, u16 len);
void VDP_DoDMAFill(u16 to, u16 len, u16 value);
void VDP_DoDMACopy(u8 area, u16 from, u16 to, u16 len);
void VDP_spriteBuffToVram(u16 *sprite_buff, u16 sprite_num);
void VDP_loadTileTo(const u32 *data, u16 index, u16 num, u8 use_dma);
void VDP_loadFont(const u32 *font, u8 use_dma);
void VDP_setTile(u16 plan, u16 tile, u16 x, u16 y);
void VDP_fillTileRect(u16 plan, u16 tile, u16 x, u16 y, u16 w, u16 h);
void VDP_fillTileRectInc(u16 plan, u16 basetile, u16 x, u16 y, u16 w, u16 h);
void VDP_setTileRect(u16 plan, const u16 *data, u16 basetile, u16 x, u16 y, u16 w, u16 h);
void VDP_drawText(u16 plan, const char *str, u16 basetile, u16 x, u16 y);
void VDP_drawTextAnc(u16 plan, const char *str, u16 basetile, u16 x, u16 y, u8 anchor);

#endif	/* VDP_H */
