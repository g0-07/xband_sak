/*
 * main_menu.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "vdp.h"
#include "joy.h"
#include "menu_form.h"
#include "string.h"
#include "hard.h"
#include "select_file.h"
#include "globals.h"
#include "tools.h"
#include "sram_manager.h"
#include "rom_manager.h"

enum {
    MENU_SRAM = 0,
    MENU_ROM,
    MENU_SIZE
};

void mainMenu() {

    u8 resp;
    u8 *menu_items[MENU_SIZE];
    menu_items[MENU_SRAM] = "sram manager";
    menu_items[MENU_ROM] = "(flash)rom manager";
    s8 selector = 0;

    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
    VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);

    for (;;) {

        selector = menuFormShow(menu_items, MENU_SIZE, 2, selector, 1);

        switch (selector) {

            case MENU_SRAM:
                sramManager();
                break;

            case MENU_ROM:
                romManager();
                break;
        }
    }
}
