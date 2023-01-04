/*
   This program is provided under the LGPL license ver 2.1
   Written by Katsumi and Kenken.
   http://hp.vector.co.jp/authors/VA016157/
   http://www.ze.em-net.ne.jp/~kenken/index.html
   https://github.com/kmorimatsu
   https://github.com/kenkenMkIISR
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./compiler.h"
#include "./api.h"
#include "./display.h"

/*
	withkeyboard.c and withoutkeyboard.c are alternatively compiled.
	See CMakeLists.txt.
	
	withkeyboard.c
		USB keyboard is active. Instead, USB serial communication with PC is disabled.
	
	withoutkeyboard.c
		USB serial communication with PC is active. Instead, USB keyboard is disabled.
*/

const char g_active_usb_keyboard=0;

// Dummy global vars for USB keyboard follow
uint8_t lockkey=0;
uint8_t keytype=0;

void post_inifile(void){
	// Connect to PC
	connect2pc();
}

void pre_fileselect(void){
	// Do nothing
}

int lib_readkey(int r0, int r1, int r2){
	int i=getchar_timeout_us(0);
	if (i<0 || 255<i) return 0;
	return i;
}

int lib_inkey(int r0, int r1, int r2){
	int i=getchar_timeout_us(0);
	if (i<0 || 255<i) i=0;
	if (0==r0) return i;
	else return r0==i ? 1:0;
}

int lib_input(int r0, int r1, int r2){
	int max=15;
	int num=0;
	unsigned char* str=alloc_memory(4,-1);
	unsigned char* str2;
	int c;
	while(1){
		// Get a character from console
		c=getchar_timeout_us(1000);
		if (c<0 || 255<c) continue;
		// Detect special keys
		switch(c){
			case '\r': // Enter
			case '\n': // Enter
				break;
			case 0x08: // BS
			case 0x7f: // BS
				if (0<num) {
					printstr("\b \b");
					num--;
				}
				continue;
			default:
				if (c<0x20) continue;
				break;
		}
		if ('\r'==c || '\n'==c) break;
		// Show the character on display
		printchar(c);
		// Add a character to buffer
		str[num++]=c;
		// If number of string exceeds maximum, increase buffer size
		if (max<num) {
			max=(max+1)*2;
			str2=alloc_memory(max/4,-1);
			max-=1;
			memcpy(str2,str,num);
			garbage_collection(str);
			str=str2;
		}
	}
	printchar('\n');
	str[num]=0x00;
	return (int)str;
}
