/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./api.h"
#include "./compiler.h"

void dump(){
	int i;
	printf("\n");
	for(i=0;i<64;i++){
		printf("%x%x%x%x ",(kmbasic_object[i]>>12)&0xf,(kmbasic_object[i]>>8)&0xf,(kmbasic_object[i]>>4)&0xf,kmbasic_object[i]&0xf);
	}
	printf("\n\n");
	sleep_ms(1);
}

int main() {
	int e;
	stdio_init_all();
	// Wait for two seconds
	sleep_ms(2000);
	// Start
	printstr("KM-BASIC for ARM\n");
	init_compiler();
	compile_line("A#=12.340");
	compile_line("B#=23.450");
	compile_line("C#=34.560");
	compile_line("D#=A#+B#*C#");
	compile_line("PRINT A#,B#,C#,D#");
	compile_line("END");
	dump();
	//printf("Error: %d",e);
	run_code();
	while (true) {
		sleep_ms(1000);
		//run_code();
		printchar('.');
	}
	return 0;
}
