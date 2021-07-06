/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./api.h"
#include "pico/stdlib.h"
#include "./compiler.h"

#ifndef DEBUG_MODE
void debug_dummy(void){}
#else // DEBUG_MODE

void dump(void){
	int i;
	printstr("\nkmbasic_object:");
	printhex32((int)&kmbasic_object[0]);
	printstr("\n");
	for(i=0;i<256;i++) {
		printhex16(kmbasic_object[i]);
		printchar(' ');
		if (0x0000==kmbasic_object[i] && 0x0000==kmbasic_object[i+1] && 0x0000==kmbasic_object[i+2]) break;
	}
	printstr("\n\n");
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

#endif // DEBUG_MODE

