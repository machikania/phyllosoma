/*
   This program is provided under the LGPL license ver 2.1
   Written by Katsumi and Kenken.
   http://hp.vector.co.jp/authors/VA016157/
   http://www.ze.em-net.ne.jp/~kenken/index.html
   https://github.com/kmorimatsu
   https://github.com/kenkenMkIISR
*/

#include "./compiler.h"
#include "./api.h"
#include "./core1.h"
#include "./interface/usbkeyboard.h"
#include "./interface/keyinput.h"

/*
	withkeyboard.c and withoutkeyboard.c are alternatively compiled.
	See CMakeLists.txt.
	
	withkeyboard.c
		USB keyboard is active. Instead, USB serial communication with PC is disabled.
	
	withoutkeyboard.c
		USB serial communication with PC is active. Instead, USB keyboard is disabled.
*/

const char g_active_usb_keyboard=1;

static char g_fileselect_no_keyboard=0;

void usbk_polling_handler(void){
  usbkb_polling();
  request_core1_callback_at(usbk_polling_handler,time_us_32()+10000);
}

void post_inifile(void){
	unsigned int t;
	// USB keyboard and editor mode
	g_disable_printf=1;
	if(!usbkb_init()){
	    printstr("Init USB failed...\n");
		while(1) sleep_ms(1000);
	}
    printstr("Init USB OK\n");
	start_core1();
	request_core1_callback(usbk_polling_handler);
	// Waiting for USB keyboard connected
	t=time_us_32();
	while(!usbkb_mounted()) {
		sleep_ms(16);
		if (!g_wait_for_keyboard) continue;
		if (g_wait_for_keyboard*1000<time_us_32()-t) {
		    printstr("USB keyboard not found\n");
			sleep_ms(500);
			g_fileselect_no_keyboard=1;
			return;
		}
	}
    printstr("USB keyboard found\n");
	sleep_ms(500);
}

void pre_fileselect(void){
	if (g_fileselect_no_keyboard) return; // File select (no keyboard found)
	texteditor(); // Start editor, never come back
}

int lib_readkey(int r0, int r1, int r2){
	int ret=usbkb_readkey();
	return ret | ((vkey & 0x0cff)<<8) | ((vkey & 0x3100)<<9) | ((vkey & 0x0200)<<7) | ((vkey & 0x4000)<<6);
}

int lib_inkey(int r0, int r1, int r2){
	int i;
	if (r0) {
		return usbkb_keystatus[r0&0xff];
	} else {
		for(i=0;i<256;i++){
			if (usbkb_keystatus[i]) return i;
		}
		return 0;
	}
}

int lib_input(int r0, int r1, int r2){
	unsigned char* str=calloc_memory(64,-1);
	// Clear key buffer
	do usbkb_readkey();
	while(vkey!=0);
	// Get string as a line
	lineinput(str,255);
	return (int)str;
}

int check_break(void){
	if (usbkb_keystatus[VK_PAUSE]) return 1; // Break/Pause key
	if (usbkb_keystatus[VK_LCONTROL] || usbkb_keystatus[VK_RCONTROL]) { // control keys
		if (usbkb_keystatus[VK_LMENU] || usbkb_keystatus[VK_RMENU]) { // alt keys
			if (usbkb_keystatus[VK_BACK]) return 1;      // backspace key
			if (usbkb_keystatus[VK_DELETE]) return 1;    // delete key
		}
	}
	return 0;
}

int check_keypress(void){
	return keycodeExists() ?1:0;
}
