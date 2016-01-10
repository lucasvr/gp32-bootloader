/* src/clock.c
 *
 * Copyright (c) 2005 Lucas Correia Villa Real <lucasvr@gobolinux.org>
 *
 * http://sourceforge.net/projects/gp32linux
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 *
 * Changelog:
 *   20-Jul-2005  lucasvr
 *      Moved clock code from main.c into this file
 */
#include <stdio.h>

#include "gp32.h"       /* Mr. Mirko's SDK header */
#include "registers.h"  /* GP32 registers */

unsigned long
get_pll_value(int freq)
{
	/*
	 * Fin  = always 12Mhz
	 * Fout = m * Fin / p * s, where m = MDIV+8, p = PDIV+2, s = 2^SDIV
	 *
	 * Note: Fout == Mpll (MPLL output)
	 */
	switch(freq) {
		case 22:
			return PLLCON_MDIV(37) | PLLCON_PDIV(1) | PLLCON_SDIV(3);
		case 33:
			return PLLCON_MDIV(37) | PLLCON_PDIV(0) | PLLCON_SDIV(3);
		case 44:
			return PLLCON_MDIV(51) | PLLCON_PDIV(2) | PLLCON_SDIV(2);
		case 48:
			return PLLCON_MDIV(40) | PLLCON_PDIV(1) | PLLCON_SDIV(2);
		case 51:
			return PLLCON_MDIV(43) | PLLCON_PDIV(1) | PLLCON_SDIV(2);
		case 56:
			return PLLCON_MDIV(48) | PLLCON_PDIV(1) | PLLCON_SDIV(2);
		case 67:
			return PLLCON_MDIV(37) | PLLCON_PDIV(0) | PLLCON_SDIV(2);
		case 79:
			return PLLCON_MDIV(71) | PLLCON_PDIV(1) | PLLCON_SDIV(2);
		case 84:
			return PLLCON_MDIV(34) | PLLCON_PDIV(1) | PLLCON_SDIV(1);
		case 90:
			return PLLCON_MDIV(37) | PLLCON_PDIV(1) | PLLCON_SDIV(1);
		case 102:
			return PLLCON_MDIV(43) | PLLCON_PDIV(1) | PLLCON_SDIV(1);
		case 112:
			return PLLCON_MDIV(67) | PLLCON_PDIV(2) | PLLCON_SDIV(1);
		case 118:
			return PLLCON_MDIV(71) | PLLCON_PDIV(2) | PLLCON_SDIV(1);
		case 124:
			return PLLCON_MDIV(54) | PLLCON_PDIV(1) | PLLCON_SDIV(1);
		case 133:
			return PLLCON_MDIV(81) | PLLCON_PDIV(2) | PLLCON_SDIV(1);
		default:
			/* defaults to reset state = 57MHz (could be 120MHz) */
			return PLLCON_MDIV(48) | PLLCON_PDIV(1) | PLLCON_SDIV(2);
	}
}

void
init_clocks(int base_clock)
{
	unsigned long value;

	/*
	 * Observations:
	 *
	 * MCLK is the main clock for the CPU  (166MHz max)
	 * HCLK is the clock for the system bus (SDRAM, LCD) (133MHz max)
	 * PCLK is the clock for the peripherals (66MHz max)
	 */

	/* configure the divide ratio (FCLK:HCLK:PCLK) */
	value = CLKDIVN_1_1_1;
	set_register(CLKDIVN, value);

	/* 
	 * since the LCD VCLK is touched by this change, modify it as well
	 * VCLK(Hz) = HCLK/(CLKVAL*2)
	 */
	value = get_register(LCDCON1) | LCDCON1_CLKVAL(2);
	set_register(LCDCON1, value);

	/* and also do that for the SMC controller */
	value = get_register(MISCCR) & ~MISCCR_CLKSEL_MASK;
	value |= MISCCR_CLKSEL_PCLK;
	set_register(MISCCR, value);

	/* adjust the LOCKTIME register to reduce PLL lock time */
//	value = 0xFFFFFF;
	value = 0x7D07D0;	// accordingly to the GP32 BIOS dump
	set_register(LOCKTIME, value);

	/* initialize the base clock */
	value = get_pll_value(base_clock);
	set_register(MPLLCON, value);
}
