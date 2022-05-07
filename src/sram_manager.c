/*
 * sram_manager.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "vdp.h"
#include "joy.h"
#include "hard.h"
#include "string.h"
#include "globals.h"
#include "tools.h"
#include "select_file.h"
#include "enter_name.h"
#include "menu_form.h"
#include "rom_manager.h"
#include "sram_manager.h"

enum {
    SRM_MANAGER_SAVE = 0,
    SRM_MANAGER_LOAD,
    SRM_MANAGER_HEX,
    SRM_MANAGER_LEN
};

void sramManager() {

    u8 selector = 0;
    u8 *menu_items[SRM_MANAGER_LEN];

    menu_items[SRM_MANAGER_SAVE] = "backup SRAM to SD";
    menu_items[SRM_MANAGER_LOAD] = "restore SRAM from SD";
    menu_items[SRM_MANAGER_HEX] = "hex-viewer";

    for (;;) {
        selector = menuFormShow(menu_items, SRM_MANAGER_LEN, 2, selector, 0);
        if (selector >= 64)return;

        switch (selector) {
            case SRM_MANAGER_SAVE:
                backupRom((u8 *) SRAM_BASE, SRAM_SIZE, "srm");
                break;

            case SRM_MANAGER_LOAD:
                restoreSram();
                break;

            case SRM_MANAGER_HEX:
                viewMem(SRAM_BASE);
                break;
        }
    }
}

void restoreSram() {
    
    u8 resp;
    char selected_path[255];
    u8 *sram_ptr = (u8 *) SRAM_BASE;

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    resp = selectFile(0, (char *)selected_path);
    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
    if (resp)return;
    cy = 1;

    resp = f_open(&file, selected_path, FA_READ);
    if (resp)return;
    if (f_size(&file) != SRAM_SIZE) {
        f_close(&file);
        VDP_drawText(APLAN, "ERROR: wrong filesize", 0, 1, cy++);
        while (joy != 0);
        while (joy == 0);
        return;        
    }

    VDP_drawText(APLAN, "restoring...", 0, 1, cy++);
    UINT br;
    progress_step = 0;
    cy++;
    for (u32 i = 0; i < SRAM_SIZE; i += 512) {
        resp = f_read(&file, &file.buf, 512, &br);
        if (resp != 0 || br < 512) {
            f_close(&file);
            drawNum("ERROR: can not read input file: ", resp, 0, 1, cy++);
            while (joy != 0);
            while (joy == 0);
            return;
        }
        for (u16 u = 0; u < 512; u++) {
            if (i == 0 && ((u >= 0x70 && u <= 0x73) || (u >= 0x78 && u <= 0x7b))) {
                *sram_ptr++;
                continue;
            }
            *sram_ptr++ = file.buf[u];
        }
        if (i % 4096 == 0)drawProgress(i, SRAM_SIZE, 1, cy);
    }
    drawProgress(SRAM_SIZE, SRAM_SIZE, 1, cy);
    cy++;cy++;
    f_close(&file);

    VDP_drawText(APLAN, "done!", 0, 1, cy++);
    VDP_drawText(APLAN, "press any key", 0, 1, cy++);
    while (joy != 0);
    while (joy == 0);
}
