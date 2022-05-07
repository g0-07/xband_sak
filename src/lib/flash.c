/*
 * flash.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "flash.h"

u8 checkDataPolling(u32 dst, u16 truedata) {

    u16 currdata;
    u32 timeout = 0;

    truedata = truedata & 0x0080;
    while (timeout < MAX_TIMEOUT) {
        currdata = *FLASH_ADDRESS(dst);
        currdata = currdata & 0x0080;
        if (truedata == currdata)return 1;
        timeout++;
    }

    return 0;
}

u8 checkToggleReady(u32 dst) {

    u16 predata;
    u16 currdata;
    u32 timeout = 0;

    predata = *FLASH_ADDRESS(dst);
    predata = predata & 0x0040;
    while (timeout < MAX_TIMEOUT) {
        currdata = *FLASH_ADDRESS(dst);
        currdata = currdata & 0x0040;
        if (predata == currdata)return 1;
	    predata = currdata;
        timeout++;
    }

    return 0;
}

u8 flashWriteWord(u16 *srcword, u32 dst) {

    u8 resp;
    u32 destbuf = dst;
    u16 *sourcebuf = srcword;

    *FLASH_ADDRESS((0x555 << 1)) = 0x00aa;
    *FLASH_ADDRESS((0x2aa << 1)) = 0x0055;
    *FLASH_ADDRESS((0x555 << 1)) = 0x00a0;
    *FLASH_ADDRESS(destbuf) = *sourcebuf;
    resp = checkToggleReady(destbuf);

    return resp;
}

u8 flashErase() {

    *FLASH_ADDRESS((0x555 << 1)) = 0x00aa;
    *FLASH_ADDRESS((0x2aa << 1)) = 0x0055;
    *FLASH_ADDRESS((0x555 << 1)) = 0x0080;
    *FLASH_ADDRESS((0x555 << 1)) = 0x00aa;
    *FLASH_ADDRESS((0x2aa << 1)) = 0x0055;
    *FLASH_ADDRESS((0x555 << 1)) = 0x0010;
    
    if (checkDataPolling(0, 0xffff))return 1; else return 0;
}

u8 flashExist() {

    u8 resp = 0;
    u16 id1, id2;

    *FLASH_ADDRESS((0x555 << 1)) = 0x00aa;
    *FLASH_ADDRESS((0x2aa << 1)) = 0x0055;
    *FLASH_ADDRESS((0x555 << 1)) = 0x0090;

    id1 = *FLASH_ADDRESS((0x0000 << 1));
    id2 = *FLASH_ADDRESS((0x0001 << 1));

    if (id1 == FLASH_MFR_ID && (id2 == FLASH_AM29F400BB || id2 == FLASH_AM29F400BT))resp = 1;

    *FLASH_ADDRESS((0x555 << 1)) = 0x00aa;
    *FLASH_ADDRESS((0x2aa << 1)) = 0x0055;
    *FLASH_ADDRESS((0x555 << 1)) = 0x00f0;

    return resp;
}
