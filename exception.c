/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "hardware/exception.h"
#include "./compiler.h"
#include "./api.h"

void show_shcsr_dfsr(void){
	int* shcsr=(int*)0xE000ED24;
	int* dfsr=(int*)0xE000ED30;
	printstr("\nSHCSR: ");
	printhex32(shcsr[0]);
	printstr("\nDFSR:  ");
	printhex32(dfsr[0]);

}

void exception_handler_main(int* sp){
	int i;
	short* pc;
	printstr("\n\nException: hard fault.\n");
	// Show the information in stack
	printstr("\nSP:  ");
	printhex32((int)sp);
	printstr("\nR0:  ");
	printhex32(sp[0]);
	printstr("\nR1:  ");
	printhex32(sp[1]);
	printstr("\nR2:  ");
	printhex32(sp[2]);
	printstr("\nR3:  ");
	printhex32(sp[3]);
	printstr("\nR12: ");
	printhex32(sp[4]);
	printstr("\nLR:  ");
	printhex32(sp[5]);
	printstr("\nPC:  ");
	printhex32(sp[6]);
	// Show the code where exception happened
	printstr("       \x1f\n");
	pc=(short*)sp[6];
	for(i=-4;i<4;i++){
		printhex16(pc[i]);
		printchar(' ');
	}
	// Show SHCSR and DHSR
	//show_shcsr_dfsr();
	// Set PC to kmbasic_data[3] if exception happened in RAM
	if (((int)&kmbasic_object[0])<=sp[6] && sp[6]<((int)&kmbasic_object[0])+sizeof(kmbasic_object)) kmbasic_data[3]=sp[6];
	//
	printstr("\nReset MachiKania to continue.\n");
	stop_with_error(ERROR_EXCEPTION);
}

void exception_handler(void){
	asm("movs r0,sp");
	//asm("ldr r1,=0x20041000");
	//asm("movs sp,r1");
	asm("b exception_handler_main");
}

void handle_exception(int set){
	static exception_handler_t org_handler=0;
	if (!org_handler) {
		// Store the default handler in the beginning
		org_handler=exception_get_vtable_handler(HARDFAULT_EXCEPTION);
	}
	if (set) {
		// Use the exception handler here
		exception_set_exclusive_handler(HARDFAULT_EXCEPTION,exception_handler);
	} else {
		// Go back to the default handler
		exception_restore_handler(HARDFAULT_EXCEPTION,org_handler);
	}
}
