/*----------------------------------------------------------------------------

Copyright (C) 2025, Katsumi, all right reserved.

This program supplied herewith by Katsumi is free software; you can
redistribute it and/or modify it under the terms of the same license written
here and only for non-commercial purpose.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of FITNESS FOR A PARTICULAR
PURPOSE. The copyright owner and contributors are NOT LIABLE for any damages
caused by using this program.

----------------------------------------------------------------------------*/
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "usbkeyboard.h"
#include "../compiler.h"

/*
	Keyboard-I2C settings follow
	Note that the baud-rate can be as fast as 250 kHz, but use 10 kHz here.
	As the usbkb_polling() function is executed by second CPU core,
	the slower I2C communication does not affect execution of BASIC code very much.
	The BASIC code execution speed at 10 kHz I2C baud-rate is 99.99% of that at 250 kHz.
	The I2C-keyboard function at 10 kHz occupies 12 % power of second CPU core.
*/

#define MACHIKANIA_PC_I2C_KBD_MOD i2c1
#define MACHIKANIA_PC_I2C_KBD_SDA 6
#define MACHIKANIA_PC_I2C_KBD_SCL 7
#define MACHIKANIA_PC_I2C_KBD_SPEED  10000
#define MACHIKANIA_PC_I2C_KBD_ADDR 0x1F

/*
	Machikania global variables follow
*/

volatile uint8_t usbkb_keystatus[256]; // 仮想コードに相当するキーの状態（Onの時1）
uint16_t vkey; // usbkb_readkey()関数でセットされるキーコード、上位8ビットはシフト関連キー
uint8_t lockkey; // 初期化時にLockキーの状態指定。下位3ビットが<SCRLK><CAPSLK><NUMLK>
uint8_t keytype; // キーボードの種類。0：日本語109キー、1：英語104キー

uint16_t keycodebuf[KEYCODEBUFSIZE]; //キーコードバッファ
uint16_t * volatile keycodebufp1; //キーコード書き込み先頭ポインタ
uint16_t * volatile keycodebufp2; //キーコード読み出し先頭ポインタ

bool usb_set_report(void* report, uint16_t len){ return false; } // Dummy function required

/*
	Static global variables follow
*/

static char g_shift_status=0;
static const uint8_t g_i2c_to_vkey[]={
	// 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x09,0x0D,0x00,0x00,0x00,0x00,0x00, // 0x00-0x0F
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0x10-0x1F
	0x20,'1' ,0xDE,'3' ,'4' ,'5' ,'7' ,0xDE,'9' ,'0' ,'8' ,0xBB,0xBC,0xBD,0xBE,0xBF, // 0x20-0x2F
	'0' ,'1' ,'2' ,'3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,0xBA,0xBA,0xBC,0xBB,0xBE,0xBF, // 0x30-0x3F
	'2' ,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' , // 0x40-0x4F
	'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'X' ,'X' ,'Y' ,'Z' ,0xDB,0xDC,0xDD,'6' ,0xBD, // 0x50-0x5F
	0xC0,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' , // 0x60-0x6F
	'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'X' ,'X' ,'Y' ,'Z' ,0xDB,0xDC,0xDD,0xC0,0x00, // 0x70-0x7F
	0x00,0x70,0x71,0x72,0x73,0x74,0x70,0x71,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00, // 0x80-0x8F
	0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0x90-0x9F
	0x00,0xA4,0xA0,0xA1,0x00,0xA2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xA0-0xAF
	0x00,0x1B,0x00,0x00,0x25,0x26,0x28,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xB0-0xBF
	0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xC0-0xCF
	0x1B,'I' ,0x24,0x00,0x2E,0x23,0x26,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xD0-0xDF
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xE0-0xEF
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0xF0-0xFF
};

void usbkb_release_all(void){
	unsigned int* p_keystatus=(unsigned int*)&usbkb_keystatus[0];
	for(int i=0;i<64;i++) p_keystatus[i]=0;
}

bool usbkb_init(void){
	// Initialice I2C system
	gpio_set_function(MACHIKANIA_PC_I2C_KBD_SCL, GPIO_FUNC_I2C);
	gpio_set_function(MACHIKANIA_PC_I2C_KBD_SDA, GPIO_FUNC_I2C);
	i2c_init(MACHIKANIA_PC_I2C_KBD_MOD, MACHIKANIA_PC_I2C_KBD_SPEED);
	gpio_pull_up(MACHIKANIA_PC_I2C_KBD_SCL);
	gpio_pull_up(MACHIKANIA_PC_I2C_KBD_SDA);
	// Set English keyboard type
	keytype=1;
	// Initialize buffer
	keycodebufp1=keycodebufp2=keycodebuf;
	usbkb_release_all();
	return true;
}

bool usbkb_mounted(void){
	return true;
}

