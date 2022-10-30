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
#include "./compiler.h"
#include "./api.h"
#include "./display.h"
#include "./config.h"
#include "./sleep.h"

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
	// Garbage collection
	garbage_collection((char*)r1);
	garbage_collection((char*)r0);
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
			if (r0) {
				for(i=0;((unsigned char*)r0)[i];i++);
				printstr((unsigned char*)r0);
				garbage_collection((char*)r0);
			} else {
				i=0;
			}
			if (0x00 == (r1&0xf0)) printstr("\n");
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
		printstr(&("          "[i%10]));
	}
	return r0;
}

void lib_print(){
	use_lib_stack("lib_print_main");
}

void lib_fprint(){
	use_lib_stack("lib_fprint_main");
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
	asm("ldr r7,=kmbasic_data");
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
	garbage_collection(str);
	return r0;
}

int lib_val(int r0, int r1, int r2){
	char* str=(char*)r0;
	if ('$'==str[0] || '0'==str[0] && ('X'==str[1] || 'x'==str[1])) {
		if ('$'==str[0]) str++;
		else str+=2;
		r1 = strtol(str,0,16);
	} else r1=strtol(str,0,10);
	garbage_collection((char*)r0);
	return r1;
}

int lib_strncmp(int r0, int r1, int r2){
	r0=strncmp((char*)r2,(char*)r1,r0);
	garbage_collection((char*)r1);
	garbage_collection((char*)r2);
	return r0;
}

int lib_float(int r0, int r1, int r2){
	g_scratch_float[0]=(float)r0;
	return g_scratch_int[0];
}

