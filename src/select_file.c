/*
 * select_file.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "vdp.h"
#include "joy.h"
#include "select_file.h"
#include "ff.h"
#include "tools.h"
#include "memory.h"
#include "string.h"
#include "globals.h"

u8 selectFile(u8 select_dir, char *selected_path) {

    u16 i;
    u8 resp;
    u16 selector = 0;
    s16 item;
    s16 page = 0;
    s16 page_items;
    u8 dir_tree[MAX_DEPTH];
    u16 dir_tree_top = 0;
    FILINFO inf[MENU_ITEMS+1];    
    u16 key_friz = 0;
    u16 pages;

    while (joy != 0);

    for (;;) {

        while (joy != 0);

        cy = 1;
        dir_tree_top = 0;
        selector = 0;

        VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
        VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
        resp = f_mount(&FatFs, "", 0);

        if (resp != 0) {
            drawNum("ERROR: fat init failed: ", resp, 0, 1, cy++);
            while (joy != 0);
            while (joy == 0);
            return 0;
        }

        f_chdir("/");
        resp = readDir(select_dir, 0, inf);
        if (resp)return resp;

        VDP_fillTileRect(BPLAN, 0, 0, 0, 40, 28);
        VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);

        for (;;) {
            pages = dirsize / MENU_ITEMS;
            if (dirsize % MENU_ITEMS != 0)pages++;
            VDP_fillTileRect(BPLAN, 2, 0, 26, 40, 2);
            drawNum("page: ", selector / MENU_ITEMS + 1, 0, 1, 25);
            VDP_drawText(APLAN, "/", 0, 9, 25);
            drawNum("", pages, 0, 11, 25);
            VDP_drawTextAnc(APLAN, "(A)open dir, (left/right)select page", 0, 20, 26, CENTER);
            if (select_dir) {
                VDP_drawTextAnc(APLAN, "(B)back, (start)select current dir", 0, 20, 27, CENTER);
            } else {
                VDP_drawTextAnc(APLAN, "(B)back, (start)select file", 0, 20, 27, CENTER);
            }

            drawDir(1, 1, selector, MENU_ITEMS, inf);

            while (joy != 0 && key_friz < 30) {
                VDP_waitVSync();
                key_friz++;
            };

            while (joy != 0) {
                VDP_waitVSync();
                key_friz++;
                if (key_friz % 3 != 0)continue;
                break;
            }

            while (joy == 0)key_friz = 0;

            if ((joy & BUTTON_A) && (inf[selector-MENU_ITEMS*page].fattrib & AM_DIR) && dirsize > 0) {

                if (dir_tree_top < MAX_DEPTH) {
                    VDP_fillTileRect(APLAN, 0, 0, 0, 40, 26);
                    dir_tree[dir_tree_top++] = selector;
                    f_chdir(inf[selector-MENU_ITEMS*page].fname);
                    selector = 0;
                    resp = readDir(select_dir, 0, inf);
                    if (resp)return resp;
                }

                key_friz = 0;
                continue;
            }

            if ((joy & BUTTON_START) && (inf[selector-MENU_ITEMS*page].fattrib & AM_DIR) == 0) {
                resp = f_getcwd(selected_path, 254-strlen(inf[selector-MENU_ITEMS*page].fname));
                if (resp)return resp;
                u8 j = strlen(selected_path);
                strcpy(&selected_path[j+1], inf[selector-MENU_ITEMS*page].fname);
                selected_path[j] = 0x2f;
                return 0;
            }

            if ((joy & BUTTON_START) && (inf[selector-MENU_ITEMS*page].fattrib & AM_DIR) != 0 && select_dir && dirsize > 0) {
                resp = f_getcwd(selected_path, 242);
                return resp;
            }

            if ((joy & BUTTON_B && dir_tree_top < 1)) {

                return 99;
            }

            if ((joy & BUTTON_B) && dir_tree_top > 0) {

                VDP_fillTileRect(APLAN, 0, 0, 0, 40, 26);
                f_chdir("..");
                resp = readDir(select_dir, 0, inf);
                if (resp)return resp;

                selector = dir_tree[--dir_tree_top];

                key_friz = 0;
                continue;
            }

            if ((joy & BUTTON_UP)) {

                item = selector % MENU_ITEMS;
                page = selector / MENU_ITEMS;
                page_items = dirsize - page * MENU_ITEMS;
                if (page_items > MENU_ITEMS)page_items = MENU_ITEMS;
                item--;
                if (item < 0)item = page_items - 1;
                selector = page * MENU_ITEMS + item;

                continue;
            }

            if ((joy & BUTTON_DOWN)) {

                item = selector % MENU_ITEMS;
                page = selector / MENU_ITEMS;
                page_items = dirsize - page * MENU_ITEMS;
                if (page_items > MENU_ITEMS)page_items = MENU_ITEMS;
                item++;
                item %= page_items;
                selector = page * MENU_ITEMS + item;

                continue;
            }

            if ((joy & BUTTON_RIGHT) && dirsize > MENU_ITEMS) {

                item = selector % MENU_ITEMS;
                page = selector / MENU_ITEMS;
                page++;
                if (page * MENU_ITEMS >= dirsize) {
                    page = 0;
                }

                selector = page * MENU_ITEMS + item;
                if (selector >= dirsize)selector = dirsize - 1;

                resp = readDir(select_dir, page * MENU_ITEMS, inf);
                if (resp)return resp;

                key_friz = 0;
                VDP_waitVSync();
                VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
                continue;
            }

            if ((joy & BUTTON_LEFT) && dirsize > MENU_ITEMS) {

                item = selector % MENU_ITEMS;
                page = selector / MENU_ITEMS;
                page--;
                if (page < 0) {
                    page = dirsize / MENU_ITEMS - 1;
                    if (dirsize % MENU_ITEMS != 0)page++;
                }

                selector = page * MENU_ITEMS + item;
                if (selector >= dirsize)selector = dirsize - 1;

                resp = readDir(select_dir, page * MENU_ITEMS, inf);
                if (resp)return resp;

                key_friz = 0;
                VDP_waitVSync();
                VDP_fillTileRect(APLAN, 0, 0, 0, 40, 28);
                continue;
            }
        }
    }
    return 0;
}

u8 readDir(u8 select_dir, u16 start_index, FILINFO *inf)  {

    u16 i;
    u8 resp;

    resp = f_opendir(&dir, ".");

    if (resp != 0) {
        drawNum("ERROR: can not open dir: ", resp, 0, 1, cy++);
        while (joy != 0);
        while (joy == 0);
        return resp;
    }

    dirsize = 0;
    for (int i = 0;; i++) {
        resp = f_readdir(&dir, &inf[i]);
        if (resp || inf[i].fname[0] == 0)break;

        if ((inf[i].fattrib & AM_SYS) || (inf[i].fattrib & AM_HID)) {
            i--;
        } else if (select_dir && (inf[i].fattrib & AM_DIR) == 0) {
            i--;
        } else {
            if (dirsize < start_index || dirsize >= start_index + MENU_ITEMS)i--;
            dirsize++;
        }
    }
    f_closedir(&dir);
    
    return resp;
}
