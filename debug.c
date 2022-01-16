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
11013 micro seconds spent for compiling

kmbasic_object:20002E48
F000 F80E 2383 47C0 0002 0000 001B 0012 001C 0002 2E50 2000 0000 0000 0000 0000 F000 F80E 2383 47C0 0002 0000 001C 0002 001B 0012 2E70 2000 0000 0000 0000 0000 4800 E001 0102 0000 231B 47C0 63A8 2300 68BA 8393 4800 E001 0104 0000 231B 47C0 63E8 2300 68BA 83D3 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F86D F7FF FFFB 0031 2315 47C0 6876 B003 6EA8 2120 2381 47C0 6EA8 2120 2381 47C0 6BA8 211B 231C 47C0 6800 2120 2381 47C0 6BE8 211B 231C 47C0 6800 2100 2381 47C0 6BA8 211B 231C 47C0 B401 207B BC02 6008 6BE8 211B 231C 47C0 B401 4800 E001 01C8 0000 BC02 6008 20EA 66A8 2300 68BA 8693 4801 E002 46C0 0237 0000 66A8 2300 68BA 8693 6EA8 2120 2381 47C0 6EA8 2120 2381 47C0 6BA8 211B 231C 47C0 6800 2120 2381 47C0 6BE8 211B 231C 47C0 6800 2100 2381 47C0 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F809 F7FF FFFB 0031 2315 47C0 6876 B003 2383 47C0 2000 6228 2300 68BA 8213 2002 B401 6A28 F000 F808 2002 B401 6A28 3001 6228 2300 68BA 8213 BC02 4281 DA01 F000 F810 6BA8 B401 6A28 0080 BC02 5808 B401 2000 BC02 2302 47C0 2121 2381 47C0 F7FF FFE3 2000 2101 2381 47C0 2000 6228 2300 68BA 8213 2002 B401 6A28 F000 F808 2002 B401 6A28 3001 6228 2300 68BA 8213 BC02 4281 DA01 F000 F810 6BE8 B401 6A28 0080 BC02 5808 B401 2000 BC02 2302 47C0 2121 2381 47C0 F7FF FFE3 2000

END: 2383 47C0
call_lib_code(LIB_OBJ_FIELD): 231C 47C0

Class ID list
0104 0102
Class list
20002E70  20002E50
Class structures
20002E70: 0002001C  0012001B
20002E50: 0012001B  0002001C

Field/method name list
001C TEST3
001B TEST2
001A TEST
Program will start.
20002E50        0               0
20002E70        0               0
0               0               0               0
567             567             123             456
20002E50        7B              0
20002E70        0               1C8

