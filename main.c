/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"
#include "./debug.h"
#include "./display.h"
#include "./config.h"

char g_autoexec[12]="MACHIKAP.BAS";

void read_ini(void){
	int i;
	FIL fpo;
	unsigned char* str=g_compile_buffer;
	// Open INI file
	if (f_open(&fpo,"MACHIKAP.INI",FA_READ)) return;
	// Read each line
	while(f_gets(str,g_file_buffer_size,&fpo)){
		if (!strncmp(str,"AUTOEXEC=",9)) {
			// Get file name
			for(i=0;i<12;i++){
				if (str[i+9]<0x21) break;
				g_autoexec[i]=str[i+9];
			}
			g_autoexec[i]=0x00;
		} else if (!strncmp(str,"VERTICAL",8)) {
			set_lcdalign(VERTICAL);
		} else if (!strncmp(str,"HORIZONTAL",10)) {
			set_lcdalign(HORIZONTAL);
		} else if (!strncmp(str,"LCD270TURN",10)) {
			set_lcdalign(VERTICAL);
		} else if (!strncmp(str,"LCD0TURN",8)) {
			set_lcdalign(HORIZONTAL);
		} else if (!strncmp(str,"LCD90TURN",9)) {
			set_lcdalign(VERTICAL | LCD180TURN);
		} else if (!strncmp(str,"LCD180TURN",10)) {
			set_lcdalign(HORIZONTAL | LCD180TURN);
		} else if (!strncmp(str,"ROTATEBUTTONS",13)) {
			g_enable_button_rotation=1;
		} else if (!strncmp(str,"NOROTATEBUTTONS",15)) {
			g_enable_button_rotation=0;
		} else if (!strncmp(str,"USBSERIALON",11)) {
			g_disable_printf=0;
		} else if (!strncmp(str,"USBSERIALOFF",12)) {
			g_disable_printf=1;
		} else if (!strncmp(str,"DEBUGWAIT2500",13)) {
			g_disable_debugwait2500=0;
		} else if (!strncmp(str,"NODEBUGWAIT2500",15)) {
			g_disable_debugwait2500=1;
		}
	}
	// Close file
	f_close(&fpo);
}

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
	// Read MACHIKAP.INI
	read_ini();
	// Get filename to compile
	if (file_exists(g_autoexec)) str=&g_autoexec[0];
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
