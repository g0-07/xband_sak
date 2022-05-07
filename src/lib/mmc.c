/*
 * mmc.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"
#include "mmc.h"

#define CMD0  0x40    // GO_IDLE_STATE
#define CMD1  0x41    // SEND_OP_COND
#define CMD8  0x48    // SEND_IF_COND
#define CMD9  0x49    // SEND_CSD
#define CMD12 0x4C    // STOP_TRANSMISSION
#define CMD17 0x51    // READ_SINGLE_BLOCK
#define CMD18 0x52    // READ_MULTIPLE_BLOCK
#define CMD24 0x58    // WRITE_BLOCK
#define CMD25 0x59    // WRITE_MULTIPLE_BLOCK
#define CMD55 0x77    // APP_CMD
#define CMD58 0x7A    // READ_OCR

#define ACMD41 0x69   // APP_SEND_OP_COND

u8 csd_data[16];
u8 ocr_data[4];
u32 stor_addr;

u8 mmcCrc7(u8 *buff) {

    u16 a;
    u16 crc = 0;
    u8 len = 5;

    while (len--) {
        crc ^= *buff++;
        a = 8;
        do {
            crc <<= 1;
            if (crc & (1 << 8)) crc ^= 0x12;
        } while (--a);
    }
    return (crc & 0xfe);
}

u8 mmcCmd(u8 cmd, u32 arg) {

    u8 crc;
    u8 buff[5];

    buff[0] = cmd;
    buff[1] = (arg >> 24) & 0xff;
    buff[2] = (arg >> 16) & 0xff;
    buff[3] = (arg >> 8) & 0xff;
    buff[4] = (arg >> 0) & 0xff;
    crc = mmcCrc7(buff) | 1;

    SPI_BUSY;
    SS_ON;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = cmd;
    SPI_BUSY;
    SPI_PORT = (arg >> 24);
    SPI_BUSY;
    SPI_PORT = (arg >> 16);
    SPI_BUSY;
    SPI_PORT = (arg >> 8);
    SPI_BUSY;
    SPI_PORT = arg;
    SPI_BUSY;
    SPI_PORT = crc;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SS_OFF;
    SPI_BUSY;
    return SPI_PORT & 0xff;
}

u8 mmcFastCmd(u8 cmd, u32 arg) {

    SPI_BUSY;
    SS_ON;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = cmd;
    SPI_BUSY;
    SPI_PORT = (arg >> 24);
    SPI_BUSY;
    SPI_PORT = (arg >> 16);
    SPI_BUSY;
    SPI_PORT = (arg >> 8);
    SPI_BUSY;
    SPI_PORT = arg;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SS_OFF;
    SPI_BUSY;
    return SPI_PORT & 0xff;
}

u8 mmcInit() {

    u16 i;
    u8 resp;

    card_type = 0;
    resp = 0;

    SS_OFF;
    SPI_HI_SPEED_OFF;

    for (i = 0; i < 20; i++) {
        SPI_BUSY;
        SPI_PORT = 0xff;
    }

    i = 0;
    while (resp != 1 && i++ < 2) {
        resp = mmcCmd(CMD0, 0);
    }

    resp = mmcCmd(CMD8, 0x1aa);
    for (i = 0; i < 5; i++) {
        SPI_BUSY;
        SPI_PORT = 0xff;
    }
    if (resp == 0xff)return 2;
    if (resp != 5)card_type |= SD_V2;

    if (card_type == SD_V2) {
        for (i = 0; i < WAIT_LEN; i++) {
            resp = mmcCmd(CMD55, 0);
            if (resp == 0xff)return 3;
            if (resp != 1)continue;

            resp = mmcCmd(ACMD41, 0x40300000);
            if (resp == 0xff)return 4;
            if (resp != 0)continue;

            break;
        }
        if (i == WAIT_LEN)return 5;
        resp = mmcReadOCR();
        if (resp != 0)return 20 + resp;   
        if ((ocr_data[0] & 0x40))card_type |= SD_HC;
    } else {
        resp = mmcCmd(CMD55, 0);
        if (resp == 0xff)return 7;

        resp = mmcCmd(ACMD41, 0);
        if (resp == 0xff)return 8;

        for (i = 0; i < WAIT_LEN; i++) {
            if (resp < 1) {
                resp = mmcCmd(CMD55, 0);
                if (resp == 0xff)return 9;
                if (resp != 1)continue;

                resp = mmcCmd(ACMD41, 0);
                if (resp == 0xff)return 10;
                if (resp != 0)continue;
            } else {
                resp = mmcCmd(CMD1, 0);
                if (resp != 0)continue;
            }
            break;
        }
        if (i == WAIT_LEN)return 11;
    }

    SS_ON;
    SPI_PORT = 0xff;
    SPI_BUSY;

    SPI_HI_SPEED_ON;
    SS_OFF;

    return 0;
}

u8 mmcWrBlock(u32 mmc_addr, u8 *data_ptr) {

    u16 i;
    if (mmcCmd(CMD24, mmc_addr) != 0) {
        return 1;
    }

    SS_ON;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xfe;


    for (i = 0; i < 512; i++) {
        SPI_BUSY;
        SPI_PORT = *data_ptr++;
    }

    SPI_BUSY;
    SPI_PORT = 0xFF;
    SPI_BUSY;
    SPI_PORT = 0xFF;
    SPI_BUSY;
    SPI_PORT = 0xFF;
    SPI_BUSY;

    if ((SPI_PORT & 0x1f) != 0x05) {
        SS_OFF;
        return 2;
    }

    i = 0;
    SPI_BUSY;
    SPI_PORT = 0xFF;
    for (;;) {
        SPI_BUSY;
        SPI_PORT = 0xFF;
        SPI_BUSY;
        if ((SPI_PORT & 0xFF) == 0xff)break;
        if (i++ == 65535) {
            SS_OFF;
            return 3;
        }
    }
    SS_OFF;
    return 0;
}

u8 mmcRdBlock(u32 mmc_addr, u8 *stor) {

    u16 i = 0;
    stor_addr = (u32) stor;
    //u16 *stor16 = (u16 *) stor;
    //SPI_HI_SPEED_OFF;

    if (mmcCmd(CMD17, mmc_addr) != 0) {
        SS_ON;
        for (;;) {

            SPI_PORT = 0xff;
            SPI_BUSY;


            if ((SPI_PORT & 0xff) == 0) {
                break;
            }

            if (i++ == 65535) {
                SS_OFF;
                return 1;
            }
        }
    }

    SS_ON;


    i = 0;

    for (;;) {

        SPI_PORT = 0xff;
        SPI_BUSY;
        if ((SPI_PORT & 0xff) == 0xfe)break;

        if (i++ == 65535) {
            SS_OFF;
            return 2;
        }
    }

    CFGS(_SPI16);
    asm("movem.l %d0-%d1/%a0, -(%sp)");

    asm("move.l stor_addr, %a0");
    asm("move.l #512, %d0");

    asm("copy:");

    asm("move.w #0xffff, 0xA13000");

    asm("wait_spi:");
    asm("move.w 0xA13002, %d1");
    asm("btst #0, %d1");
    asm("beq.s  wait_spi");

    asm("move.w 0xA13000, (%a0)+");

    asm("subq.w  #2, %d0");
    asm("bne.s  copy");

    asm("movem.l (%sp)+, %d0-%d1/%a0");
    /*
    for (i = 0; i < 256; i++) {

        SPI_PORT = 0xffff;
        SPI_BUSY;
     *stor16++ = SPI_PORT;

    }
     * */
    CFGC(_SPI16);


    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SS_OFF;
    return 0;
}

