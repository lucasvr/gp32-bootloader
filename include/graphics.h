#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include <stdio.h>

/* The GP32 color scheme is arranged like this:
 * RRRR RGGG GGBB BBB0
 *
 * so, we have:
 * 0xF800         RED
 * 0x07C0         GREEN
 * 0x003E         BLUE
 */

#define RED         0xF800
#define GREEN       0x07C0
#define BLUE        0x003E

#define WIDTH          320
#define HEIGHT         240
#define DEPTH            2          /* 16bpp framebuffer */
#define TICKS_IN_A_SEC  64          /* how many timer ticks we have in a second */

void lcd_init(int clear_screen);
void lcd_splashscreen(int flip);
int  lcd_bootloader_menu(void);
void lcd_printf(char *buf, int wherex, int wherey, unsigned short color, int flip);
void lcd_printf_front(char *buf, int wherex, int wherey, unsigned short color);

#endif /* GRAPHICS_H */