int lib_val_float(int r0, int r1, int r2){
	g_scratch_float[0]=strtof((const char*)r0,0);
	garbage_collection((char*)r0);
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

int lib_float_str_main(int r0, int r1, int r2){
	char* res;
	int i;
	g_scratch_int[0]=r0;
	res=alloc_memory(4,-1);
	i=snprintf(res,16,"%g",g_scratch_float[0]);
	// Adjust the size of memory
	kmbasic_var_size[g_last_var_num]=(i+4)/4;
	return (int)res;
}

void lib_float_str(){
	use_lib_stack("lib_float_str_main");
}


int lib_sprintf(int r0, int r1, int r2){
	char* res;
	int i;
	g_scratch_int[0]=r0;
	res=alloc_memory(8,-1);
	i=snprintf(res,32,(char*)r1,g_scratch_float[0]);
	// Adjust the size of memory
	kmbasic_var_size[g_last_var_num]=(i+4)/4;
	// Garbage collection
	garbage_collection((char*)r1);
	return (int)res;
}

unsigned short* seek_data(int mode){
	int i;
	unsigned short* obj=g_read_point;
	while(obj<object) { // Search until the end of compiled object
		switch (obj[0]&0xfc00) {
			case 0xe000:
				// B instruction forward found
				i=obj[0]&0x03ff;
				obj+=i+2;
				continue;
			case 0xf000:
				// BL instruction forward found
				i=obj[0]&0x03ff;
				i=(i<<11)|(obj[1]&0x7ff);
				switch(obj[2]){
					case 0x462d: // DATA
						if (mode!=0x462d) break;
						g_read_mode=obj[2];
						g_read_valid_len=i-1;
						g_read_point=&obj[3];
						return g_read_point;
					case 0x4636: // CDATA, last byte is valid
					case 0x463f: // CDATA, last byte is invalid
						if (mode!=0x4636 && mode!=0x463f) break;
						g_read_mode=obj[2];
						g_read_valid_len=i-1;
						g_read_point=&obj[3];
						return g_read_point;
					default:
						// This is not DATA/CDATA
						obj+=2;
						continue;
				}
				// Skip this DATA/CDATA
				obj+=i+2;
				continue;
			default:
				obj++;
				continue;
		}
	}
	stop_with_error(ERROR_DATA_NOT_FOUND);
}

int lib_read(int r0, int r1, int r2){
	if (g_read_mode!=0x462d || g_read_valid_len<=0) seek_data(0x462d);
	r0=(g_read_point++)[0];
	r0=r0 | ((g_read_point++)[0]<<16);
	g_read_valid_len-=2;
	return r0;
}

int lib_read_str(int r0, int r1, int r2){
	int i;
	char* str;
	if (g_read_mode!=0x462d || g_read_valid_len<=0) seek_data(0x462d);
	str=(char*)g_read_point;
	// Determine the length
	for(i=0;str[i];i++);
	// Calculate for unsigned short* (note that null character at end is also included)
	i=(i+2)/2;
	// Go forward
	g_read_point+=i;
	g_read_valid_len-=i;
	// Return
	return (int)str;
}

int lib_cread(int r0, int r1, int r2){
	static int odd;
	if (g_read_mode!=0x463f && g_read_mode!=0x4636) {
		seek_data(0x4636);
		odd=0;
	} else if (g_read_mode==0x463f && g_read_valid_len<=0) {
		seek_data(0x4636);
		odd=0;
	} else if (g_read_mode==0x4636 && g_read_valid_len<=0 && 0==odd) {
		seek_data(0x4636);
		odd=0;
	}
	if (odd){
		odd=0;
		return (g_read_point++)[0]>>8;
	} else {
		g_read_valid_len--;
		odd=1;
		return g_read_point[0]&0x00ff;
	}
}

int lib_restore(int r0, int r1, int r2){
	int i;
	if (0xf0000000 == (0xf0000000 & r0)) {
		// Calculate address from BL instruction and pc
		i=(r0>>16)&0x7ff;
		i|=(r0&0x3ff)<<11;
		if (r0&0x0400) i|=0xffe00000; // Negative value
		r0=r1+i*2-4;
	}
	// r0 is the address
	g_read_point=(unsigned short*)r0;
	g_read_mode=0;
	g_read_valid_len=0;
	return (int)g_read_point;
}

int lib_asc(int r0, int r1, int r2){
	unsigned char* str=(unsigned char*)r0;
	r0=str[0];
	garbage_collection(str);
	return r0;
}

int lib_post_gosub(int r0, int r1, int r2){
	// r1 is pointer to r6 array, that contains argument data
	// r0 must retain after this function
	int i;
	int* r6=(int*)r1;
	int num=r6[2];
	// Garbage collection
	for(i=0;i<num;i++) garbage_collection((void*)r6[3+i]);
	return r0;
}

int lib_var_push(int r0, int r1, int r2){
	// r1 is the pointer to array containing variable data
	// r1[0]; Lower half: variable number; Upper half: permanent block to store string data
	// r1[1]; Variable value
	// r2 is number of variables to push
	int i,vn,on;
	int* stack=(int*)r1;
	for(i=0;i<r2;i++){
		vn=stack[i*2]&0xffff;
		stack[i*2+1]=kmbasic_variables[vn];
		// Check if object
		if (0<kmbasic_var_size[vn]) {
			// This is object
			on=get_permanent_block_number();
			kmbasic_variables[on]=kmbasic_variables[vn];
			kmbasic_var_size[on]=kmbasic_var_size[vn];
			stack[i*2]|=on<<16;
		}
		// Clear the variable
		kmbasic_variables[vn]=0;
		kmbasic_var_size[vn]=0;
	}
	return r0;
}

int lib_var_pop(int r0, int r1, int r2){
	// r1 is the pointer to array containing variable data
	// r1[0]; Lower half: variable number; Upper half: permanent block to store string data
	// r1[1]; Variable value
	// r2 is number of variables to push
	int i,vn,on;
	int* stack=(int*)r1;
	for(i=0;i<r2;i++){
		vn=stack[i*2]&0xffff;
		on=(stack[i*2]>>16)&0xffff;
		if (0<on) {
			// This is object
			kmbasic_variables[vn]=kmbasic_variables[on];
			kmbasic_var_size[vn]=kmbasic_var_size[on];
			// Delete stored object
			kmbasic_var_size[on]=0;
		} else {
			// This is simple value
			kmbasic_variables[vn]=stack[i*2+1];
			kmbasic_var_size[vn]=0;
		}
	}
	return r0;
}

int lib_wait(int r0, int r1, int r2){
	unsigned short n=(unsigned short)r0;
	uint64_t t=to_us_since_boot(get_absolute_time())%16667;
	sleep_us(16667*n-t);
	return r0;
}

int lib_delayus(int r0, int r1, int r2){
	// At least 449 CPU cycles (3.592 micro seconds) are required to call this function.
	r1=r0-3;
	if (0<r1) sleep_us(r1);
	return r0;
}

int lib_delayms(int r0, int r1, int r2){
	sleep_ms(r0);
	return r0;
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

int lib_str2obj(int r0, int r1, int r2){
	int i,j;
	char* str2;
	char* str=(char*)r0;
	// Count character number
	for(i=0;str[i];i++);
	// Get permanent block number
	j=get_permanent_block_number();
	// Get object area
	str2=alloc_memory((i+4)/4,j);
	memcpy(str2,str,i+1);
	// Garbage collection
	garbage_collection(str);
	return (int)str2;
}

int lib_debug(int r0, int r1, int r2){
#ifdef DEBUG_MODE
	//M0PLUS 0xe0000000
	//SYST_CVR 0xe018
	//asm("ldr	r0, [r5, r0]");
	//printhex32(r0);
	//lib_wait(60,0,0);
	return r0;
#else
	return r0+1;
#endif
}

extern unsigned char TVRAM[];
extern unsigned char *cursor;
extern unsigned char cursorcolor;
extern unsigned char *fontp;
extern const unsigned char FontData[256*8];

int lib_system(int r0, int r1, int r2){
	switch(r0){
		case 0:
		//	MachiKania バージョン文字列、"Zoea"等を返す。
			return (int)SYSVER1;
		case 1:
		//	MachiKania バージョン文字列、"1.2"等を返す。
			return (int)SYSVER2;
		case 2:
		//	BASIC バージョン文字列、"KM-1208"等を返す。
			return (int)BASVER;
		case 3:
		//	現在実行中のHEXファイル名、"ZOEA.HEX"等を返す。
			return (int)"";
		case 4:
		//	現在実行中のCPUのクロック周波数を返す。
			return 125000000;
		case 20:
		//	キャラクターディスプレイ横幅を返す。
			return WIDTH_X;
		case 21:
		//	キャラクターディスプレイ縦幅を返す。
			return WIDTH_Y;
		case 22:
		//	グラフィックディスプレイ横幅を返す。
			return WIDTH_X*8;
		case 23:
		//	グラフィックディスプレイ横幅を返す。
			return WIDTH_Y*8;
		case 24:
		//	キャラクターディスプレイ用の指定色を返す。
			return (unsigned char)lib_display(0,0,0);
		case 25:
		//	グラフィックディスプレイ用の指定色を返す。
			return (unsigned char)lib_display(1,0,0);
		case 26:
		//	キャラクターディスプレイの、現在のX位置を返す。
			return ((int)cursor-(int)&TVRAM[0])%WIDTH_X;
		case 27:
		//	キャラクターディスプレイの、現在のY位置を返す。
			return ((int)cursor-(int)&TVRAM[0])/WIDTH_X;
		case 28:
		//	グラフィックディスプレイの、現在のX位置を返す。
			return lib_display(3,0,0);
		case 29:
		//	グラフィックディスプレイの、現在のY位置を返す。
			return lib_display(4,0,0);
		case 40:
		//	PS/2キーボードを使用中かどうかを返す。
			return 0;
		case 41:
		//	PS/2キーボード情報、vkeyを返す。
			return 0;
		case 42:
		//	PS/2キーボード情報、lockkeyを返す。
			return 0;
		case 43:
		//	PS/2キーボード情報、keytypeを返す。
			return 0;
		case 100:
		//	変数格納領域(g_var_mem)へのポインターを返す。
			return (int)&kmbasic_variables[0];
		case 101:
		//	乱数シードへのポインターを返す。
			return (int)&g_rnd_seed;
		case 102:
		//	キャラクターディスプレイ領域(TVRAM)へのポインターを返す。
			return (int)&TVRAM[0];
		case 103:
		//	フォント領域へのポインターを返す。
			return (int)&FontData[0];
		case 104:
		//	PCGフォント領域へのポインターを返す。
			return (int)fontp;
		case 105:
		//	グラフィックディスプレイ領域へのポインターを返す。
			return 0;
		case 200:
		//	ディスプレイの表示を停止(xが0のとき)、もしくは開始(xが0以外の時)する。
			break;
		case 250:
		// void* calloc (int bytes);
			return (int)calloc_memory((r0+3)/4,get_permanent_block_number());		
		case 251:
		// void* malloc (int bytes);
			return (int)alloc_memory((r0+3)/4,get_permanent_block_number());		
		case 252:
		// void free (void* addr);
			delete_memory((void*)r0);
			break;
		case 253:
		// Garbage collection
			garbage_collection((void*)r0);
			break;
		default:
			break;
	}
	return 0;
}

static const void* lib_list1[]={
	lib_calc,                   // #define LIB_CALC 0
	lib_calc_float,             // #define LIB_CALC_FLOAT 1
	lib_hex,                    // #define LIB_HEX 2
	lib_add_string,             // #define LIB_ADD_STRING 3
	lib_strncmp,                // #define LIB_STRNCMP 4
	lib_val,                    // #define LIB_VAL 5
	lib_len,                    // #define LIB_LEN 6
	lib_int,                    // #define LIB_INT 7
	lib_rnd,                    // #define LIB_RND 8
	lib_float,                  // #define LIB_FLOAT 9
	lib_val_float,              // #define LIB_VAL_FLOAT 10
	lib_math,                   // #define LIB_MATH 11
	lib_mid,                    // #define LIB_MID 12
	lib_chr,                    // #define LIB_CHR 13
	lib_dec,                    // #define LIB_DEC 14
	lib_float_str,              // #define LIB_FLOAT_STRING 15
	lib_sprintf,                // #define LIB_SPRINTF 16
	lib_read,                   // #define LIB_READ 17
	lib_cread,                  // #define LIB_CREAD 18
	lib_read_str,               // #define LIB_READ_STR 19
	lib_asc,                    // #define LIB_ASC 20
	lib_post_gosub,             // #define LIB_POST_GOSUB 21
	lib_display,                // #define LIB_DISPLAY_FUNCTION 22
	lib_inkey,                  // #define LIB_INKEY 23
	lib_input,                  // #define LIB_INPUT 24
	lib_timer,                  // #define LIB_TIMER 32
	lib_keys,                   // #define LIB_KEYS 26
	lib_new,                    // #define LIB_NEW 27
	lib_resolve_field_address,  // #define LIB_OBJ_FIELD 28
	lib_resolve_method_address, // #define LIB_OBJ_METHOD 29
	lib_pre_method,             // #define LIB_PRE_METHOD 30
	lib_post_method,            // #define LIB_POST_METHOD 31
};

static const void* lib_list2[]={
	lib_debug,      // #define LIB_DEBUG 128
	lib_print,      // #define LIB_PRINT 129
	lib_let_str,    // #define LIB_LET_STR 130
	lib_end,        // #define LIB_END 131
	lib_line_num,   // #define LIB_LINE_NUM 132
	lib_dim,        // #define LIB_DIM 133
	lib_restore,    // #define LIB_RESTORE 134
	lib_var_push,   // #define LIB_VAR_PUSH 135
	lib_var_pop,    // #define LIB_VAR_POP 136
	lib_display,    // #define LIB_DISPLAY 137
	lib_wait,       // #define LIB_WAIT 138
	lib_system,     // #define LIB_SYSTEM 139
	lib_str2obj,    // #define LIB_STR_TO_OBJECT 140
	lib_delete,     // #define LIB_DELETE 141
	lib_file,       // #define LIB_FILE 142
	lib_fopen,      // #define LIB_FOPEN 143
	lib_fprint,     // #define LIB_FPRINT 144
	lib_interrupt,  // #define LIB_INTERRUPT 145
	lib_pwm,        // #define LIB_PWM 146
	lib_analog,     // #define LIB_ANALOG 147
	lib_spi,        // #define LIB_SPI 148
	lib_i2c,        // #define LIB_I2C 149
	lib_serial,     // #define LIB_SERIAL 150
	lib_gpio,       // #define LIB_GPIO 151
	lib_music,      // #define LIB_MUSIC 152
	lib_delayus,    // #define LIB_DELAYUS 153
	lib_delayms,    // #define LIB_DELAYMS 154
};

int statement_library(int r0, int r1, int r2, int r3){
	int (*f)(int r0, int r1, int r2) = lib_list2[r3-128];
	r0=f(r0,r1,r2);
	// Raise garbage collection flag
	// g_garbage_collection=1; // This feature is disabled. See galbage_collection() function.
	// Check break key (Ctrl-Z)
	if (getchar_timeout_us(0)==0x1a) return lib_end(0,0,0);
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