u8 mmcReadOCR() {

    u16 i = 0;

    if (mmcFastCmd(CMD58, 0) != 0) {
        SS_ON;
        for (;;) {
            SPI_PORT = 0xff;
            SPI_BUSY;
            if ((SPI_PORT & 0xff) == 0) {
                break;
            }
            if (i++ == 65535) {
                SS_OFF;
                return 1;
            }
        }
    }

    SS_ON;

    for (i = 0; i < 4; i++) {
        SPI_PORT = 0xff;
        SPI_BUSY;
        ocr_data[i] = SPI_PORT;
    }

    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SS_OFF;
    return 0;
}

u8 mmcReadCSD() {

    u16 i = 0;

    if (mmcCmd(CMD9, 0) != 0) {
        SS_ON;
        for (;;) {
            SPI_PORT = 0xff;
            SPI_BUSY;
            if ((SPI_PORT & 0xff) == 0) {
                break;
            }
            if (i++ == 65535) {
                SS_OFF;
                return 1;
            }
        }
    }

    SS_ON;
    i = 0;

    for (;;) {
        SPI_PORT = 0xff;
        SPI_BUSY;
        if ((SPI_PORT & 0xff) == 0xfe)break;
        if (i++ == 65535) {
            SS_OFF;
            return 2;
        }
    }

    for (i = 0; i < 16; i++) {
        SPI_PORT = 0xff;
        SPI_BUSY;
        csd_data[i] = SPI_PORT;
    }

    SPI_PORT = 0xff;
    SPI_BUSY;
    SPI_PORT = 0xff;
    SPI_BUSY;
    SS_OFF;
    return 0;
}

u8 mmcGetDeviceSize(u32 *size) {

    u32 c_size;
    u32 bl_len;
    u32 c_size_mul;
    u8 resp;

    resp = mmcInit();
    if (resp != 0)return 1;
    resp = mmcReadCSD();
    if (resp != 0)return 2;

    c_size = (csd_data[6] << 16) | (csd_data[7] << 8) | (csd_data[8] << 0);
    c_size &= 0x3FFC0;
    c_size >>= 6;

    c_size_mul = (csd_data[9] << 8) | (csd_data[10] << 0);
    c_size_mul &= 0x380;
    c_size_mul >>= 7;
    bl_len = csd_data[5] & 0xf;

    *size = (c_size + 1) * deg(c_size_mul + 2) * deg(bl_len);
    return 0;
}

u32 deg(u32 d) {

    u8 i;
    u32 result = 1;

    for (i = 0; i < d; i++)result *= 2;


    return result;
}

u8 mmcInitRead(u32 mmc_addr) {

    u16 i;

    if (mmcCmd(CMD17, mmc_addr) != 0) {
        SS_ON;
        for (i = 0;;) {

            SPI_PORT = 0xFF;
            SPI_BUSY;
            if ((SPI_PORT & 0xff) == 0)break;
            if (i++ == 65535) {
                SS_OFF;
                return 1;
            }
        }
    }

    SS_ON;

    i = 0;
    for (;;) {
        SPI_PORT = 0xFF;
        SPI_BUSY;
        if ((SPI_PORT & 0xff) == 0xfe)break;
        if (i++ == 65535) {
            SS_OFF;
            return 2;
        }
    }

    return 0;
}

void mmcFinishRead() {

    SPI_BUSY;
    SPI_PORT = 0xFF;
    SPI_BUSY;
    SPI_PORT = 0xFF;
    SS_OFF;
}

void mmcOff() {

    u16 i = 0;

    for (i = 0; i < 65535; i++)asm("nop");
    mmcCmd(CMD0, 0);
    for (i = 0; i < 65535; i++)asm("nop");
    mmcCmd(CMD0, 0);
    for (i = 0; i < 65535; i++)asm("nop");
}
