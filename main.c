/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"
#include "./debug.h"
#include "./display.h"
#include "./config.h"

int main() {
	int e,i,s;
	char* str;
	// Initializations
	io_init();
	timer_init();
	sleep_ms(500);
	stdio_init_all();
	display_init();
	init_buttons();
	init_file_system();
	fileselect_init();
	// Get filename to compile
	if (file_exists("MACHIKAP.BAS")) str="MACHIKAP.BAS";
	else str=fileselect();
	// Start
	printstr("MachiKania BASIC System\n");
	printstr(" Ver "SYSVER1" "SYSVER2"\n");
	printstr("BASIC Compiler "BASVER" by Katsumi\n");
	printstr("LCD and File systems by KENKEN\n");
	printstr("\n");
	// Compile the code
	s=time_us_32();
	init_compiler();
	e=compile_file(str,0);
	if (!e) e=post_compile();
	printint(time_us_32()-s);
	printstr(" micro seconds spent for compiling\n");
	printstr("\n");
	// Show dump
	// dump();
	// Run the code if error didn't occur
	if (!e) {
		pre_run();
		run_code();
		post_run();
	}
	// Show dump
	//dump_variables();
	// Infinite loop
	for(i=0;i<16;i++){
		sleep_ms(1000);
		//run_code();
		printchar("-/|\\"[i&0x03]); printchar(0x08);
	}
	return 0;
}
