/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"
#include "./debug.h"
#include "./display.h"
#include "./config.h"
#include "./io.h"
#include "./interface/usbkeyboard.h"

char g_autoexec[13]="MACHIKAP.BAS";

void read_ini(void){
	int i;
	FIL fpo;
	unsigned char* str=g_compile_buffer;
	// Open INI file
	if (f_open(&fpo,"MACHIKAP.INI",FA_READ)) return;
	// Read each line
	while(f_gets(str,g_file_buffer_size,&fpo)){
		if (ini_file_rtc(str)) {
			continue;
		} else if (ini_file_wifi(str)) {
			continue;
		} else if (ini_file_exception(str)) {
			continue;
		} else if (ini_file_io(str)) {
			continue;
		} else if (!strncmp(str,"AUTOEXEC=",9)) {
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
		} else if (!strncmp(str,"LCDOUTON",8)) {
			g_disable_lcd_out=0;
		} else if (!strncmp(str,"LCDOUTOFF",9)) {
			g_disable_lcd_out=1;
		} else if (!strncmp(str,"DEBUGWAIT2500",13)) {
			g_disable_debugwait2500=0;
		} else if (!strncmp(str,"NODEBUGWAIT2500",15)) {
			g_disable_debugwait2500=1;
		} else if (!strncmp(str,"LOOPATEND",9)) {
			g_reset_at_end=0;
		} else if (!strncmp(str,"RESETATEND",10)) {
			g_reset_at_end=1;
		} else if (!strncmp(str,"STARTWAIT=",10)) {
			sscanf(str+10,"%d",&g_wait_at_begin);
			if (g_wait_at_begin<500) g_wait_at_begin=500;
		} else if (!strncmp(str,"106KEY",6)) {
			keytype=0;
		} else if (!strncmp(str,"101KEY",6)) {
			keytype=1;
		} else if (!strncmp(str,"NUMLOCK",7)) {
			lockkey|=1;
		} else if (!strncmp(str,"CAPSLOCK",8)) {
			lockkey|=2;
		} else if (!strncmp(str,"SCRLLOCK",8)) {
			lockkey|=4;
		} else if (!strncmp(str,"WAIT4KEYBOARD=",14)) {
			sscanf(str+14,"%d",&g_wait_for_keyboard);
		} else if (!strncmp(str,"SHOWTIMESTAMP",13)) {
			show_timestamp=1;
		} else if (!strncmp(str,"FILESORTBY=",11)) {
			sscanf(str+11,"%hhd",&filesortby);
		}
	}
	// Close file
	f_close(&fpo);
}

void software_reset(void){
	unsigned int* AIRCR=(unsigned int*)0xe000ed0c;
	AIRCR[0]=0x05FA0004;
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
	init_machikania_rtc();
	// Read MACHIKAP.INI
	read_ini();
	sleep_ms(g_wait_at_begin-500);
	
	// Connect to Wifi
	connect_wifi(1);
	
	// Connect to USB keyboard or to PC
	post_inifile();
	
	// Get filename to compile
	if (file_exists(g_autoexec)) {
		str=&g_autoexec[0];
	} else {
		// Open text editor if USB keyboard mode
		pre_fileselect();
		// Open file selector
		str=fileselect();
	}
	
	// Start
	printstr(INTRODUCE_MACHIKANIA);
	// Run application if HEX file
	for(i=0;str[i];i++);
	if (!strncmp(&str[i-4],".HEX",4)) runHex(str);
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
	// Dump RAM area to a file
	//memdump();
	// Run the code if error didn't occur
	if (!e) {
		pre_run();
		run_code();
		post_run();
	}
	// Show dump
	//dump_variables();
	// Infinite loop
	s=0;
	if (g_reset_at_end) software_reset();
	for(i=0;true;i++){
		sleep_ms(50);
		if (s) {
			if (lib_keys(16,0,0)) software_reset();
		} else {
			if (!lib_keys(16,0,0)) s=1;
		}
		if (320<=i) continue;
		if (i%20) continue;
		printchar("-/|\\"[(i/20)&0x03]); printchar(0x08);
	}
	return 0;
}
