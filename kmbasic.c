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

void dump(void){
	int i;
	printf("\nkmbasic_object:");
	printhex32((int)&kmbasic_object[0]);
	printf("\n");
	for(i=0;i<128;i++){
		printf("%x%x%x%x ",
			(kmbasic_object[i]>>12)&0xf,
			(kmbasic_object[i]>>8)&0xf,
			(kmbasic_object[i]>>4)&0xf,
			kmbasic_object[i]&0xf);
		if (0x0000==kmbasic_object[i] && 0x0000==kmbasic_object[i+1] && 0x0000==kmbasic_object[i+2]) break;
	}
	printf("\n\n");
	sleep_ms(1);
}

void dump_cmpdata(void){
	int* data;
	unsigned int i,num;
	printstr("\nCMPDATA dump\n");
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_ALL)){
		num=(data[0]>>16)&0xff;
		for(i=0;i<num;i++){
			printhex32(data[i]);
			printstr(" ");
		}
		printstr("\n");
	}
}

int main() {
	int e;
	stdio_init_all();
	// Wait for two seconds
	sleep_ms(2000);
	// Start
	printstr("KM-BASIC for ARM\n");
	init_compiler();
	compile_line("PRINT 12,");
	compile_line("PRINT GOSUB(LBL1,34)");
	compile_line("END");
	compile_line("LABEL LBL1");
	compile_line("  PRINT ARGS(1),");
	compile_line("  GOSUB LBL2");
	compile_line("RETURN 78");
	compile_line("LABEL LBL2");
	compile_line("  PRINT 56,");
	compile_line("RETURN");
	dump();
	
	run_code();
	for(e=0;true;e++){
		sleep_ms(1000);
		//run_code();
		printchar("-/|\\"[e&0x03]); printchar(0x08);
	}
	return 0;
}