20002e48 F000 F80E BL +14 (20002e68)
20002e4c 2383 MOVS R3, #0x83 (LIB_END)
20002e4e 47C0 BLX R8
20002e50 0002 MOVS R2,R0
20002e52 0000 MOVS R0,R0
20002e54 001B MOVS R3,R3
20002e56 0012 MOVS R2,R2
20002e58 001C MOVS R4,R3
20002e5a 0002 MOVS R2,R0
20002e5c 2E50 inst2C00
20002e5e 2000 MOVS R0, #0x0
20002e60 0000 MOVS R0,R0
20002e62 0000 MOVS R0,R0
20002e64 0000 MOVS R0,R0
20002e66 0000 MOVS R0,R0
20002e68 F000 F80E BL +14 (20002e88)
20002e6c 2383 MOVS R3, #0x83 (LIB_END)
20002e6e 47C0 BLX R8
20002e70 0002 MOVS R2,R0
20002e72 0000 MOVS R0,R0
20002e74 001C MOVS R4,R3
20002e76 0002 MOVS R2,R0
20002e78 001B MOVS R3,R3
20002e7a 0012 MOVS R2,R2
20002e7c 2E70 inst2C00
20002e7e 2000 MOVS R0, #0x0
20002e80 0000 MOVS R0,R0
20002e82 0000 MOVS R0,R0
20002e84 0000 MOVS R0,R0
20002e86 0000 MOVS R0,R0
20002e88 4800 E001 0102 0000 LDR R0,0x102
20002e90 231B MOVS R3, #0x1b (LIB_NEW)
20002e92 47C0 BLX R8
20002e94 63A8 STR R0,[R5,#14]
20002e96 2300 MOVS R3, #0x0
20002e98 68BA LDR R2,[R7,#2]
20002e9a 8393 STRH R3,[R2,#14]
20002e9c 4800 E001 0104 0000 LDR R0,0x104
20002ea4 231B MOVS R3, #0x1b (LIB_NEW)
20002ea6 47C0 BLX R8
20002ea8 63E8 STR R0,[R5,#15]
20002eaa 2300 MOVS R3, #0x0
20002eac 68BA LDR R2,[R7,#2]
20002eae 83D3 STRH R3,[R2,#15]
20002eb0 6830 LDR R0,[R6,#0]
20002eb2 B083 SUB SP,SP,#12
20002eb4 9601 STR R6,[SP,#4]
20002eb6 466E MOV R6,R13
20002eb8 6030 STR R0,[R6,#0]
20002eba 2000 MOVS R0, #0x0
20002ebc 60B0 STR R0,[R6,#2]
20002ebe F000 F803 BL +3 (20002ec8)
20002ec2 B500 PUSH {LR,}
20002ec4 F000 F86D BL +109 (20002fa2)
20002ec8 F7FF FFFB BL -5 (20802ec2)
20002ecc 0031 MOVS R1,R6
20002ece 2315 MOVS R3, #0x15 (LIB_POST_GOSUB)
20002ed0 47C0 BLX R8
20002ed2 6876 LDR R6,[R6,#1]
20002ed4 B003 ADD SP,SP,#12
20002ed6 6EA8 LDR R0,[R5,#26]
20002ed8 2120 MOVS R1, #0x20
20002eda 2381 MOVS R3, #0x81 (LIB_PRINT)
20002edc 47C0 BLX R8
20002ede 6EA8 LDR R0,[R5,#26]
20002ee0 2120 MOVS R1, #0x20
20002ee2 2381 MOVS R3, #0x81 (LIB_PRINT)
20002ee4 47C0 BLX R8
20002ee6 6BA8 LDR R0,[R5,#14]
20002ee8 211B MOVS R1, #0x1b
20002eea 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002eec 47C0 BLX R8
20002eee 6800 LDR R0,[R0,#0]
20002ef0 2120 MOVS R1, #0x20
20002ef2 2381 MOVS R3, #0x81 (LIB_PRINT)
20002ef4 47C0 BLX R8
20002ef6 6BE8 LDR R0,[R5,#15]
20002ef8 211B MOVS R1, #0x1b
20002efa 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002efc 47C0 BLX R8
20002efe 6800 LDR R0,[R0,#0]
20002f00 2100 MOVS R1, #0x0
20002f02 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f04 47C0 BLX R8
20002f06 6BA8 LDR R0,[R5,#14]
20002f08 211B MOVS R1, #0x1b
20002f0a 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002f0c 47C0 BLX R8
20002f0e B401 PUSH {R0,}
20002f10 207B MOVS R0, #0x7b
20002f12 BC02 POP {R1,}
20002f14 6008 STR R0,[R1,#0]
20002f16 6BE8 LDR R0,[R5,#15]
20002f18 211B MOVS R1, #0x1b
20002f1a 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002f1c 47C0 BLX R8
20002f1e B401 PUSH {R0,}
20002f20 4800 E001 01C8 0000 LDR R0,0x1c8
20002f28 BC02 POP {R1,}
20002f2a 6008 STR R0,[R1,#0]
20002f2c 20EA MOVS R0, #0xea
20002f2e 66A8 STR R0,[R5,#26]
20002f30 2300 MOVS R3, #0x0
20002f32 68BA LDR R2,[R7,#2]
20002f34 8693 STRH R3,[R2,#26]
20002f36 4801 E002 46C0 0237 0000 LDR R0,0x237
20002f40 66A8 STR R0,[R5,#26]
20002f42 2300 MOVS R3, #0x0
20002f44 68BA LDR R2,[R7,#2]
20002f46 8693 STRH R3,[R2,#26]
20002f48 6EA8 LDR R0,[R5,#26]
20002f4a 2120 MOVS R1, #0x20
20002f4c 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f4e 47C0 BLX R8
20002f50 6EA8 LDR R0,[R5,#26]
20002f52 2120 MOVS R1, #0x20
20002f54 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f56 47C0 BLX R8
20002f58 6BA8 LDR R0,[R5,#14]
20002f5a 211B MOVS R1, #0x1b
20002f5c 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002f5e 47C0 BLX R8
20002f60 6800 LDR R0,[R0,#0]
20002f62 2120 MOVS R1, #0x20
20002f64 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f66 47C0 BLX R8
20002f68 6BE8 LDR R0,[R5,#15]
20002f6a 211B MOVS R1, #0x1b
20002f6c 231C MOVS R3, #0x1c (LIB_OBJ_FIELD)
20002f6e 47C0 BLX R8
20002f70 6800 LDR R0,[R0,#0]
20002f72 2100 MOVS R1, #0x0
20002f74 2381 MOVS R3, #0x81 (LIB_PRINT)
20002f76 47C0 BLX R8
20002f78 6830 LDR R0,[R6,#0]
20002f7a B083 SUB SP,SP,#12
20002f7c 9601 STR R6,[SP,#4]
20002f7e 466E MOV R6,R13
20002f80 6030 STR R0,[R6,#0]
20002f82 2000 MOVS R0, #0x0
20002f84 60B0 STR R0,[R6,#2]
20002f86 F000 F803 BL +3 (20002f90)
20002f8a B500 PUSH {LR,}
20002f8c F000 F809 BL +9 (20002fa2)
20002f90 F7FF FFFB BL -5 (20802f8a)
20002f94 0031 MOVS R1,R6
20002f96 2315 MOVS R3, #0x15 (LIB_POST_GOSUB)
20002f98 47C0 BLX R8
20002f9a 6876 LDR R6,[R6,#1]
20002f9c B003 ADD SP,SP,#12
20002f9e 2383 MOVS R3, #0x83 (LIB_END)
20002fa0 47C0 BLX R8
20002fa2 2000 MOVS R0, #0x0
20002fa4 6228 STR R0,[R5,#8]
20002fa6 2300 MOVS R3, #0x0
20002fa8 68BA LDR R2,[R7,#2]
20002faa 8213 STRH R3,[R2,#8]
20002fac 2002 MOVS R0, #0x2
20002fae B401 PUSH {R0,}
20002fb0 6A28 LDR R0,[R5,#8]
20002fb2 F000 F808 BL +8 (20002fc6)
20002fb6 2002 MOVS R0, #0x2
20002fb8 B401 PUSH {R0,}
20002fba 6A28 LDR R0,[R5,#8]
20002fbc 3001 ADDS R0,#1
20002fbe 6228 STR R0,[R5,#8]
20002fc0 2300 MOVS R3, #0x0
20002fc2 68BA LDR R2,[R7,#2]
20002fc4 8213 STRH R3,[R2,#8]
20002fc6 BC02 POP {R1,}
20002fc8 4281 CMP R1,R0
20002fca DA01 BGE 1
20002fcc F000 F810 BL +16 (20002ff0)
20002fd0 6BA8 LDR R0,[R5,#14]
20002fd2 B401 PUSH {R0,}
20002fd4 6A28 LDR R0,[R5,#8]
20002fd6 0080 inst0000
20002fd8 BC02 POP {R1,}
20002fda 5808 inst5800
20002fdc B401 PUSH {R0,}
20002fde 2000 MOVS R0, #0x0
20002fe0 BC02 POP {R1,}
20002fe2 2302 MOVS R3, #0x2 (LIB_HEX)
20002fe4 47C0 BLX R8
20002fe6 2121 MOVS R1, #0x21
20002fe8 2381 MOVS R3, #0x81 (LIB_PRINT)
20002fea 47C0 BLX R8
20002fec F7FF FFE3 BL -29 (20802fb6)
20002ff0 2000 MOVS R0, #0x0
20002ff2 2101 MOVS R1, #0x1
20002ff4 2381 MOVS R3, #0x81 (LIB_PRINT)
20002ff6 47C0 BLX R8
20002ff8 2000 MOVS R0, #0x0
20002ffa 6228 STR R0,[R5,#8]
20002ffc 2300 MOVS R3, #0x0
20002ffe 68BA LDR R2,[R7,#2]
20003000 8213 STRH R3,[R2,#8]
20003002 2002 MOVS R0, #0x2
20003004 B401 PUSH {R0,}
20003006 6A28 LDR R0,[R5,#8]
20003008 F000 F808 BL +8 (2000301c)
2000300c 2002 MOVS R0, #0x2
2000300e B401 PUSH {R0,}
20003010 6A28 LDR R0,[R5,#8]
20003012 3001 ADDS R0,#1
20003014 6228 STR R0,[R5,#8]
20003016 2300 MOVS R3, #0x0
20003018 68BA LDR R2,[R7,#2]
2000301a 8213 STRH R3,[R2,#8]
2000301c BC02 POP {R1,}
2000301e 4281 CMP R1,R0
20003020 DA01 BGE 1
20003022 F000 F810 BL +16 (20003046)
20003026 6BE8 LDR R0,[R5,#15]
20003028 B401 PUSH {R0,}
2000302a 6A28 LDR R0,[R5,#8]
2000302c 0080 inst0000
2000302e BC02 POP {R1,}
20003030 5808 inst5800
20003032 B401 PUSH {R0,}
20003034 2000 MOVS R0, #0x0
20003036 BC02 POP {R1,}
20003038 2302 MOVS R3, #0x2 (LIB_HEX)
2000303a 47C0 BLX R8
2000303c 2121 MOVS R1, #0x21
2000303e 2381 MOVS R3, #0x81 (LIB_PRINT)
20003040 47C0 BLX R8
20003042 F7FF FFE3 BL -29 (2080300c)
20003046 2000 MOVS R0, #0x0
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
"PRINT CLASS1::TEST,CLASS2::TEST,O.TEST2,P.TEST2" CR
"O.TEST2=123" CR
"P.TEST2=456" CR
"CLASS1::TEST=234" CR
"CLASS2::TEST=567" CR
"PRINT CLASS1::TEST,CLASS2::TEST,O.TEST2,P.TEST2" CR
"REM This is error: O.TEST3=123" CR
"GOSUB SUB" CR
"END" CR
"LABEL SUB" CR
"FOR I=0 TO 2:PRINT HEX$(O(I)),:NEXT" CR
"PRINT" CR
"FOR I=0 TO 2:PRINT HEX$(P(I)),:NEXT" CR
"PRINT" CR
"RETURN" CR
	,"CLASS1.BAS",
"STATIC TEST" CR
"FIELD  TEST2" CR
"FIELD PRIVATE TEST3" CR
"END" CR
	,"CLASS2.BAS",
"STATIC TEST" CR
"FIELD PRIVATE TEST3" CR
"FIELD  TEST2" CR
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