void usbkb_polling(void){
	// Main routine
	static unsigned char s_prev_vkey=0;
	static char s_caps_lock=0;
	static char s_write_or_read=0;
	static unsigned char s_null_count=0;
	static int s_clock_hz=0;
	unsigned char buff[2];
	unsigned char current_vkey;
	int ret;
	if (g_clock_hz!=s_clock_hz) {
		// CPU clock has been changed
		if (0!=s_clock_hz) i2c_init(MACHIKANIA_PC_I2C_KBD_MOD, MACHIKANIA_PC_I2C_KBD_SPEED);
		s_clock_hz=g_clock_hz;
	}
	if (0==s_write_or_read) {
		s_write_or_read=1;
		// Write command to I2C keyboard
		buff[0]=0x09;
		ret=i2c_write_blocking(MACHIKANIA_PC_I2C_KBD_MOD,MACHIKANIA_PC_I2C_KBD_ADDR,&buff[0],1,false);
		if (ret<0) {
			// Error occurred.
			// Reset I2C for keyboard and do it again
			usbkb_init();
			s_write_or_read=0;
		}
		return;
	} else {
		s_write_or_read=0;
		// Read data from I2C keyboard
		ret=i2c_read_blocking(MACHIKANIA_PC_I2C_KBD_MOD,MACHIKANIA_PC_I2C_KBD_ADDR,&buff[0],2,false);
		if (ret<0) {
			// Error occurred.
			// Reset I2C for keyboard and do it again
			usbkb_init();
			return;
		}
	}
	// Data reading succeeded
	current_vkey=g_i2c_to_vkey[buff[1]];
	switch(buff[0]){
		case 0x01:
			// A key is pressed
			usbkb_keystatus[current_vkey]=1;
			switch(current_vkey){
				case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
					// Shift key
					g_shift_status|=CHK_SHIFT;
					break;
				case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
					// Ctrl key
					g_shift_status|=CHK_CTRL;
					break;
				case VK_MENU: case VK_LMENU: case VK_RMENU:
					// Alt key
					g_shift_status|=CHK_ALT;
					break;
				default:
					if (current_vkey!=s_prev_vkey) {
						// New press of a key (also emulate repeating key press)
						// Write the code, first
						keycodebufp1[0]=(g_shift_status<<8)|buff[1];
						// Increment writing point
						keycodebufp1++;
						if(keycodebufp1==keycodebuf+KEYCODEBUFSIZE) keycodebufp1=keycodebuf;
					}
					break;
			}
			break;
		case 0x03:
			// A key is unpressed
			usbkb_keystatus[current_vkey]=0;
			switch(current_vkey){
				case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
					// Shift key
					g_shift_status&=~CHK_SHIFT;
					break;
				case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
					// Ctrl key
					g_shift_status&=~CHK_CTRL;
					break;
				case VK_MENU: case VK_LMENU: case VK_RMENU:
					// Alt key
					g_shift_status&=~CHK_ALT;
					break;
				case VK_CAPITAL:
					// CapsLK key
					g_shift_status^=CHK_CAPSLK;
					break;
				default:
					break;
			}
			break;
		case 0x00:
			// All keys are released
			if (0x00==s_prev_vkey) {
				s_null_count++;
				if (50==s_null_count) usbkb_release_all();
			} else {
				s_null_count=0;
			}
			s_prev_vkey=0x00;
			return;
		case 0x02:
			// Ignore the case of 0x02 (continuous pressing SHIFT etc)
		default:
			return;
	}
	s_prev_vkey=current_vkey;
}

unsigned char shiftkeys(void){
	// Just return global variable
	return g_shift_status;
}

unsigned char usbkb_readkey(void){
	// 入力された1つのキーのキーコードをグローバル変数vkeyに格納（押されていなければ0を返す）
	// 下位8ビット：キーコード
	// 上位8ビット：シフト状態（押下：1）、上位から<0><CAPSLK><NUMLK><SCRLK><Win><ALT><CTRL><SHIFT>
	// 英数・記号文字の場合、戻り値としてASCIIコード（それ以外は0を返す）
	unsigned char ascii;
	if (keycodebufp2==keycodebufp1) {
		vkey=0;
		return 0;
	}
	ascii=keycodebufp2[0]&255;
	vkey=keycodebufp2[0]&0xff00;
	vkey|=g_i2c_to_vkey[ascii];
	// Special case conversions
	if (vkey&(CHK_ALT<<8)) {
		// Alt key + xx
		switch(vkey&255){
			case VK_UP:    // Alt + UP -> Page UP
				vkey=vkey&0xff00;
				vkey|=VK_PRIOR;
				break;
			case VK_DOWN:  // Alt + DOWN -> Page DOWN
				vkey=vkey&0xff00;
				vkey|=VK_NEXT;
				break;
			case VK_RIGHT: // Alt + RIGHT -> Shift + RIGHT
				vkey=CHK_SHIFT<<8;
				vkey|=VK_RIGHT;
				break;
			case VK_LEFT:  // Alt + LEFT -> Shift + LEFT
				vkey=CHK_SHIFT<<8;
				vkey|=VK_LEFT;
				break;
			case VK_TAB:    // Alt + TAB -> HOME
				vkey=vkey&0xff00;
				vkey|=VK_HOME;
				break;
			case VK_DELETE: // Alt + DEL -> END
				vkey=vkey&0xff00;
				vkey|=VK_END;
				break;
			case 'I':       // Alt + I -> Ins
				vkey=vkey&0xff00;
				vkey|=VK_INSERT;
				break;
			default:
				break;
		}
	} 
	keycodebufp2++;
	if(keycodebufp2==keycodebuf+KEYCODEBUFSIZE) keycodebufp2=keycodebuf;
	// If ALT/CTRL key is pressed, return 0
	if (vkey&( (CHK_ALT|CHK_CTRL) <<8 )) return 0;
	// If alphabet or character, return ascii code
	if (0x20<=ascii && ascii<0x7f) return ascii;
	// Otherwise, return 0;
	return 0;
}

