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

#warning DEBOG_MODE is active

// Local protptypes
void dump_class_list(void);
void dump_fieldnames(void);
void dump_cmpdata(void);
void dump_variables(void);

unsigned char* debug_fileselect(void){
	// Wait for total three seconds
	sleep_ms(2500);
	// Return file name to compile
	return "main.bas";
}

#define CR "\n"
static const char* debug_files[]={
	"main.bas",
"USEVAR CA,CB,CT" CR
"CT=CORETIMER()" CR
"FOR Y=-12 to 12" CR
" FOR X=-39 to 39" CR
"  CA#=FLOAT#(X)*0.0458" CR
"  CB#=FLOAT#(Y)*0.08333" CR
"  A#=CA#" CR
"  B#=CB#" CR
"  FOR I=0 TO 15" CR
"   T#=A#*A#-B#*B#+CA#" CR
"   B#=2*A#*B#+CB#" CR
"   A#=T#" CR
"   IF INT(A#*A#+B#*B#-4)>=0 THEN BREAK" CR
"  NEXT" CR
"  IF INT(A#*A#+B#*B#-4)>=0 THEN" CR
"   IF I>9 THEN I=I+7" CR
"   PRINT CHR$(48+I);" CR
"  ELSE" CR
"   PRINT \" \";" CR
"  ENDIF" CR
" NEXT" CR
" PRINT" CR
"NEXT" CR
"PRINT CORETIMER()-CT" CR
"DO:LOOP" CR
	,"CLASS1.BAS",
"STATIC TEST" CR
"FIELD  TEST2" CR
"FIELD PRIVATE TEST3" CR
"METHOD TEST4" CR
"  TEST3=TEST3+1" CR
"RETURN TEST3" CR
"END" CR
	,"CLASS2.BAS",
"STATIC TEST" CR
"FIELD PRIVATE TEST3" CR
"FIELD  TEST2" CR
"METHOD TEST4" CR
"  TEST3=TEST3+2" CR
"RETURN TEST3" CR
"METHOD INIT" CR
"  TEST3=ARGS(1)" CR
"RETURN" CR
"END" CR
	,0
};

void dump(void){
	int i;
	printstr("\nkmbasic_object:");
	printhex32((int)&kmbasic_object[0]);
	printstr("\n");
	for(i=0;i<256;i++) {
		if (object<=&kmbasic_object[i]) break;
		printhex16(kmbasic_object[i]);
		printchar(' ');
		
	}
	printstr("\n\n");
	printstr("END: 2383 47C0\n");
	printstr("call_lib_code(LIB_OBJ_FIELD): ");
	printhex16(0x2300 | LIB_OBJ_FIELD);
	printstr(" 47C0\n");
	//dump_cmpdata();
	//dump_variables();
	dump_class_list();
	dump_fieldnames();
	sleep_ms(1);
	printstr("Program will start.\n");
}

void dump_class_list(void){
	int i,j,num;
	int* class_structure;
	printstr("\nClass ID list\n");
	for(i=0;g_class_id_list[i];i++){
		printhex16(g_class_id_list[i]);
		printchar(' ');
	}
	num=i;
	printstr("\nClass list\n");
	for(i=0;i<num;i++){
		printhex32(g_class_list[i]);
		printchar(' ');
		printchar(' ');
	}
	printstr("\nClass structures\n");
	for(i=0;i<num;i++){
		printhex32(g_class_list[i]);
		printchar(':');
		printchar(' ');
		class_structure=(int*)g_class_list[i];
		for(j=1;j<=class_structure[0];j++){
			printhex32(class_structure[j]);
			printchar(' ');
			printchar(' ');
		}
		printstr("\n");
	}
}

void dump_fieldnames(void){
	int* data;
	printstr("\nField/method name list\n");
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_FIELDNAME)){
		printhex16(data[0]);
		printchar(' ');
		printstr((char*)(&data[2]));
		printchar('\n');
	}
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
	printstr("\ndump variables\n");
	for(i=0;i<30;i++){
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

FRESULT debug_f_getcwd (TCHAR* buff, UINT len){
	if (len<2) return FR_NOT_ENOUGH_CORE;
	buff[0]='/';
	buff[1]=0x00;
	return FR_OK;
}

FRESULT debug_f_chdir (const TCHAR* path){
	return FR_OK;
}

#endif // DEBUG_MODE

