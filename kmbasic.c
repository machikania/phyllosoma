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
	compile_line("PRINT 12+23*34,(12+23)*34,12*23+34,12*(23+34)");
	compile_line("END");
	dump();
	//printf("e:%d\n",e);
	run_code();
	
	while (true) {
		sleep_ms(1000);
		printchar('.');
	}
	return 0;
}
