/*
 * rom_manager.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "vdp.h"
#include "joy.h"
#include "hard.h"
#include "string.h"
#include "flash.h"
#include "globals.h"
#include "tools.h"
#include "select_file.h"
#include "enter_name.h"
#include "menu_form.h"
#include "mem_mapper.h"
#include "rom_manager.h"

enum {
    ROM_MANAGER_SAVE = 0,
    ROM_MANAGER_LOAD,
    ROM_MANAGER_HEX,
    ROM_MANAGER_LEN
};

void romManager() {

    u8 selector = 0;
    u8 *menu_items[ROM_MANAGER_LEN];

    menu_items[ROM_MANAGER_SAVE] = "backup ROM to SD";
    menu_items[ROM_MANAGER_LOAD] = "flash ROM from SD";
    menu_items[ROM_MANAGER_HEX] = "hex-viewer";

    for (;;) {
        selector = menuFormShow(menu_items, ROM_MANAGER_LEN, 2, selector, 0);
        if (selector >= 64)return;

        switch (selector) {
            case ROM_MANAGER_SAVE:
                backupRom((u8 *) FLASH_BASE, FLASH_SIZE, "rom");
                break;

            case ROM_MANAGER_LOAD:
                flashRom();
                break;

            case ROM_MANAGER_HEX:
                viewMem(FLASH_BASE);
                break;
        }
    }
}

void backupRom(u8 *mem_ptr, u32 length, char extension[3]) {

    u16 u;
    u16 i;
    u8 resp;
    char selected_path[255];
    char selected_name[9];

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    resp = selectFile(1, (char *)selected_path);
    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    if (resp)return;

    for (u8 i = 0; i < 9; i++)selected_name[i] = 0x20;
    resp = enterName(selected_name, 8);
    if (resp == 0)return;
    for (u8 i = 8; i > 0; i--)if (selected_name[i] == 0x20)selected_name[i] = 0x00;else break;

    u8 j = strlen(selected_path);
    u8 k = strlen(selected_name);
    strcpy(&selected_path[j], selected_name);
    selected_path[j+k] = '.';
    selected_path[j+1+k] = extension[0];
    selected_path[j+2+k] = extension[1];
    selected_path[j+3+k] = extension[2];
    selected_path[j+4+k] = 0x00;

    cy = 1;
    resp = f_stat(selected_path, (void *)0);
    if (resp == 0) {
        VDP_drawText(APLAN, "overwrite existing file?", 0, 1, cy++);
        VDP_drawText(APLAN, "(A)yes,  (B)no", 0, 1, cy++);

        while (joy != 0);
        while ((joy & BUTTON_A) == 0 && (joy & BUTTON_B) == 0);
        if ((joy & BUTTON_B))return;      
        f_unlink(selected_path);
    } else {
        VDP_drawText(APLAN, "create new file...", 0, 1, cy++);
    }
    
    resp = f_open(&file, selected_path, FA_WRITE | FA_CREATE_ALWAYS);
    if (resp)return;
    VDP_drawText(APLAN, "saving dump...", 0, 1, cy++);

    UINT bw;
    u8 *sector_buff_8 = (u8 *) file.buf;
    progress_step = 0;
    cy++;
    for (u32 i = 0; i < length; i += 512) {
        for (u16 u = 0; u < 512; u++)sector_buff_8[u] = *mem_ptr++ & 0xff;
        resp = f_write(&file, sector_buff_8, 512, &bw);
        if (resp != 0 || bw < 512) {
            f_close(&file);
            drawNum("ERROR: can not write file: ", resp, 0, 1, cy++);
            while (joy != 0);
            while (joy == 0);
            return;
        }
        if (i % 4096 == 0)drawProgress(i, length, 1, cy);
    }
    drawProgress(length, length, 1, cy);
    cy++;cy++;
    f_close(&file);

    VDP_drawText(APLAN, "done!", 0, 1, cy++);
    VDP_drawText(APLAN, "press any key", 0, 1, cy++);
    while (joy != 0);
    while (joy == 0);
}

void flashRom() {

    u8 resp;
    char selected_path[255];

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    resp = selectFile(0, (char *)selected_path);
    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    if (resp)return;
    cy = 1;

    resp = flashExist();
    if (resp == 0) {
        VDP_drawText(APLAN, "ERROR: no compatible flash-rom found", 0, 1, cy++);
        while (joy != 0);
        while (joy == 0);
        return;    
    }

    resp = f_open(&file, selected_path, FA_READ);
    if (resp)return;
    if (f_size(&file) != FLASH_SIZE) {
        f_close(&file);
        VDP_drawText(APLAN, "ERROR: wrong filesize", 0, 1, cy++);
        while (joy != 0);
        while (joy == 0);
        return;        
    }

    VDP_drawText(APLAN, "erasing...", 0, 1, cy++);
    resp = flashErase();
    if (resp == 0) {
        f_close(&file);
        VDP_drawText(APLAN, "ERROR: erasing failed", 0, 1, cy++);
        while (joy != 0);
        while (joy == 0);
        return;         
    }

    VDP_drawText(APLAN, "flashing...", 0, 1, cy++);
    UINT br;
    progress_step = 0;
    cy++;
    VBLANK_HANDLER = RAM_RTE_LOCATION;

    for (u32 i = 0; i < FLASH_SIZE; i += 512) {
        resp = f_read(&file, &file.buf, 512, &br);
        if (resp != 0 || br < 512) {
            f_close(&file);
            VBLANK_HANDLER = RAM_VB_LOCATION;
            drawNum("ERROR: can not read input file: ", resp, 0, 1, cy++);
            while (joy != 0);
            while (joy == 0);
            return;
        }
        for (u32 u = 0; u < 512; u += 2) {
            resp = flashWriteWord((u16 *)&file.buf[u], i+u);
            if (resp == 0) {
                f_close(&file);
                VBLANK_HANDLER = RAM_VB_LOCATION;
                VDP_drawText(APLAN, "ERROR: flashing failed", 0, 1, cy++);
                while (joy != 0);
                while (joy == 0);
                return;         
            }
        }
        if (i % 4096 == 0)drawProgress(i, FLASH_SIZE, 1, cy);
    }
    drawProgress(FLASH_SIZE, FLASH_SIZE, 1, cy);
    VBLANK_HANDLER = RAM_VB_LOCATION;
    cy++;cy++;

    VDP_drawText(APLAN, "verifying...", 0, 1, cy++);
    progress_step = 0;
    cy++;
    f_lseek(&file, 0);
    for (u32 i = 0; i < FLASH_SIZE; i += 512) {
        resp = f_read(&file, &file.buf, 512, &br);
        if (resp != 0 || br < 512) {
            f_close(&file);
            drawNum("ERROR: can not read input file: ", resp, 0, 1, cy++);
            while (joy != 0);
            while (joy == 0);
            return;
        }
        for (u32 u = 0; u < 512; u += 2) {
            if (*(u16 *)&file.buf[u] != *((u16 *)(FLASH_BASE+i+u))) {
                f_close(&file);
                VDP_drawText(APLAN, "ERROR: verify failed", 0, 1, cy++);
                while (joy != 0);
                while (joy == 0);
                return;              
            }
        }
        if (i % 4096 == 0)drawProgress(i, FLASH_SIZE, 1, cy);
    }
    drawProgress(FLASH_SIZE, FLASH_SIZE, 1, cy);
    f_close(&file);
    cy++;cy++;

    VDP_drawText(APLAN, "done!", 0, 1, cy++);
    VDP_drawText(APLAN, "press any key", 0, 1, cy++);
    while (joy != 0);
    while (joy == 0);
}
