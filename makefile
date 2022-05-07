MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(subst \,/,$(MAKEFILE_DIR))

ifneq ("$(wildcard $(MAKEFILE_DIR)bin/rescomp.jar)","")
    GDK := $(patsubst %/,%,$(MAKEFILE_DIR))
endif

include $(GDK)/common.mk

SRC := src
RES := res
INCLUDE := inc

SRC_C= $(wildcard *.c)
SRC_C+= $(wildcard $(SRC)/*.c)
SRC_C+= $(wildcard $(SRC)/*/*.c)
SRC_C+= $(wildcard $(SRC)/*/*/*.c)
SRC_C:= $(filter-out $(SRC)/boot/rom_head.c,$(SRC_C))
SRC_S= $(wildcard *.s)
SRC_S+= $(wildcard $(SRC)/*.s)
SRC_S+= $(wildcard $(SRC)/*/*.s)
SRC_S+= $(wildcard $(SRC)/*/*/*.s)
SRC_S:= $(filter-out $(SRC)/boot/sega.s,$(SRC_S))
SRC_S:= $(filter-out $(SRC)/boot/sega_ram.s,$(SRC_S))
SRC_ASM= $(wildcard *.asm)
SRC_ASM+= $(wildcard $(SRC)/*.asm)
SRC_ASM+= $(wildcard $(SRC)/*/*.asm)
SRC_ASM+= $(wildcard $(SRC)/*/*/*.asm)
SRC_S80= $(wildcard *.s80)
SRC_S80+= $(wildcard $(SRC)/*.s80)
SRC_S80+= $(wildcard $(SRC)/*/*.s80)
SRC_S80+= $(wildcard $(SRC)/*/*/*.s80)

