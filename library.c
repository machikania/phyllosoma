/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./api.h"
#include "./compiler.h"


int lib_print(int r0, int r1, int r2){
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	int i;
	unsigned char buff[16]; // TODO: Consider buffer size
	switch(r1&0x0f){
		case 0x01: // string
			for(i=0;((unsigned char*)r0)[i];i++);
			printstr((unsigned char*)r0);
			if (0x00 == (r1&0xf0)) printchar('\n');
			break;
		case 0x02: // float
			if (0x00 == (r1&0xf0)) i=sprintf(buff,"%g\n",(float)r0);
			else i=sprintf(buff,"%g",(float)r0);
			printstr(buff);
			break;
		default:   // integer
			if (0x00 == (r1&0xf0)) i=sprintf(buff,"%d\n",(int)r0);
			else i=sprintf(buff,"%d",(int)r0);
			printstr(buff);
			break;
	}
	if (0x20==(r1&0xf0)) {
		// ","
		printstr(&("                "[i&0xf]));
	}
	return r0;
}
int lib_let_str(int r0, int r1, int r2){
	return r0;
}

int lib_calc(int r0, int r1, int r2){
	switch(r2){
		case OP_DIV: return r1/r0;
		case OP_REM: return r1%r0;
		default: // error
			return r0;
	}
}

float _lib_calc_float(float r0, float r1, int r2){
	switch(r2){
		case OP_EQ:  return r1==r0 ? 1:0;
		case OP_NEQ: return r1!=r0 ? 1:0;
		case OP_LT:  return r1<r0 ? 1:0;
		case OP_LTE: return r1<=r0 ? 1:0;
		case OP_MT:  return r1>r0 ? 1:0;
		case OP_MTE: return r1>=r0 ? 1:0;
		case OP_ADD: return r1+r0;
		case OP_SUB: return r1-r0;
		case OP_MUL: return r1*r0;
		case OP_DIV: return r1/r0;
		case OP_OR:  return (r1||r0) ? 1:0;
		case OP_AND: return (r1&&r0) ? 1:0;
		default: // error
			return r0;
	}
}

int lib_calc_float(int r0, int r1, int r2){
	return (int)_lib_calc_float((float)r0,(float)r1,r2);
}

int debug(int r0, int r1, int r2){
	asm("mov r0,#123");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("ldr	r0, [pc, #0]");
	asm("b skip");
	asm("nop");
	asm("nop");
	asm("skip:");
	asm("str r0,[r3,#0]");
	asm("nop");
	asm("nop");
	return lib_print(g_scratch[0],0,r2);
}

static const void* lib_list[]={
	debug,          // #define LIB_DEBUG 0
	lib_print,      // #define LIB_PRINT 1
	lib_let_str,    // #define LIB_LET_STR 2
	lib_calc,       // #define LIB_CALC 3
	lib_calc_float, // #define LIB_CALC_FLOAT 4
};

int kmbasic_library(int r0, int r1, int r2, int r3){
	int (*f)(int r0, int r1, int r2) = lib_list[r3];
	return f(r0,r1,r2);
}
