/* src/main.c
 * 
 * Copyright (c) 2005 Lucas Correia Villa Real <lucasvr@gobolinux.org>
 *
 * http://sourceforge.net/projects/gp32linux
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * Changelog:
 *   12-Apr-2005  lucasvr
 *      Added atag support, code cleanup. 
 *                  
 *   15-Apr-2005  lucasvr
 *      Disabling interrupts, mmu and {i,d}cache registers;
 *      Configuring r0, r1 and r2 registers before setting the PC.
 *
 *   02-May-2005  lucasvr
 *      Added UART initialization code - defaults to 8n1 at 9600 bauds;
 *      Replaced disable_irq() asm operations by C code.
 *                  
 *   07-Jul-2005  lucasvr
 *      Written get_pll_value() and send_uart();
 *      Rewrote init_uart() calls.
 *
 *   08-Jul-2005  lucasvr
 *      Reseting the UART FIFO prior to disabling it.
 *
 *   12-Jul-2005  lucasvr
 *      Fixed the UART bug;
 *      Moved print() to the new function uart_printf().
 *                  
 *   16-Jul-2005  lucasvr
 *      Made the CPU speed selectable at boot time through
 *      a graphical menu.
 *                  
 *   19-Jul-2005  lucasvr
 *      Moved away from GamePark's proprietary SDK in favour to 
 *      Mr. Mirko's GPL'ed SDK -- and hey, it's *GREAT*!
 *
 *   17-Jan-2006  lucasvr
 *      Disabling the MMU before jumping to zImage
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "gp32.h"       /* Mr. Mirko's SDK header */
#include "fileio.h"     /* Mr. Mirko's SDK file I/O header */

#include "registers.h"  /* GP32 registers */
#include "graphics.h"   /* graphical bootloader stuff */
#include "clock.h"      /* clock manipulation functions */
#include "uart.h"       /* UART manipulation functions */
#include "atag.h"       /* Linux<->bootloader conversation structs */

#define BOOTLDR_VERSION "2006.01.17"


void
suicide()
{
	while(2);
}

void
start_kernel()
{
	unsigned long machine_type = GP32_MACH_NUMBER;
	unsigned long tag_address = TAG_ADDR;
	void (*arm_boot)(int zero, int arch, unsigned long params);
	
	/*
	 * r0 must be 0, while r1 must be the ARM Linux machine type and
	 * r2 must hold the parameter list address 
	 */
	arm_boot = (void (*)(int, int, unsigned long)) ZTEXTADDR;
	arm_boot(0, machine_type, tag_address);
}

void 
load_file(char *filename, unsigned long mem_addr)
{
	GPFILE *fp;
	long file_len, bytes_read;
	long n = 256, offset = 0;
	unsigned char *memptr = (char *) mem_addr;
	
	/* skip the 'dev0:\\gpmm\\linux\\' part */
	uart_printf("Loading %s...", filename+17);
	
	/* try to open 'filename' */
	fp = smc_fopen(filename, "r");
	if (! fp) {
		uart_printf("Loading %s...failed!\n", filename+17);
		suicide();
	}

	/* get file length */
	smc_fseek(fp, 0, SEEK_END);
	file_len = smc_ftell(fp);
	smc_rewind(fp);
	
	/* read it and copy to physical memory 'mem_addr' */
	while (offset < file_len) {
		if (offset + n > file_len)
			n = file_len - n;

		bytes_read = smc_fread(memptr + offset, n, sizeof(char), fp);
		if (bytes_read == 0) {
		  	uart_printf("Reading %s...failed!\n", filename+17);
			suicide();
		}
		offset += bytes_read;
	}
	uart_printf("Loading %s...ok \n", filename+17);
}

int
main(void) 
{
	int base_clock;

	/* just initialize the cpu speed to the standard 66MHz */
  	gp_setCpuspeed(66);
	
	/* configure the LCD and initialize the joystick and SMC filesystem */
	lcd_init(1);
	lcd_splashscreen(1);

	/* load the zImage and initrd */
	load_file("dev0:\\GPMM\\LINUX\\ZIMAGE26", ZTEXTADDR);
//	load_file("dev0:\\GPMM\\LINUX\\INITRD", INITRD_LOADADDR);
	
	/* get the base clock from a user-friendly menu */
	base_clock = lcd_bootloader_menu();
	
	/* initialize the UART and the base clock to the specified frequency */
//	gp_setCpuspeed(base_clock);
	init_clocks(base_clock);
	init_uart(base_clock);

	uart_printf("GP32 Linux bootloader version %s\n", BOOTLDR_VERSION);
	uart_printf("See http://sourceforge.net/projects/gp32linux for project information\n");	
	uart_printf("Clock speed set to %dMHz\n", base_clock);
	uart_printf("Time to cross your fingers!\n\n");

	/* voila`! */
	setup_tags();

	/* disable the MMU, instruction/data caches and IRQs */
	__asm__(
			"bic r0, r0, #CR_M    @ disable MMU\n"
			"bic r0, r0, #CR_C    @ disable data cache\n"
			"bic r0, r0, #CR_I    @ disable instruction cache\n"
			:
			:
			: "r0"
	);
	
	set_register(IRQ_INTMSK, 0xFFFFFFFF);

	start_kernel();
	return 0;
}
