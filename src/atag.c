/* src/atag.c
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
 *      Moved atag stuff from main.c into this file
 */
#include <string.h>

#include "gp32.h"       /* Mr. Mirko's SDK header */
#include "registers.h"  /* GP32 registers */
#include "atag.h"       /* Linux<->bootloader conversation structs */


/* tag support */
void
setup_core_tag(void *mem_addr, unsigned long pagesize)
{
	tag_array = (struct tag *) mem_addr;

	tag_array->hdr.tag = ATAG_CORE;
	tag_array->hdr.size = tag_size(tag_core);
	
	tag_array->u.core.flags = 1;		/* bit 0 = read-only */
	tag_array->u.core.pagesize = pagesize;
	tag_array->u.core.rootdev = 0;

	tag_array = tag_next(tag_array);
}

void
setup_mem_tag(unsigned long start, unsigned long len)
{
	tag_array->hdr.tag = ATAG_MEM;
	tag_array->hdr.size = tag_size(tag_mem32);

	tag_array->u.mem.start = start;
	tag_array->u.mem.size = len;
	
	tag_array = tag_next(tag_array);
}

void
setup_initrd2_tag(unsigned long start, unsigned long len)
{
	tag_array->hdr.tag = ATAG_INITRD2;
	tag_array->hdr.size = tag_size(tag_initrd);

	tag_array->u.initrd.start = start;
	tag_array->u.initrd.size = len;
	
	tag_array = tag_next(tag_array);
}

void
setup_cmdline_tag(const char *cmdline)
{
	int len = strlen(cmdline);
	
	tag_array->hdr.tag = ATAG_CMDLINE;
	tag_array->hdr.size = (sizeof(struct tag_header) + len + 1 + 4) >> 2;

	strcpy(tag_array->u.cmdline.cmdline, cmdline);
	
	tag_array = tag_next(tag_array);
}

void
setup_end_tag()
{
	tag_array->hdr.tag = ATAG_NONE;
	tag_array->hdr.size = 0;
}

void
setup_tags()
{
	setup_core_tag((void *) TAG_ADDR, 4096);	/* core tag 4k pagesize */
	setup_mem_tag(MEMORY_ADDR, 0x2000);		/* 8MB of RAM */
	setup_initrd2_tag(INITRD_LOADADDR, 0x400000);	/* 4MB of compressed data */
	setup_cmdline_tag("root=/dev/ram0 console=ttySAC0 console=tty0");
	setup_end_tag();
}
