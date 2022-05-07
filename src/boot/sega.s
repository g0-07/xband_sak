.section .text.keepboot

*-------------------------------------------------------
*
*       Sega startup code for the GNU Assembler
*       Translated from:
*       Sega startup code for the Sozobon C compiler
*       Written by Paul W. Lee
*       Modified by Charles Coty
*       Modified by Stephane Dallongeville
*
*-------------------------------------------------------

    .globl  rom_header

    .org    0x00000000

_Start_Of_Rom:
_Vecteurs_68K:
        dc.l    __stack                 /* Stack address */
        dc.l    _Entry_Point            /* Program start address */
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT, _INT, _INT, _INT
        dc.l    _INT, _INT, _INT, _INT
        dc.l    _INT, _INT, _INT, _INT
        dc.l    _INT, _INT, _INT, _INT
        dc.l    _HBL
        dc.l    _INT
        dc.l    _VBL
        dc.l    _INT
        dc.l    _INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT

rom_header:
        .incbin "out/rom_head.bin", 0, 0x100

_Entry_Point:
        move    #0x2700,%sr
        tst.l   0xa10008
        bne.s   SkipJoyDetect

        tst.w   0xa1000c

SkipJoyDetect:
	*bne	SkipSetup

	lea	Table,%a5
	movem.w (%a5)+,%d5-%d7
	movem.l (%a5)+,%a0-%a4
* Check Version Number
	move.b	-0x10ff(%a1),%d0
	andi.b	#0x0f,%d0
	beq	WrongVersion
* Sega Security Code (SEGA)
	move.l	#0x53454741,0x2f00(%a1)
WrongVersion:
	move.w	(%a4),%d0
	moveq	#0x00,%d0
	movea.l %d0,%a6
	move	%a6,%usp

* Set VDP registers
	moveq	#0x17,%d1
FillLoop:
	move.b	(%a5)+,%d5
	move.w	%d5,(%a4)
	add.w	%d7,%d5
	dbra	%d1,FillLoop

	move.l	(%a5)+,(%a4)
	move.w	%d0,(%a3)
	move.w	%d7,(%a1)
	move.w	%d7,(%a2)
L0250:
	btst	%d0,(%a1)
	bne	L0250
* Put initial values into a00000
	moveq	#0x25,%d2
Filla:
	move.b	(%a5)+,(%a0)+
	dbra	%d2,Filla
	move.w	%d0,(%a2)
	move.w	%d0,(%a1)
	move.w	%d7,(%a2)
L0262:
	move.l	%d0,-(%a6)
	dbra	%d6,L0262
	move.l	(%a5)+,(%a4)
	move.l	(%a5)+,(%a4)
* Put initial values into c00000
	moveq	#0x1f,%d3
Filc0:
	move.l	%d0,(%a3)
	dbra	%d3,Filc0
	move.l	(%a5)+,(%a4)
* Put initial values into c00000
	moveq	#0x13,%d4
Fillc1:
	move.l	%d0,(%a3)
	dbra	%d4,Fillc1
* Put initial values into c00011
	moveq	#0x03,%d5
Fillc2:
	move.b	(%a5)+,0x0011(%a3)
	dbra	%d5,Fillc2
	move.w	%d0,(%a2)
	movem.l (%a6),%d0-%d7/%a0-%a6
	move	#0x2700,%sr
SkipSetup:
	bra	Continue
Table:
	dc.w	0x8000, 0x3fff, 0x0100, 0x00a0, 0x0000, 0x00a1, 0x1100, 0x00a1
	dc.w	0x1200, 0x00c0, 0x0000, 0x00c0, 0x0004, 0x0414, 0x302c, 0x0754
	dc.w	0x0000, 0x0000, 0x0000, 0x812b, 0x0001, 0x0100, 0x00ff, 0xff00
	dc.w	0x0080, 0x4000, 0x0080, 0xaf01, 0xd91f, 0x1127, 0x0021, 0x2600
	dc.w	0xf977, 0xedb0, 0xdde1, 0xfde1, 0xed47, 0xed4f, 0xd1e1, 0xf108
	dc.w	0xd9c1, 0xd1e1, 0xf1f9, 0xf3ed, 0x5636, 0xe9e9, 0x8104, 0x8f01
	dc.w	0xc000, 0x0000, 0x4000, 0x0010, 0x9fbf, 0xdfff

Continue:
	tst.w	 0x00C00004

* set stack pointer
	move.w	 #0,%a7

* user mode
	move.w	#0x2300,%sr

* clear Genesis RAM
	lea	0xff0000,%a0
	moveq	#0,%d0
clrram:
        move.w	#0,(%a0)+
	subq.w	#2,%d0
	bne	clrram
        jmp	main

*------------------------------------------------
*
*	interrupt functions
*
*------------------------------------------------


_HBL:
	movem.l %d0-%d1/%a0-%a1,-(%sp)
	jsr hb
	movem.l (%sp)+,%d0-%d1/%a0-%a1
	rte

_VBL:
	movem.l %d0-%d1/%a0-%a1,-(%sp)
        jsr vb
	movem.l (%sp)+,%d0-%d1/%a0-%a1
	rte

_INT:
	movem.l %d0-%d1/%a0-%a1,-(%sp)
	jsr in
	movem.l (%sp)+,%d0-%d1/%a0-%a1
	rte
