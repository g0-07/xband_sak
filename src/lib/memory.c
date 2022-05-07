/*
 * memory.c
 *
 * Part of xband_sak written by g007
 * Based on work from Krikzz & Stephane-D
 */

#include "memory.h"

static u16* free;
static u16* heap;

static u16* pack(u16 nsize);
extern void memset_(void *to, u8 value, u16 len);
extern void memcpy_(void *to, const void *from, u16 len);

void __attribute__ ((noinline, used)) memset(void *to, u8 value, u16 len) {

    memset_(to, value, len);
}

void __attribute__ ((noinline, used)) memcpy(void *to, const void *from, u16 len) {

    memcpy_(to, from, len);
}

void memcpyU16(u16 *to, const u16 *from, u16 len) {

    memcpy(to, from, len * 2);
}

void memcpyU32(u32 *to, const u32 *from, u16 len) {

    memcpy(to, from, len * 4);
}

void *MEM_alloc(u16 size) {

    u16* p;
    u16 adjsize;
    u16 remaining;

    if (size == 0)
        return 0;

    // 2 bytes aligned
    adjsize = (size + sizeof(u16) + 1) & 0xFFFE;

    if (adjsize > *free)
    {
        p = pack(adjsize);

        // no enough memory
        if (p == NULL)
        {
            return NULL;
        }

        free = p;
    }
    else
        // at this point we can allocate memory
        p = free;

    // set free to next free block
    free += adjsize >> 1;

    // get remaining (old - allocated)
    remaining = *p - adjsize;
    // adjust remaining free space
    if (remaining > 0) *free = remaining;
    else
    {
        // no more space in bloc so we have to find the next free bloc
        u16 *newfree = free;
        u16 bloc;

        while((bloc = *newfree) & USED)
            newfree += bloc >> 1;

        free = newfree;
    }

    // set block size, mark as used and point to free region
    *p++ = adjsize | USED;

    // return block
    return p;
}

void MEM_free(void *ptr) {

    // valid block ?
    if (ptr)
    {
        // mark block as no more used
        ((u16*)ptr)[-1] &= ~USED;
    }
}

static u16* pack(u16 nsize) {

    u16 *b;
    u16 *best;
    u16 bsize, psize;

    b = heap;
    best = b;
    bsize = 0;

    while ((psize = *b))
    {
        if (psize & USED)
        {
            if (bsize != 0)
            {
                // store packed free size
                *best = bsize;

                // return it if greater than what we're looking for
                if (bsize >= nsize)
                    return best;

                // reset packed free size
                bsize = 0;
            }

            // point to next memory block
            b += psize >> 1;
            // remember it in case it becomes free
            best = b;
        }
        else
        {
            // increment free size
            bsize += psize;
            // clear this memory block as it will be packed
            *b = 0;
            // point to next memory block
            b += psize >> 1;
        }
    }

    // last free block update
    if (bsize != 0)
    {
        // store packed free size
        *best = bsize;

        // return it if greater than what we're looking for
        if (bsize >= nsize)
            return best;
    }

    return NULL;
}

int8_t memcmp(const char *m1, const char *m2, int16_t len) {

	int8_t dif = 0;
	for (int16_t i = 0; i < len && !dif; i++) {
		dif = m1[i] - m2[i];
	}

	return dif;
}
