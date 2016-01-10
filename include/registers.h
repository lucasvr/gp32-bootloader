/*
 * GP32 register definitions
 * 
 * Changelog
 *   19-Jul-2005  lucasvr
 *      First version
 *
 *   17-Jan-2006  lucasvr
 *      Added CP15 bits
 */
#ifndef REGISTERS_H
#define REGISTERS_H 1

/* 
 * The memory region the zImage is loaded into must have up to 4Megabytes of space after it;
 * The kernel image must be loaded with a 32K (0x8000) space below it: this hollow is used by
 * the kernel page tables.
 */
#define MEMORY_ADDR         0x0C000000UL
#define TAG_ADDR            (MEMORY_ADDR|0x100)      /* Parameters address */
#define ZTEXTADDR           (MEMORY_ADDR|0x200000)   /* Start address of decompressor */
#define INITRD_LOADADDR     (MEMORY_ADDR|0x400000)   /* Initrd address */

/* ARM CP15 registers */
#define CR_M	(1 << 0)	/* MMU enable */
#define CR_A	(1 << 1)	/* Alignment abort enable */
#define CR_C	(1 << 2)	/* Dcache enable */
#define CR_W	(1 << 3)	/* Write buffer enable */
#define CR_P	(1 << 4)	/* 32-bit exception handler */
#define CR_D	(1 << 5)	/* 32-bit data address range */
#define CR_L	(1 << 6)	/* Implementation defined */
#define CR_B	(1 << 7)	/* Big endian */
#define CR_S	(1 << 8)	/* System MMU protection */
#define CR_R	(1 << 9)	/* ROM MMU protection */
#define CR_F	(1 << 10)	/* Implementation defined */
#define CR_Z	(1 << 11)	/* Implementation defined */
#define CR_I	(1 << 12)	/* Icache enable */
#define CR_V	(1 << 13)	/* Vectors relocated to 0xffff0000 */
#define CR_RR	(1 << 14)	/* Round Robin cache replacement */
#define CR_L4	(1 << 15)	/* LDR pc can set T bit */
#define CR_DT	(1 << 16)
#define CR_IT	(1 << 18)
#define CR_ST	(1 << 19)
#define CR_FI	(1 << 21)	/* Fast interrupt (lower latency mode) */
#define CR_U	(1 << 22)	/* Unaligned access operation */
#define CR_XP	(1 << 23)	/* Extended page tables */
#define CR_VE	(1 << 24)	/* Vectored interrupts */


/* S3C2400 IRQ registers */
#define IRQ_BASE            0x14400000UL
#define IRQ_INTMSK          (IRQ_BASE|0x08)          /* Interrupt mask register */
#define IRQ_INTMSK_UTXD1    (1 << 24)
#define IRQ_INTMSK_UTXD0    (1 << 23)

/* S3C2400 CLOCK control registers */
#define LOCKTIME            0x14800000UL
#define MPLLCON             (LOCKTIME|0x04)
#define UPLLCON             (LOCKTIME|0x08)
#define CLKCON              (LOCKTIME|0x0C)
#define CLKCON_UART0        (1 << 8)
#define CLKCON_UART1        (1 << 9)
#define CLKSLOW             (LOCKTIME|0x10)
#define CLKDIVN             (LOCKTIME|0x14)
#define CLKDIVN_1_1_1       0x0                      /* FCLK:HCLK:PCLK */
#define CLKDIVN_1_1_2       0x1
#define CLKDIVN_1_2_1       0x2
#define CLKDIVN_1_2_2       0x3

#define PLLCON_MDIV(x)      (x << 12)                /* Main divider control */
#define PLLCON_PDIV(x)      (x <<  4)                /* Pre-divider control */
#define PLLCON_SDIV(x)      (x <<  0)                /* Post-divider control */

/* S3C2400 UART registers */
#define UART_BASE_ADDR      0x15000000UL
#define UART0               (UART_BASE_ADDR)         /* UART 0 base address */
#define UART1               (UART_BASE_ADDR|0x4000)  /* UART 1 base address */

#define ULCON(x)            (x)                      /* UART line control register */
#define ULCON_WORD8         (3 << 0)
#define ULCON_STOP1         (0 << 2)
#define ULCON_NOPAR         (0 << 3)
#define ULCON_NORMAL        (0 << 6)

#define UCON(x)             (x|0x04)             /* UART control register */
#define UCON_RXIRQ          (1 << 0)
#define UCON_TXIRQ          (1 << 2)
#define UCON_BREAK          (1 << 4)
#define UCON_RXERREN        (1 << 6)
#define UCON_TXPULSE        (0 << 9)
#define UCON_TXLEVEL        (1 << 9)

#define UFCON(x)            (x|0x08)		     /* FIFO control register */
#define UFCON_TX_FIFO_RST   (1 << 2)
#define UFCON_RX_FIFO_RST   (1 << 1)
#define UFCON_FIFO_ENABLE   (1 << 0)
#define UFCON_TX_TRIG_EMPTY (0 << 6)
#define UFCON_TX_TRIG_4     (1 << 6)
#define UFCON_TX_TRIG_8     (2 << 6)
#define UFCON_TX_TRIG_12    (3 << 6)

#define UMCON(x)            (x|0x0C)		     /* Modem control register */
#define UTRSTAT(x)          (x|0x10)		     /* Tx/Rx status register */
#define UERSTAT(x)          (x|0x14)	   	     /* Rx error status register */
#define UFSTAT(x)           (x|0x18)		     /* FIFO status register */
#define UMSTAT(x)           (x|0x1C)		     /* Modem status register */
#define UTXH(x)             (x|0x20)		     /* Transmit buffer register */
#define URXH(x)             (x|0x24)		     /* Receive buffer register */
#define UBRDIV(x)           (x|0x28)		     /* Baud rate divisor register */

/* S3C2400 IO pins */
#define PFCON               0x15600038UL 	     /* Port F control register */
#define PFCON_RXD0          (2 << 0)
#define PFCON_RXD1          (2 << 2)
#define PFCON_TXD0          (2 << 4)
#define PFCON_TXD1          (2 << 6)
#define PFCON_nRTS0         (2 << 8)
#define PFCON_nCTS0         (2 << 10)
#define PFUP                0x15600040UL 	     /* Port F pull-up disable register */

/* S3C2400 LCD registers */
#define LCDCON1             0x14A00000UL 	     /* LCD control 1 register */
#define LCDCON1_CLKVAL(x)   (((x) << 8) & 0x9)

/* GP32 Smart Media Card related pins */
#define MISCCR              0x15600054UL 	     /* Miscellaneous control register */
#define MISCCR_CLKSEL_MASK  (0x5 << 4)
#define MISCCR_CLKSEL_MPLL  (0 << 4)
#define MISCCR_CLKSEL_UPLL  (1 << 4)
#define MISCCR_CLKSEL_FCLK  (2 << 4)
#define MISCCR_CLKSEL_HCLK  (3 << 4)
#define MISCCR_CLKSEL_PCLK  (4 << 4)

#define GP32_MACH_NUMBER    706                  /* ARM mach type */
#define DEBUG_UART          UART0                /* UART to send debug messages */

#define set_register(x,y)     *(volatile unsigned long *)(x) = (y)
#define get_register(x)       *((volatile unsigned long *)(x))

#endif /* REGISTERS_H */
