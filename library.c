/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

int lib_print(int r0, int r1, int r2){
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

/*

In some cases, following code is required for the correct response from printf() function used for float value.
See also the comment in run_code_main() function in compiler.c.
Briefly, different stack area is used for printf() function. In detail, see following repository:
https://github.com/kmorimatsu/kmbasic4arm/tree/31db9c2df3c0bcf61e184c23eb656b8dbcc5133d

int lib_print_main(int r0, int r1, int r2);
void lib_print(){
	use_lib_stack("lib_print_main");
}*/

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

int lib_dim(int argsnum, int varnum, int r2){
	// R1 is var number
	// R0 is number of integer values
	// R2 is pointer of data array
	int* sp=(int*)r2;
	int i,j;
	static int* heap;
	// Calculate total length.
	int len=0;  // Total length
	int size=1; // Size of current block
	for(i=0;i<argsnum;i++){
		size*=sp[i]+1;
		len+=size;
	}
	// Allocate memory
	heap=calloc_memory(len,varnum);
	// Construct pointers
	len=0;
	size=1;
	for(i=0;i<argsnum-1;i++){
		size*=sp[i]+1;
		for(j=0;j<size;j++){
			heap[len+j]=(int)&heap[len+size+(sp[i+1]+1)*j];
		}
		len+=size;
	}
	return (int)heap;
};

int lib_rnd(int r0, int r1, int r2){
	int y;
	y=g_rnd_seed;
	y = y ^ (y << 13);
	y = y ^ (y >> 17);
	y = y ^ (y << 5);
	g_rnd_seed=y;
	return y&0x7fff;
}

int lib_int(int r0, int r1, int r2){
	g_scratch_int[0]=r0;
	return (int)g_scratch_float[0];
}

int lib_len(int r0, int r1, int r2){
	char* str=(char*)r0;
	for(r0=0;str[r0];r0++);
	return r0;
}

int lib_val(int r0, int r1, int r2){
	char* str=(char*)r0;
	if ('$'==str[0] || '0'==str[0] && ('X'==str[1] || 'x'==str[1])) {
		if ('$'==str[0]) str++;
		else str+=2;
		return strtol(str,0,16);
	}
	return strtol(str,0,10);
}

int lib_strncmp(int r0, int r1, int r2){
	return strncmp((char*)r2,(char*)r1,r0);
}

int lib_float(int r0, int r1, int r2){
	g_scratch_float[0]=(float)r0;
	return g_scratch_int[0];
}

int lib_val_float(int r0, int r1, int r2){
	g_scratch_float[0]=strtof((const char*)r0,0);
	return g_scratch_int[0];
}

int lib_math(int r0, int r1, int r2){
	float f0,f1;
	g_scratch_int[0]=r0;
	g_scratch_int[1]=r1;
	f0=g_scratch_float[0];
	f1=g_scratch_float[1];
	switch(r2){
		case MATH_ACOS: g_scratch_float[0]=acosf(f0); break;
		case MATH_ASIN: g_scratch_float[0]=asinf(f0); break;
		case MATH_ATAN: g_scratch_float[0]=atanf(f0); break;
		case MATH_ATAN2: g_scratch_float[0]=atan2f(f1,f0); break;
		case MATH_CEIL: g_scratch_float[0]=ceilf(f0); break;
		case MATH_COS: g_scratch_float[0]=cosf(f0); break;
		case MATH_COSH: g_scratch_float[0]=coshf(f0); break;
		case MATH_EXP: g_scratch_float[0]=expf(f0); break;
		case MATH_FABS: g_scratch_float[0]=fabsf(f0); break;
		case MATH_FLOOR: g_scratch_float[0]=floorf(f0); break;
		case MATH_FMOD: g_scratch_float[0]=fmodf(f1,f0); break;
		case MATH_LOG: g_scratch_float[0]=logf(f0); break;
		case MATH_LOG10: g_scratch_float[0]=log10f(f0); break;
		case MATH_MODF: g_scratch_float[0]=modff(f0,(void*)&g_scratch[0]); break;
		case MATH_POW: g_scratch_float[0]=powf(f1,f0); break;
		case MATH_SIN: g_scratch_float[0]=sinf(f0); break;
		case MATH_SINH: g_scratch_float[0]=sinhf(f0); break;
		case MATH_SQRT: g_scratch_float[0]=sqrtf(f0); break;
		case MATH_TAN: g_scratch_float[0]=tanf(f0); break;
		case MATH_TANH: g_scratch_float[0]=tanhf(f0); break;
		default: g_scratch_float[0]=0; break;
	}
	return g_scratch_int[0];
}

int lib_mid(int r0, int r1, int r2){
	int i;
	char* str=(char*)kmbasic_variables[r2-1]; // Need to decrement vn (r2) to support the variable 'A'. See mid_string().
	char* str2;
	// Count the number of characters
	for(i=0;str[i];i++);
	if (r1<0) {
		// Shift the initial position
		if ((-r1)<=i) str+=i+r1;
	} else {
		// Shift the initial position
		if (r1<=i) str+=r1;
	}
	// If the 2nd parameter is omitted, return
	if (r0<0) return (int)str;
	// Count again
	for(i=0;str[i];i++);
	// It the 2nd parameter is more than the string length, return
	if (i<=r0) return (int)str;
	// Copy the part of string
	str2=alloc_memory((r0+4)/4,-1);
	for(i=0;i<r0;i++) str2[i]=str[i];
	str2[i]=0;
	// Return
	return (int)str2;
}

int lib_chr(int r0, int r1, int r2){
	char* res;
	res=alloc_memory(1,-1);
	res[0]=r0;
	res[1]=0;
	return (int)res;
}

int lib_dec(int r0, int r1, int r2){
	char* res;
	int i;
	res=alloc_memory(8,-1);
	i=snprintf(res,32,"%d",r0);
	// Adjust the size of memory
	kmbasic_var_size[g_last_var_num]=(i+4)/4;
	return (int)res;
}

int lib_float_str(int r0, int r1, int r2){
	char* res;
	int i;
	g_scratch_int[0]=r0;
	res=alloc_memory(4,-1);
	i=snprintf(res,16,"%g",g_scratch_float[0]);
	// Adjust the size of memory
	kmbasic_var_size[g_last_var_num]=(i+4)/4;
	return (int)res;
}

int lib_sprintf(int r0, int r1, int r2){
	char* res;
	int i;
	g_scratch_int[0]=r0;
	res=alloc_memory(8,-1);
	i=snprintf(res,32,(char*)r1,g_scratch_float[0]);
	// Adjust the size of memory
	kmbasic_var_size[g_last_var_num]=(i+4)/4;
	return (int)res;
}

int debug(int r0, int r1, int r2){
	return r0;
}

static const void* lib_list1[]={
	lib_calc,       // #define LIB_CALC 0
	lib_calc_float, // #define LIB_CALC_FLOAT 1
	lib_hex,        // #define LIB_HEX 2
	lib_add_string, // #define LIB_ADD_STRING 3
	lib_strncmp,    // #define LIB_STRNCMP 4
	lib_val,        // #define LIB_VAL 5
	lib_len,        // #define LIB_LEN 6
	lib_int,        // #define LIB_INT 7
	lib_rnd,        // #define LIB_RND 8
	lib_float,      // #define LIB_FLOAT 9
	lib_val_float,  // #define LIB_VAL_FLOAT 10
	lib_math,       // #define LIB_MATH 11
	lib_mid,        // #define LIB_MID 12
	lib_chr,        // #define LIB_CHR 13
	lib_dec,        // #define LIB_DEC 14
	lib_float_str,  // #define LIB_FLOAT_STRING 15
	lib_sprintf,    // #define LIB_SPRINTF 16
};

static const void* lib_list2[]={
	debug,          // #define LIB_DEBUG 128
	lib_print,      // #define LIB_PRINT 129
	lib_let_str,    // #define LIB_LET_STR 130
	lib_end,        // #define LIB_END 131
	lib_line_num,   // #define LIB_LINE_NUM 132
	lib_dim,        // #define LIB_DIM 133
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
