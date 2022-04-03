/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"

#define GPIO_KEYUP 0
#define GPIO_KEYLEFT 1
#define GPIO_KEYRIGHT 2
#define GPIO_KEYDOWN 3
#define GPIO_KEYSTART 4
#define GPIO_KEYFIRE 5
#define KEYUP (1<<GPIO_KEYUP)
#define KEYLEFT (1<<GPIO_KEYLEFT)
#define KEYRIGHT (1<<GPIO_KEYRIGHT)
#define KEYDOWN (1<<GPIO_KEYDOWN)
#define KEYSTART (1<<GPIO_KEYSTART)
#define KEYFIRE (1<<GPIO_KEYFIRE)
#define KEYSMASK (KEYUP|KEYLEFT|KEYRIGHT|KEYDOWN|KEYSTART|KEYFIRE)
int lib_keys(int r0, int r1, int r2){
	static char init=0;
	int res,k;
	if (!init) {
		init=1;
		gpio_init_mask(KEYSMASK);
		gpio_set_dir_in_masked(KEYSMASK);
		gpio_pull_up(GPIO_KEYUP);
		gpio_pull_up(GPIO_KEYLEFT);
		gpio_pull_up(GPIO_KEYRIGHT);
		gpio_pull_up(GPIO_KEYDOWN);
		gpio_pull_up(GPIO_KEYSTART);
		gpio_pull_up(GPIO_KEYFIRE);
	}
	k=~gpio_get_all() & KEYSMASK;
	res =(k&KEYUP)    ?  1:0;
	res|=(k&KEYDOWN)  ?  2:0;
	res|=(k&KEYLEFT)  ?  4:0;
	res|=(k&KEYRIGHT) ?  8:0;
	res|=(k&KEYSTART) ? 16:0;
	res|=(k&KEYFIRE)  ? 32:0;
	return res&r0;
}
