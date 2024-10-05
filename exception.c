/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "hardware/exception.h"
#include "./compiler.h"
#include "./api.h"

static unsigned char g_exception_mode=0;
#define EXCEPTION_MODE_RESTART    1
#define EXCEPTION_MODE_SCREENSHOT 2
#define EXCEPTION_MODE_DUMP       4
static char g_screenshot_file[13];
static char g_dump_file[13];

int ini_file_exception(char* line){
	int i;
	if (!strncmp(line,"EXCRESET",8)) {
		g_exception_mode|=EXCEPTION_MODE_RESTART;
	} else if (!strncmp(line,"EXCSCREENSHOT=",14)) {
		line+=14;
		g_exception_mode|=EXCEPTION_MODE_SCREENSHOT;
		for(i=0;i<12;i++){
			if (line[0]<0x21) break;
			g_screenshot_file[i]=line[i];
		}
		g_screenshot_file[i]=0;
	} else if (!strncmp(line,"EXCDUMP=",8)) {
		line+=8;
		g_exception_mode|=EXCEPTION_MODE_DUMP;
		for(i=0;i<12;i++){
			if (line[0]<0x21) break;
			g_dump_file[i]=line[i];
		}
		g_dump_file[i]=0;
	} else {
		return 0;
	}
	return 1;
}

static void dump2file(char* fname, char* mem, int len){
	FIL fhandle;
	printstr("\nSave ");
	printstr(fname);
	f_chdir("/");
	printstr("...");
	f_open(&fhandle,fname,FA_CREATE_ALWAYS | FA_WRITE);
	f_write(&fhandle,mem,len,(int*)&g_scratch_int[0]);
	f_close(&fhandle);
	if (len==g_scratch_int[0]) printstr("OK");
	else printstr("failed");
	busy_wait_us(1000000);
}

static void show_shcsr_dfsr(void){
	int* shcsr=(int*)0xE000ED24;
	int* dfsr=(int*)0xE000ED30;
	printstr("\nSHCSR: ");
	printhex32(shcsr[0]);
	printstr("\nDFSR:  ");
	printhex32(dfsr[0]);
}

void exception_handler_main(int* sp, unsigned int icsr){
	int i;
	short* pc;
	if (EXCEPTION_MODE_RESTART==g_exception_mode) software_reset();
	g_interrupt_code=1;
	printstr("\n\nException: hard fault.\n");
	// Show the information in stack
	printstr("\nPSR: ");
	printhex32(sp[7]);
	printstr("\nICSR:");
	printhex32(icsr);
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
	// Save memory dumps
	if (g_exception_mode&EXCEPTION_MODE_SCREENSHOT) dump2file(&g_screenshot_file[0],&TVRAM[0],WIDTH_X*WIDTH_Y);
	if (g_exception_mode&EXCEPTION_MODE_DUMP) dump2file(&g_dump_file[0],(char*)0x20000000,0x00042000);
	// Restart
	if (g_exception_mode&EXCEPTION_MODE_RESTART) software_reset();	
	// Show SHCSR and DHSR
	//show_shcsr_dfsr();
	// Set PC to kmbasic_data[3] if exception happened in RAM
	if (((int)&kmbasic_object[0])<=sp[6] && sp[6]<((int)&kmbasic_object[0])+sizeof(kmbasic_object)) kmbasic_data[3]=sp[6];
	// The following code corresponds to "stop_with_error(ERROR_EXCEPTION);".
	sp[0]=(int)ERROR_EXCEPTION;
	sp[6]=(int)stop_with_error;
}

void exception_handler(void){
	asm("mov r0,sp");
	asm("ldr r1,=0xE000ED04"); // ICSR
	asm("ldr r1,[r1]");
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
