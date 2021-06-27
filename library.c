/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "./api.h"
#include "./compiler.h"

/*
	Local macros
*/

// Use 1016 bytes stack area dedicated for library
// This routine is required to prevent mulfunctions of some library call
#define use_lib_stack(funcname) \
	asm("push {r4,lr}");\
	asm("mov r4,sp");\
	asm("ldr r3,[r7,#0]");\
	asm("mov sp,r3");\
	asm("bl "funcname);\
	asm("mov sp,r4");\
	asm("pop {r4,pc}")

int lib_add_string(int r0, int r1, int r2){
	int i,j;
	char* res;
	char* str1=(char*)r1;
	char* str2=(char*)r0;
	// Determine total length
	for(i=0;str1[i];i++);
	for(j=0;str2[j];j++);
	// Allocate memory
	res=alloc_memory((i+j+4)/4,-1);
	// Copy string
	for(i=0;str1[i];i++) res[i]=str1[i];
	for(j=0;str2[j];j++) res[i++]=str2[j];
	res[i]=0x00;
	// Return string
	return (int)res;
}
int lib_hex(int width, int num, int r2){
	char* str;
	int i,j,minus;
	str=alloc_memory(3,-1);
	for(i=0;i<8;i++){
		str[i]="0123456789ABCDEF"[(num>>((7-i)<<2))&0x0F];
	}
	// Width must be between 0 and 8;
	if (width<0||8<width) width=8;
	if (width==0) {
		// Width not asigned. Use the minimum width.
		for(i=0;i<7;i++){
			if ('0'!=str[i]) break;
		}
		width=8-i;
	}
	// Shift string
	j=8-width;
	for(i=0;i<width;i++) str[i]=str[j++];
	str[i]=0x00;
	return (int)str;
}

int lib_print_main(int r0, int r1, int r2){
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	int i;
	float f;
	char* buff=(char*)&g_scratch[0];
	switch(r1&0x0f){
		case 0x01: // string
			for(i=0;((unsigned char*)r0)[i];i++);
			printstr((unsigned char*)r0);
			if (0x00 == (r1&0xf0)) printchar('\n');
			break;
		case 0x02: // float
			g_scratch_int[0]=r0;
			f=g_scratch_float[0];
			if (0x00 == (r1&0xf0)) i=snprintf(buff,sizeof g_scratch,"%g\n",f);
			else i=snprintf(buff,sizeof g_scratch,"%g",f);
			printstr(buff);
			break;
		default:   // integer
			if (0x00 == (r1&0xf0)) i=snprintf(buff,sizeof g_scratch,"%d\n",(int)r0);
			else i=snprintf(buff,sizeof g_scratch,"%d",(int)r0);
			printstr(buff);
			break;
	}
	if (0x20==(r1&0xf0)) {
		// ","
		printstr(&("                "[i&0xf]));
	}
	return r0;
}

void lib_print(){
	use_lib_stack("lib_print_main");
}

int lib_let_str(int r0, int r1, int r2){
	int i;
	char* str=(char*)r0;
	char* res;
	// If r0 is pointer to temporary area, use it.
	if (move_from_temp(r1,r0)) return r0;
	// It's not in temporary area. Need to be copied.
	// Calculate length
	for(i=0;str[i];i++);
	// Allocate memory
	res=alloc_memory((i+4)/4,r1);
	// Copy string
	for(i=0;res[i]=str[i];i++);
	// All done
	return (int)res;
}

int lib_calc(int r0, int r1, int r2){
	switch(r2){
		case OP_DIV: return r1/r0;
		case OP_REM: return r1%r0;
		default: // error
			return r0;
	}
}

float lib_calc_float_main(float r0, float r1, int r2){
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
	g_scratch_int[0]=r0;
	g_scratch_int[1]=r1;
	g_scratch_float[2]=lib_calc_float_main(g_scratch_float[0],g_scratch_float[1],r2);
	return g_scratch_int[2];
}

int lib_line_num(int r0, int r1, int r2){
	int* data=cmpdata_findfirst_with_id(CMPDATA_LINENUM,r0);
	if (data) return data[1];// Found
	// Not found
	printstr("\nLine ");
	printint(r0);
	printstr(" not found\n");
	return lib_end(r0,r1,r2);
}

int lib_end(int r0, int r1, int r2){
	asm("ldr r0, [r7, #0]");
	asm("mov sp, r0");
	asm("ldr r0, [r7, #4]");
	asm("bx r0");
	return r0;
}

int debug(int r0, int r1, int r2){
	asm("mov r0,#0");
	asm("add r0,r0,r5");
	return r0<<2;
}

static const void* lib_list1[]={
	lib_calc,       // #define LIB_CALC 0
	lib_calc_float, // #define LIB_CALC_FLOAT 1
	lib_hex,        // #define LIB_HEX 2
	lib_add_string, // #define LIB_ADD_STRING 3
};

static const void* lib_list2[]={
	debug,          // #define LIB_DEBUG 128
	lib_print,      // #define LIB_PRINT 129
	lib_let_str,    // #define LIB_LET_STR 130
	lib_end,        // #define LIB_END 131
	lib_line_num,   // #define LIB_LINE_NUM 132
};

int statement_library(int r0, int r1, int r2, int r3){
	int (*f)(int r0, int r1, int r2) = lib_list2[r3-128];
	r0=f(r0,r1,r2);
	// Raise garbage collection flag
	g_garbage_collection=1;
	// TODO: Check break key
	return r0;
}

int kmbasic_library(int r0, int r1, int r2, int r3){
	// Confirm push {r4, lr} assembly in kmbasic.dis file
	// Store return address in R7[3]
	asm("mov r4,lr");
	asm("str r4,[r7,#12]");
	// Quick library
	int (*f)(int r0, int r1, int r2) = lib_list1[r3];
	if (r3<128) return f(r0,r1,r2);
	// Statement library
	else return statement_library(r0,r1,r2,r3);
}
