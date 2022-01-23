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

/*
KM-BASIC for ARM
Compiling CLASS1.BAS
Compiling CLASS2.BAS
Compiling main.bas
14138 micro seconds spent for compiling

kmbasic_object:20002E48
F000 F826 231E 47C0 F000 F803 231F 47C0 BD00 B500 6F28 B081 9000 2001 9900 1808 B001 6728 2300 68BA 8713 2383 47C0 0000 0003 0000 001B 0012 001C 0002 0103 0011 2E78 2000 0000 0000 0000 0000 2E5C 2000 F000 F828 231E 47C0 F000 F803 231F 47C0 BD00 B500 6FA8 B081 9000 2001 9900 1808 B001 67A8 2300 68BA 8793 6FA8 BD00 2383 47C0 0000 0003 0000 001C 0002 001B 0012 0103 0011 2ECC 2000 0000 0000 0000 0000 2EAC 2000 4800 E001 0102 0000 231B 47C0 63A8 2300 68BA 8393 4800 E001 0106 0000 231B 47C0 63E8 2300 68BA 83D3 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F8FD F7FF FFFB 0031 2315 47C0 6876 B003 6BA8 4900 E001 0103 0000 231D 47C0 2380 47C0 B081 9000 B083 9601 466E 6030 2000 60B0 9803 2380 47C0 E002 B500 4780 46C0 F7FF FFFB 0031 2315 47C0 6876 B003 B001 2120 2381 47C0 6BE8 4901 E002 46C0 0103 0000 231D 47C0 2380 47C0 B081 9000 B083 9601 466E 6030 2000 60B0 9803 2380 47C0 E002 B500 4780 46C0 F7FF FFFB 0031 2315 47C0 6876 B003 B001 2100 2381 47C0 6EA8 2120 2381 47C0 6F68 2120 2381 47C0 6BA8 211B 231C 47C0 6800 2120 2381 47C0 6BE8 211B 231C 47C0 6800 2100 2381 47C0 6BA8 211B 231C 47C0 B401 207B BC02 6008 6BE8 211B 231C 47C0 B401 4801 E002 46C0 01C8 0000 BC02 6008 20EA 66A8 2300 68BA 8693 4801 E002 46C0 0237 0000 6768 2300 68BA 8753 6BA8 4901 E002 46C0 0103 0000

END: 2383 47C0
call_lib_code(LIB_OBJ_FIELD): 231C 47C0

Class ID list
0106 0102
Class list
20002ECC  20002E78
Class structures
20002ECC: 0002001C  0012001B  00110103
20002E78: 0012001B  0002001C  00110103

Field/method name list
0103 TEST4
001C TEST3
001B TEST2
001A TEST
Program will start.
20002E78        0               0
20002ECC        0               0
20002E5C20002E5C

20002e48 F000 F826 BL +38 (20002e98)
20002e4c 231E MOVS R3, #0x1e (LIB_PRE_METHOD)
20002e4e 47C0 BLX R8
20002e50 F000 F803 BL +3 (20002e5a)
20002e54 231F MOVS R3, #0x1f (LIB_POST_METHOD)
20002e56 47C0 BLX R8
20002e58 BD00 POP {PC,}
20002e5a B500 PUSH {LR,}
20002e5c 6F28 LDR R0,[R5,#28]
20002e5e B081 SUB SP,SP,#4
20002e60 9000 STR R0,[SP,#0]
20002e62 2001 MOVS R0, #0x1
20002e64 9900 LDR R1,[SP,#0]
20002e66 1808 inst1800
20002e68 B001 ADD SP,SP,#4
20002e6a 6728 STR R0,[R5,#28]
20002e6c 2300 MOVS R3, #0x0
20002e6e 68BA LDR R2,[R7,#2]
20002e70 8713 STRH R3,[R2,#28]
20002e72 2383 MOVS R3, #0x83 (LIB_END)
20002e74 47C0 BLX R8
20002e76 0000 MOVS R0,R0
20002e78 0003 MOVS R3,R0
20002e7a 0000 MOVS R0,R0
20002e7c 001B MOVS R3,R3
20002e7e 0012 MOVS R2,R2
20002e80 001C MOVS R4,R3
20002e82 0002 MOVS R2,R0
20002e84 0103 inst0000
20002e86 0011 MOVS R1,R2
20002e88 2E78 inst2C00
20002e8a 2000 MOVS R0, #0x0
20002e8c 0000 MOVS R0,R0
20002e8e 0000 MOVS R0,R0
20002e90 0000 MOVS R0,R0
20002e92 0000 MOVS R0,R0
20002e94 2E5C inst2C00
20002e96 2000 MOVS R0, #0x0
20002e98 F000 F828 BL +40 (20002eec)
20002e9c 231E MOVS R3, #0x1e (LIB_PRE_METHOD)
20002e9e 47C0 BLX R8
20002ea0 F000 F803 BL +3 (20002eaa)
20002ea4 231F MOVS R3, #0x1f (LIB_POST_METHOD)
20002ea6 47C0 BLX R8
20002ea8 BD00 POP {PC,}
20002eaa B500 PUSH {LR,}
20002eac 6FA8 LDR R0,[R5,#30]
20002eae B081 SUB SP,SP,#4
20002eb0 9000 STR R0,[SP,#0]
20002eb2 2001 MOVS R0, #0x1
20002eb4 9900 LDR R1,[SP,#0]
20002eb6 1808 inst1800
20002eb8 B001 ADD SP,SP,#4
20002eba 67A8 STR R0,[R5,#30]
20002ebc 2300 MOVS R3, #0x0
20002ebe 68BA LDR R2,[R7,#2]
20002ec0 8793 STRH R3,[R2,#30]
20002ec2 6FA8 LDR R0,[R5,#30]
20002ec4 BD00 POP {PC,}
20002ec6 2383 MOVS R3, #0x83 (LIB_END)
20002ec8 47C0 BLX R8
20002eca 0000 MOVS R0,R0
20002ecc 0003 MOVS R3,R0
20002ece 0000 MOVS R0,R0
20002ed0 001C MOVS R4,R3
20002ed2 0002 MOVS R2,R0
20002ed4 001B MOVS R3,R3
20002ed6 0012 MOVS R2,R2
20002ed8 0103 inst0000
20002eda 0011 MOVS R1,R2
20002edc 2ECC inst2C00
20002ede 2000 MOVS R0, #0x0
20002ee0 0000 MOVS R0,R0
20002ee2 0000 MOVS R0,R0
20002ee4 0000 MOVS R0,R0
20002ee6 0000 MOVS R0,R0
20002ee8 2EAC inst2C00
20002eea 2000 MOVS R0, #0x0
20002eec 4800 E001 0102 0000 LDR R0,0x102
20002ef4 231B MOVS R3, #0x1b (LIB_NEW)
20002ef6 47C0 BLX R8
20002ef8 63A8 STR R0,[R5,#14]
20002efa 2300 MOVS R3, #0x0
20002efc 68BA LDR R2,[R7,#2]
20002efe 8393 STRH R3,[R2,#14]
20002f00 4800 E001 0106 0000 LDR R0,0x106
20002f08 231B MOVS R3, #0x1b (LIB_NEW)
20002f0a 47C0 BLX R8
20002f0c 63E8 STR R0,[R5,#15]
20002f0e 2300 MOVS R3, #0x0
20002f10 68BA LDR R2,[R7,#2]
20002f12 83D3 STRH R3,[R2,#15]
20002f14 6830 LDR R0,[R6,#0]
20002f16 B083 SUB SP,SP,#12
20002f18 9601 STR R6,[SP,#4]
20002f1a 466E MOV R6,R13
20002f1c 6030 STR R0,[R6,#0]
20002f1e 2000 MOVS R0, #0x0
20002f20 60B0 STR R0,[R6,#2]
20002f22 F000 F803 BL +3 (20002f2c)
20002f26 B500 PUSH {LR,}
20002f28 F000 F8FD BL +253 (20003126)
20002f2c F7FF FFFB BL -5 (20802f26)
20002f30 0031 MOVS R1,R6
20002f32 2315 MOVS R3, #0x15 (LIB_POST_GOSUB)
20002f34 47C0 BLX R8
20002f36 6876 LDR R6,[R6,#1]
20002f38 B003 ADD SP,SP,#12
20002f3a 6BA8 LDR R0,[R5,#14]
20002f3c 4900 LDR R1,[PC+#0]
20002f3e E001 B 2
20002f40 0103 inst0000
20002f42 0000 MOVS R0,R0
20002f44 231D MOVS R3, #0x1d (LIB_OBJ_METHOD)
20002f46 47C0 BLX R8
20002f48 2380 MOVS R3, #0x80 (LIB_DEBUG)
20002f4a 47C0 BLX R8
20002f4c B081 SUB SP,SP,#4
20002f4e 9000 STR R0,[SP,#0]
20002f50 B083 SUB SP,SP,#12
20002f52 9601 STR R6,[SP,#4]
20002f54 466E MOV R6,R13
20002f56 6030 STR R0,[R6,#0]
20002f58 2000 MOVS R0, #0x0
20002f5a 60B0 STR R0,[R6,#2]
20002f5c 9803 LDR R0,[SP,#12]
20002f5e 2380 MOVS R3, #0x80 (LIB_DEBUG)
20002f60 47C0 BLX R8
20002f62 E002 B 4
20002f64 B500 PUSH {LR,}
20002f66 4780 BLX R0
20002f68 46C0 MOV R8,R8
20002f6a F7FF FFFB BL -5 (20802f64)
20002f6e 0031 MOVS R1,R6
20002f70 2315 MOVS R3, #0x15 (LIB_POST_GOSUB)
20002f72 47C0 BLX R8
20002f74 6876 LDR R6,[R6,#1]
20002f76 B003 ADD SP,SP,#12
20002f78 B001 ADD SP,SP,#4
20002f7a 2120 MOVS R1, #0x20
20002f7c 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f7e 47C0 BLX R8
20002f80 6BE8 LDR R0,[R5,#15]
20002f82 4901 LDR R1,[PC+#1]
20002f84 E002 B 4
20002f86 46C0 MOV R8,R8
20002f88 0103 inst0000
20002f8a 0000 MOVS R0,R0
20002f8c 231D MOVS R3, #0x1d (LIB_OBJ_METHOD)
20002f8e 47C0 BLX R8
20002f90 2380 MOVS R3, #0x80 (LIB_DEBUG)
20002f92 47C0 BLX R8
20002f94 B081 SUB SP,SP,#4
20002f96 9000 STR R0,[SP,#0]
20002f98 B083 SUB SP,SP,#12
20002f9a 9601 STR R6,[SP,#4]
20002f9c 466E MOV R6,R13
20002f9e 6030 STR R0,[R6,#0]
20002fa0 2000 MOVS R0, #0x0
20002fa2 60B0 STR R0,[R6,#2]
20002fa4 9803 LDR R0,[SP,#12]
20002fa6 2380 MOVS R3, #0x80 (LIB_DEBUG)
20002fa8 47C0 BLX R8
20002faa E002 B 4
20002fac B500 PUSH {LR,}
20002fae 4780 BLX R0
20002fb0 46C0 MOV R8,R8
20002fb2 F7FF FFFB BL -5 (20802fac)
20002fb6 0031 MOVS R1,R6
20002fb8 2315 MOVS R3, #0x15 (LIB_POST_GOSUB)
20002fba 47C0 BLX R8
20002fbc 6876 LDR R6,[R6,#1]
20002fbe B003 ADD SP,SP,#12
20002fc0 B001 ADD SP,SP,#4
20002fc2 2100 MOVS R1, #0x0
20002fc4 2381 MOVS R3, #0x81 (LIB_PRINT)
20002fc6 47C0 BLX R8
20002fc8 6EA8 LDR R0,[R5,#26]
20002fca 2120 MOVS R1, #0x20
20002fcc 2381 MOVS R3, #0x81 (LIB_PRINT)
20002fce 47C0 BLX R8
20002fd0 6F68 LDR R0,[R5,#29]
20002fd2 2120 MOVS R1, #0x20
20002fd4 2381 MOVS R3, #0x81 (LIB_PRINT)
20002fd6 47C0 BLX R8
20002fd8 6BA8 LDR R0,[R5,#14]
20002fda 211B MOVS R1, #0x1b
20002fdc 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002fde 47C0 BLX R8
20002fe0 6800 LDR R0,[R0,#0]
20002fe2 2120 MOVS R1, #0x20
20002fe4 2381 MOVS R3, #0x81 (LIB_PRINT)
20002fe6 47C0 BLX R8
20002fe8 6BE8 LDR R0,[R5,#15]
20002fea 211B MOVS R1, #0x1b
20002fec 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002fee 47C0 BLX R8
20002ff0 6800 LDR R0,[R0,#0]
20002ff2 2100 MOVS R1, #0x0
20002ff4 2381 MOVS R3, #0x81 (LIB_PRINT)
20002ff6 47C0 BLX R8
20002ff8 6BA8 LDR R0,[R5,#14]
20002ffa 211B MOVS R1, #0x1b
20002ffc 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002ffe 47C0 BLX R8
20003000 B401 PUSH {R0,}
20003002 207B MOVS R0, #0x7b
20003004 BC02 POP {R1,}
20003006 6008 STR R0,[R1,#0]
20003008 6BE8 LDR R0,[R5,#15]
2000300a 211B MOVS R1, #0x1b
2000300c 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
2000300e 47C0 BLX R8
20003010 B401 PUSH {R0,}
20003012 4801 E002 46C0 01C8 0000 LDR R0,0x1c8
2000301c BC02 POP {R1,}
2000301e 6008 STR R0,[R1,#0]
20003020 20EA MOVS R0, #0xea
20003022 66A8 STR R0,[R5,#26]
20003024 2300 MOVS R3, #0x0
20003026 68BA LDR R2,[R7,#2]
20003028 8693 STRH R3,[R2,#26]
2000302a 4801 E002 46C0 0237 0000 LDR R0,0x237
20003034 6768 STR R0,[R5,#29]
20003036 2300 MOVS R3, #0x0
20003038 68BA LDR R2,[R7,#2]
2000303a 8753 STRH R3,[R2,#29]
2000303c 6BA8 LDR R0,[R5,#14]
2000303e 4901 LDR R1,[PC+#1]
20003040 E002 B 4
20003042 46C0 MOV R8,R8
20003044 0103 inst0000
20003046 0000 MOVS R0,R0
*/

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
"USECLASS CLASS1,CLASS2" CR
"O=NEW(CLASS1)" CR
"P=NEW(CLASS2)" CR
"GOSUB SUB" CR
"PRINT O.TEST4(),P.TEST4()" CR
"O.TEST2=123" CR
"P.TEST2=456" CR
"PRINT O.TEST4(),P.TEST4()" CR
"GOSUB SUB" CR
"END" CR
"LABEL SUB" CR
"FOR I=0 TO 3:PRINT HEX$(O(I)),:NEXT" CR
"PRINT" CR
"FOR I=0 TO 3:PRINT HEX$(P(I)),:NEXT" CR
"PRINT" CR
"RETURN" CR
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

#endif // DEBUG_MODE

