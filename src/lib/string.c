/*
 * string.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "types.h"

u8 STR_intToDecString(u32 val, u8 *str) {

    int len;

    if (val < 10)len = 1;
    else
        if (val < 100)len = 2;
    else
        if (val < 1000)len = 3;
    else
        if (val < 10000)len = 4;
    else
        if (val < 100000)len = 5;
    else
        if (val < 1000000)len = 6;
    else
        if (val < 10000000)len = 7;
    else
        if (val < 100000000)len = 8;
    else
        if (val < 1000000000)len = 9;
    else len = 10;

    str += len;
    str[0] = 0;
    if (val == 0)*--str = '0';
    while (val) {

        *--str = '0' + val % 10;
        val /= 10;
    }


    return len;
}

void STR_intToDecStringMin(u32 val, u8 *str, u8 min_size) {

    int len;
    u8 i;

    if (val < 10)len = 1;
    else
        if (val < 100)len = 2;
    else
        if (val < 1000)len = 3;
    else
        if (val < 10000)len = 4;
    else
        if (val < 100000)len = 5;
    else
        if (val < 1000000)len = 6;
    else
        if (val < 10000000)len = 7;
    else
        if (val < 100000000)len = 8;
    else
        if (val < 1000000000)len = 9;
    else len = 10;

    if (len < min_size) {

        i = min_size - len;
        while (i--)str[i] = '0';
        len = min_size;
    }
    str += len;
    str[0] = 0;
    if (val == 0)*--str = '0';
    while (val) {

        *--str = '0' + val % 10;
        val /= 10;
    }
}

u8 STR_intToHexString(u32 val, u8 *str) {

    u8 len = 0;
    u32 val_copy = val;
    while (val_copy >>= 4)len++;

    len++;
    str += len;
    str[0] = 0;

    if (val == 0)*--str = '0';
    while (val) {

        if ((val & 0xF) < 0xa)*--str = (val & 0xF) + '0';
        else
            *--str = (val & 0xF) - 10 + 'A';
        val >>= 4;
    }

    return len;
}

void STR_intToHexStringMin(u32 val, u8 *str, u8 min_size) {

    u8 i;
    u8 len = 0;
    u32 val_copy = val;
    while (val_copy >>= 4)len++;

    len++;

    if (len < min_size) {

        i = min_size - len;
        while (i--)str[i] = '0';
        len = min_size;
    }

    str += len;
    str[0] = 0;

    if (val == 0)*--str = '0';
    while (val) {

        if ((val & 0xF) < 0xa)*--str = (val & 0xF) + '0';
        else
            *--str = (val & 0xF) - 10 + 'A';
        val >>= 4;
    }
}

char *strchr(const char *s, int c) {
	while (*s) {
		if (*s == (char)c) {
			return (char*)s;
		}
		s++;
	}

	return NULL;
}

u16 strlen(const char *str) {
    const char *src;

    src = str;
    while (*src++);

    return (src - str) - 1;
}

char *strcpy(char *to, const char *from) {
    const char *src;
    char *dst;

    src = from;
    dst = to;
    while ((*dst++ = *src++));

    return to;
}
