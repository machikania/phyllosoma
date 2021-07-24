/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"

#ifndef DEBUG_MODE
void debug_dummy(void){}
#else // DEBUG_MODE

unsigned char* debug_fileselect(void){
	// Wait for total three seconds
	sleep_ms(2500);
	// Return file name to compile
	return "main.bas";
}

#define CR "\n"
static const char* debug_files[]={
	"main.bas",
"USEVAR BALLX,BALLY,BALLVX,BALLVY,BALLR" CR
"USEVAR WALLX1,WALLX2,WALLY1,WALLY2" CR
"USEVAR L1X1,L1Y1,L1X2,L1Y2,L1DX,L1DY,L1D" CR
"PRINT L1X1" CR
"L1X1=10*256" CR
"PRINT L1X1" CR
"END" CR
	,0
};

void dump(void){
	int i;
	printstr("\nkmbasic_object:");
	printhex32((int)&kmbasic_object[0]);
	printstr("\n");
	for(i=0;i<256;i++) {
		printhex16(kmbasic_object[i]);
		printchar(' ');
		if (0x0000==kmbasic_object[i] && 0x0000==kmbasic_object[i+1] && 0x0000==kmbasic_object[i+2]) break;
	}
	printstr("\n\n");
	sleep_ms(1);
}

void dump_cmpdata(void){
	int* data;
	unsigned int i,num;
	printstr("\nCMPDATA dump\n");
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_ALL)){
		num=(data[0]>>16)&0xff;
		for(i=0;i<num;i++){
			printhex32(data[i]);
			printstr(" ");
		}
		printstr("\n");
	}
}

void dump_variables(void){
	int i;
	printstr("dump variables\n");
	for(i=0;i<26;i++){
		printchar('A'+i);
		printchar(' ');
		printhex32(kmbasic_variables[i]);
		printchar(' ');
		printhex16(kmbasic_var_size[i]);
		printchar(' ');
	}
	printchar('\n');
}

FRESULT debug_f_open (FIL* fp, const TCHAR* path, BYTE mode){
	int i;
	const TCHAR* file;
	for(i=0;file=debug_files[i];i+=2){
		if (strcmp(file,path)) continue;
		// Found the file
		file=debug_files[i+1];
		fp->dir_ptr=(TCHAR*)file;
		fp->fptr=0;
		// Count the file size
		for(i=0;file[i];i++);
		fp->obj.objsize=i;
		// All done
		return FR_OK;
	}
	return FR_NO_FILE;
}

FRESULT debug_f_close (FIL* fp){
	fp->dir_ptr=0;
	fp->fptr=0;
	fp->obj.objsize=0;
	return FR_OK;
}

TCHAR* debug_f_gets (TCHAR* buff, int len, FIL* fp){
	int i;
	unsigned char c;
	TCHAR* file=(TCHAR*)fp->dir_ptr;
	for(i=0;i<len-1;i++){
		if (f_eof(fp)) break;
		c=buff[i]=file[fp->fptr++];
		if (0x0d==c && 0x0a==file[fp->fptr]) {
			buff[i+1]=file[fp->fptr++];
			i+=2;
			break;
		} else if (0x00==c || 0x0d==c || 0x0a==c) {
			i++;
			break;
		}
	}
	buff[i]=0;
	return buff;
}

#endif // DEBUG_MODE

