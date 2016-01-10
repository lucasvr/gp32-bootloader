/* src/uart.c
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
 *      Moved UART code from main.c into this file
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "registers.h"  /* GP32 registers */
#include "graphics.h"   /* graphical bootloader stuff */
#include "uart.h"       /* function prototypes to this file */
#include "gp32.h"       /* Mr. Mirko's SDK header */

static int uart_initialized;

#define uart_empty_buffer(x)  do { while ((get_register(UTRSTAT(x)) & 0x2) == 0); } while(0);

void
send_uart(unsigned long uart, char *string)
{
	int i, count = strlen(string);
	unsigned long value;
	
	/* disable TX interrupts */
	value = get_register(IRQ_INTMSK);
	value |= (IRQ_INTMSK_UTXD0 | IRQ_INTMSK_UTXD1);
	set_register(IRQ_INTMSK, value);

	for (i = 0; i < count; ++i) {
		/* expand newline to \n\r */
		if (string[i] == '\n') {
			uart_empty_buffer(uart);
			set_register(UTXH(uart), '\r');
		}
		
		/* wait until the transmit buffer is empty */
		uart_empty_buffer(uart);
		set_register(UTXH(uart), string[i]);
	}
	
	/* enable TX interrupts */
	value &= ~(IRQ_INTMSK_UTXD0 | IRQ_INTMSK_UTXD1);
	set_register(IRQ_INTMSK, value);
}

int
has_newline(char *buf, int len)
{
	int i;
	for (i = len-1; i >= 0; --i)
		if (buf[i] == '\n')
			return 1;
	return 0;
}

void
uart_printf(const char *format, ...)
{
	static int lcd_wherex = 320/5 + 7;
	static int lcd_wherey = 240/5 * 2 + 54;
	char buf[1024];
	va_list args;
	
	/* send the data to the uart */
	va_start(args, format);
	memset(buf, 0, sizeof(buf));
	vsnprintf(buf, sizeof(buf)-1, format, args);
	va_end(args);

	if (uart_initialized)
		send_uart(DEBUG_UART, buf);
	
	/* send the data to the GP32 LCD */
	lcd_printf_front(buf, lcd_wherex, lcd_wherey, 0x0000);
	
	if (has_newline(buf, strlen(buf))) {
		/* reset x position and increment line */
		lcd_wherey += 14;
	}
}

void
init_uart(int base_clock)
{
	unsigned long value;
	
	/* configures the PFCON register, which is responsible by the UART settings */
	value = PFCON_RXD0 | PFCON_RXD1 | PFCON_TXD0 | PFCON_TXD1 | PFCON_nRTS0 | PFCON_nCTS0;
	set_register(PFCON, value);
	
	/* disable pull-up on PF0..PF6 */
	value = 0x3F;
	set_register(PFUP, value);
	
	/* reset and disable the FIFO */
//	value = UFCON_TX_FIFO_RST | UFCON_RX_FIFO_RST | UFCON_FIFO_ENABLE | UFCON_TX_TRIG_EMPTY;
	value = UFCON_TX_FIFO_RST | UFCON_RX_FIFO_RST;
	set_register(UFCON(DEBUG_UART), value);
	
	/* no auto flow control */
	set_register(UMCON(DEBUG_UART), 0x0);

	/* 8n1: 8 bit word length, 1 stop bit, no parity, normal mode for operation */
	value = ULCON_NORMAL | ULCON_NOPAR | ULCON_STOP1 | ULCON_WORD8;
	set_register(ULCON(DEBUG_UART), value);
	
	/* TX/RX transmit mode by interrupt request */
	value = UCON_RXIRQ | UCON_TXIRQ | UCON_RXERREN | UCON_TXLEVEL;
	set_register(UCON(DEBUG_UART), value);
	
	/* 
	 * set the UART to 9600 bauds
	 * UBRDIV = (int)(PCLK / (bps * 16)) - 1
	 */
	value = (int) ((base_clock * 1000000) / (9600 * 16)) - 1;
	set_register(UBRDIV(DEBUG_UART), value);

	/* enable the clock for both UARTs */
	value = CLKCON_UART0 | CLKCON_UART1;
	set_register(CLKCON, value);

	/* enable TX interrupts */
	value = get_register(IRQ_INTMSK) | IRQ_INTMSK_UTXD0 | IRQ_INTMSK_UTXD1;
	set_register(IRQ_INTMSK, value);

	uart_initialized = 1;
}
