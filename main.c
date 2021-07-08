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

int main() {
	static char* const code[]={
"CLS",
"CURSOR 2,2",
"PRINT 12345678;",
"PRINT 5678;",
"END",
		0
	};
	int e,i,s;
	char* str;
	// Initializations
	stdio_init_all();
	display_init();
	// Wait for three seconds
	sleep_ms(3000);
	// Start
	printstr("KM-BASIC for ARM\n");
	// Compile the code
	s=time_us_32();
	init_compiler();
	for(i=0;str=code[i];i++) {
		e=compile_line(str);
		if (e<0) break;
	}
	printint(time_us_32()-s);
	printstr(" micro seconds spent for compiling\n");
	// Show dump
	dump();
	//dump_cmpdata();
	// Run the code if error didn't occur
	if (0<=e) run_code();
	// Infinite loop
	for(i=0;true;i++){
		sleep_ms(1000);
		//run_code();
		printchar("-/|\\"[i&0x03]); printchar(0x08);
	}
	return 0;
}
