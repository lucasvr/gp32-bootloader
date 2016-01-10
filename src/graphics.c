/* src/graphics.c
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
 *   16-Jul-2005  lucasvr
 *      Created this file.
 *
 *   19-Jul-2005  Lucas Correia Villa Real
 *      Moved away from GamePark proprietary SDK in favour to 
 *      Mr. Mirko's GPL'ed SDK.
 */
#include <stdio.h>
#include <string.h>
#include "gp32.h"
#include "graphics.h"

static unsigned short *framebuffer[2];
static int backflip = 1;

struct clkspeed {
	char *name;
	int   freq;
};

static struct clkspeed clkspeed_array[] = {
	{ "133 MHz", 133 },
	{ "112 MHz", 112 },
	{ " 90 MHz",  90 },
	{ " 79 MHz",  79 },
	{ " 67 MHz",  67 },
	{ " 56 MHz",  56 },
	{ " 48 MHz",  48 },
	{      NULL,   0 }
};

inline int
new_highlight(int new_val, int min, int max)
{
	if (new_val > max)
		return max;
	else if (new_val < min)
		return min;
	return new_val;
}

inline void
lcd_flip_buffers(void)
{
	gp_setFramebuffer(framebuffer[backflip++], 1);
	backflip %= 2;	
}

inline void
lcd_clrscr(int surface)
{
	/* surface fill color in white */
	gp_clearFramebuffer16(framebuffer[surface], 0xFFFF);
}

inline void
lcd_printf(char *buf, int wherex, int wherey, unsigned short color, int flip)
{
	/* output text in black */
	gp_drawString(wherex, wherey, strlen(buf), buf, color, framebuffer[backflip]);

	if (flip)
		lcd_flip_buffers();
}

inline void
lcd_printf_front(char *buf, int wherex, int wherey, unsigned short color)
{
	/* output text in black */
	gp_drawString(wherex, wherey, strlen(buf), buf, color, framebuffer[!backflip]);
}

void
lcd_init(int clear_screen)
{
	/* configures the double buffering */
	framebuffer[0] = (unsigned short *) FRAMEBUFFER;
	framebuffer[1] = (unsigned short *) (FRAMEBUFFER + WIDTH*HEIGHT*DEPTH);
	
	/*
	 * initializes with 16bpp at a screen rate of 85Hz
	 * (only need to do this for the primary buffer) 
	 */
	gp_initFramebuffer(framebuffer[0], 16, 85);

	if (clear_screen)
		lcd_clrscr(!backflip);

	gp_setFramebuffer(framebuffer[0], 1);
}

void
lcd_splashscreen(int flip)
{
	extern unsigned char background[];
	
	/* draw on the back surface at the origin (0,0) */
	gp_clearFramebuffer16(framebuffer[backflip], 0xFFFF);
	gp_drawSpriteH((unsigned short *)background, 0, 0, framebuffer[backflip]);
	
	if (flip)
		lcd_flip_buffers();
}

void
lcd_draw_box(int x_start, int y_start, int x_len, int y_len, unsigned short color, int screen)
{
	int i;

	for (i = y_start; i < y_start + y_len; ++i) {
		gp_drawLine16(x_start,         i, 
			      x_start + x_len, i,
			      color, framebuffer[screen]);
	}
}

void
lcd_draw_menu(int highlight_index, int print_entries)
{
	int i;
	int x_len = 320/5*3;
	int y_len = 240/5*3 - 10;
	int x_start = 320/5;
	int y_start = 240/5 * 2 + 40;
	
	/* keep using the splashscreen, but don't flip the buffers yet */
	lcd_splashscreen(0);
	
	/* draw the box itself */
	lcd_draw_box(x_start, y_start - 10, x_len, y_len, 0xE6C0, backflip);
	
	if (! print_entries) {
		lcd_flip_buffers();
		return;
	}
	
	/* draw the hightlighted entry */
	lcd_draw_box(x_start, y_start - 3 + (14 * highlight_index), x_len, 13, 0x0000, backflip);
	
	/* print the entries */
	for (i = 0; clkspeed_array[i].name != NULL; ++i) {
		if (i == highlight_index) {
			/* font has another color */
			lcd_printf(clkspeed_array[i].name, x_start + 320/5*2, y_start, 0xFFFF, 0);
		} else {
			lcd_printf(clkspeed_array[i].name, x_start + 320/5*2, y_start, 0x0000, 0);
		}
		y_start += 14;
	}
	
	lcd_flip_buffers();
}

int
lcd_bootloader_menu(void)
{
	int index_max, freq_i = 0;
	unsigned char key;	

	index_max = sizeof(clkspeed_array)/sizeof(struct clkspeed) - 2;
	lcd_draw_menu(freq_i, 1);
	
	/* look for joystick events */
	while (2) {
		key = gp_getButton();

		if (key & BUTTON_UP) {
			freq_i = new_highlight(freq_i-1, 0, index_max);
			lcd_draw_menu(freq_i, 1);
			
		} else if (key & BUTTON_DOWN) {
			freq_i = new_highlight(freq_i+1, 0, index_max);
			lcd_draw_menu(freq_i, 1);
			
		} else if (key & BUTTON_SELECT || key & BUTTON_START  ||
			   key & BUTTON_A      || key & BUTTON_B) {
			lcd_draw_menu(freq_i, 0);
			return clkspeed_array[freq_i].freq;
		}
	}
#if 0
	gp_initRTC();
	while (gp_getRTC() < TICKS_IN_A_SEC)
		/* busy wait */
	gp_clearRTC();
#endif
}
