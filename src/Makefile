# gp32linux boot Makefile

KERNDIR    = /Depot/Sources/Linux/GP32/Current-lucasvr

# chose the right compiler prefix
CROSS      = arm-elf-
CC         = $(CROSS)gcc
LD         = $(CROSS)gcc
AS         = $(CROSS)as
AR         = $(CROSS)ar
OBJCOPY    = $(CROSS)objcopy

CRT0       = ../lib/crt0.S
B2FXE      = ../bin/b2fxec
BMP2BIN    = ../bin/bmp2bin
CONVERT    = ../bin/convert
LNKSCRIPT  = ../lib/lnkscript
SED        = sed

CFLAGS     = -I../include -I$(KERNDIR)/include -Wall -O2 -s -mtune=arm9tdmi
LDFLAGS    = -L../lib -lmirkoSDK -lm

OUTFILE    = boot
BACKGROUND = background
OBJS       = atag.o clock.o graphics.o main.o uart.o $(BACKGROUND).o

all: $(OBJS)
	$(CC) -c -o crt0.o $(CRT0)
	$(LD) -nostartfiles -s -Wall -Wl,-Map,Test.map -T $(LNKSCRIPT) \
	      crt0.o -o $(OUTFILE).elf $(OBJS) $(LDFLAGS)
	$(OBJCOPY) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(B2FXE) -a "paranoidd" -t "GP32 Linux Bootloader" $(OUTFILE).bin $(OUTFILE).fxe

$(BACKGROUND).o: background/$(BACKGROUND).bmp
	$(BMP2BIN) -x background/$(BACKGROUND).bmp background/$(BACKGROUND).raw
	$(CONVERT) background/$(BACKGROUND).raw > background/$(BACKGROUND).tmp
	$(SED) "s%wasteland%$(BACKGROUND)%g" background/$(BACKGROUND).tmp > $(BACKGROUND).c
	$(CC) -c $(BACKGROUND).c $(CFLAGS)
	@rm background/$(BACKGROUND).raw background/$(BACKGROUND).tmp

install:
	gplink put $(OUTFILE).fxe gpmm
	gplink put $(OUTFILE).fxe /
	gplink end

clean:
	rm -f *.o *~ *.elf *.bin *.fxe *.map