RES_C= $(wildcard $(RES)/*.c)
RES_S= $(wildcard $(RES)/*.s)
RES_RES= $(wildcard *.res)
RES_RES+= $(wildcard $(RES)/*.res)

RES_RS= $(RES_RES:.res=.rs)
RES_H= $(RES_RES:.res=.h)
RES_DEP= $(RES_RES:.res=.d)
RES_DEPS= $(addprefix out/, $(RES_DEP))

OBJ= $(RES_RES:.res=.o)
OBJ+= $(RES_S:.s=.o)
OBJ+= $(RES_C:.c=.o)
OBJ+= $(SRC_S80:.s80=.o)
OBJ+= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_S:.s=.o)
OBJ+= $(SRC_C:.c=.o)
OBJS:= $(addprefix out/, $(OBJ))

DEPS:= $(OBJS:.o=.d)

LST:= $(SRC_C:.c=.lst)
LSTS:= $(addprefix out/, $(LST))

INCS:= -I$(INCLUDE) -I$(SRC) -I$(RES) -I$(RES_LIB)
DEFAULT_FLAGS= $(EXTRA_FLAGS) -DSGDK_GCC -m68000 -Wall -Wextra -Wno-shift-negative-value -Wno-unused-value -Wno-main -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-pointer-sign -fno-builtin $(INCS) -B$(BIN)
FLAGSZ80:= -i$(SRC) -i$(INCLUDE) -i$(RES) -i$(SRC_LIB)

release: FLAGS= $(DEFAULT_FLAGS) -Os -fuse-linker-plugin -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -flto -ffunction-sections -fdata-sections
release: pre-build out/rom.bin out/ram.bin out/symbol.txt out/symbol_ram.txt

debug: FLAGS= $(DEFAULT_FLAGS) -O1 -ggdb -DDEBUG=1
debug: pre-build out/rom.bin out/ram.bin out/rom.out out/ram.out out/symbol.txt out/symbol_ram.txt

asm: FLAGS= $(DEFAULT_FLAGS) -O1 -fuse-linker-plugin -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -S
asm: pre-build $(LSTS)


all: release
default: release

Default: release
Debug: debug
Release: release
Asm: asm

.PHONY: clean

cleantmp:
	$(RM) -f $(RES_RS)

cleandep:
	$(RM) -f $(DEPS)

cleanlst:
	$(RM) -f $(LSTS)

cleanres: cleantmp
	$(RM) -f $(RES_H) $(RES_DEP) $(RES_DEPS)

cleanobj:
	$(RM) -f $(OBJS) out/sega.o out/rom_head.bin out/rom_head.o out/sega_ram.o out/rom.out out/ram.out

clean: cleanobj cleanres cleanlst cleandep
	$(RM) -f out.lst out/cmd_ out/symbol.txt out/cmd_ out/symbol_ram.txt out/rom.nm out/rom.wch out/rom.bin out/ram.nm out/ram.wch out/ram.bin

cleanrelease: clean

cleandebug: clean

cleanasm: cleanlst

cleandefault: clean
cleanDefault: clean

cleanRelease: cleanrelease
cleanDebug: cleandebug
cleanAsm: cleanasm

pre-build:
	$(MKDIR) -p $(SRC)/boot
	$(MKDIR) -p out


out/rom.bin: out/rom.out
	$(OBJCPY) -O binary out/rom.out out/rom.bin
	$(SIZEBND) out/rom.bin -sizealign 131072 -checksum

out/ram.bin: out/ram.out
	$(OBJCPY) -O binary out/ram.out out/ram.bin

out/symbol.txt: out/rom.out out/ram.out
	$(NM) $(LTO_PLUGIN) -n out/rom.out > out/symbol.txt
	$(NM) $(LTO_PLUGIN) -n out/ram.out > out/symbol_ram.txt

out/rom.out: out/sega.o out/cmd_
	$(CC) -B$(BIN) -n -T $(GDK)/md.ld -nostdlib out/sega.o @out/cmd_ $(LIBGCC) -o out/rom.out -Wl,--gc-sections
#	$(RM) out/cmd_

out/ram.out: out/sega_ram.o out/cmd_
	$(CC) -B$(BIN) -T ram.ld -nostdlib out/sega_ram.o @out/cmd_ $(LIBGCC) -o out/ram.out
	$(RM) out/cmd_

out/cmd_: $(OBJS)
	$(ECHO) "$(OBJS)" > out/cmd_

out/sega.o: $(SRC)/boot/sega.s out/rom_head.bin
	$(CC) -x assembler-with-cpp $(DEFAULT_FLAGS) -c $(SRC)/boot/sega.s -o $@

out/sega_ram.o: $(SRC)/boot/sega_ram.s
	$(CC) -x assembler-with-cpp $(DEFAULT_FLAGS) -c $(SRC)/boot/sega_ram.s -o $@

out/rom_head.bin: out/rom_head.o
	$(OBJCPY) -O binary $< $@

out/rom_head.o: $(SRC)/boot/rom_head.c
	$(CC) $(DEFAULT_FLAGS) -c $< -o $@

$(SRC)/boot/sega.s: $(SRC_LIB)/boot/sega.s
	$(CP) $< $@

$(SRC)/boot/rom_head.c: $(SRC_LIB)/boot/rom_head.c
	$(CP) $< $@

out/%.lst: %.c
	$(MKDIR) -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

out/%.o: %.c
	$(MKDIR) -p $(dir $@)
	$(CC) $(FLAGS) -MMD -c $< -o $@

out/%.o: %.s
	$(MKDIR) -p $(dir $@)
	$(CC) -x assembler-with-cpp $(FLAGS) -MMD -c $< -o $@

out/%.o: %.rs
	$(MKDIR) -p $(dir $@)
	$(CC) -x assembler-with-cpp $(FLAGS) -c $*.rs -o $@
	$(CP) $*.d out/$*.d
	$(RM) $*.d

%.rs: %.res
	$(RESCOMP) $*.res $*.rs -dep out/$*.o

%.s: %.asm
	$(MACCER) -o $@ $<

%.o80: %.s80
	$(ASMZ80) $(FLAGSZ80) $< $@ out.lst

%.s: %.o80
	$(BINTOS) $<