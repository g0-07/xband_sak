.text
.org 0
jmp ramload

.org 0x08
jsr vb
rts

.org 0x10
jsr hb
rts

.org 0x18
jsr in
rts

.org 0x20
movem.l %d0-%d1/%a0-%a1,-(%sp)
jsr vb
movem.l (%sp)+,%d0-%d1/%a0-%a1
rte

.org 0x30
rte

.org 0x100
.ascii	"SEGA EVERDRIVE 3"
.ascii	"xband_sak"

.org 0x1da
jmp main

*app address must be set on a0
ramload:
    move.l #0, %sp
    cmp.l #0xff0000, %a0
    beq main
    move.w #0, %d0
    move.l #0xff0000, %a1
load_loop:
    move.w (%a0)+, (%a1)+
    subq.w #2,%d0
    bne	load_loop
    move.l #0xff0000, %a0
    jmp (%a0)