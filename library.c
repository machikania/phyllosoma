/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./api.h"
#include "./compiler.h"


int lib_print(int r0, int r1, int r2, int r3){
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	unsigned char buff[16]; // TODO: Consider buffer size
	switch(r1&0x0f){
		case 0x01: // string
			printstr((unsigned char*)r0);
			if (0x00 == (r1&0xf0)) printchar('\n');
			break;
		case 0x02: // float
			if (0x00 == (r1&0xf0)) sprintf(buff,"%g\n",(float)r0);
			else sprintf(buff,"%g",(float)r0);
			printstr(buff);
			break;
		default:   // integer
			if (0x00 == (r1&0xf0)) sprintf(buff,"%d\n",(int)r0);
			else sprintf(buff,"%d",(int)r0);
			printstr(buff);
			break;
	}
	if (0x20==r1&0xf0) {
		// ","
		// TODO: Not yet implimented
	}
	return r0;
}
int lib_let_str(int r0, int r1, int r2, int r3){
	return r0;
}

static const void* lib_list[]={
	lib_print,      // #define LIB_PRINT 0
	lib_let_str,    // #define LIB_LET_STR 1
};

int kmbasic_library(int r0, int r1, int r2, int r3){
	int (*f)(int r0, int r1, int r2, int r3) = lib_list[r3];
	return f(r0,r1,r2,r3);
}
