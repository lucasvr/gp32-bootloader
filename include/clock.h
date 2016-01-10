#ifndef CLOCK_H
#define CLOCK_H 1

#include <stdio.h>

unsigned long get_pll_value(int freq);
void          init_clocks(int base_clock);

#endif /* CLOCK_H */
