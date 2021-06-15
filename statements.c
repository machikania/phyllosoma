/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int let_integer(int vn){
	int e;
	switch((source++)[0]){
		case '=': // sinple integer
			e=get_integer();
			if (e) return e;
			return r0_to_variable(vn);
		case '(': // integer array
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_string(int vn){
	int e;
	switch((source++)[0]){
		case '=': // simple string
			e=get_string();
			if (e) return e;
			e=var_num_to_r1(vn);
			if (e) return e;
			return call_lib_code(LIB_LET_STR);
		case '(': // string array (not supported)
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_float(int vn){
	int e;
	switch((source++)[0]){
		case '=': // sinple float
			e=get_float();
			if (e) return e;
			return r0_to_variable(vn);
		case '(': // float array
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_statement(void){
	int i;
	// Check if there is '='
	for(i=0;source[i]!='=';i++){
		if (':'==source[i] || 0x00==source[i] || '"'==source[i]) return ERROR_SYNTAX;
	}
	i=get_var_number();
	if (i<0) return i;
	switch(source[0]){
		case '$': // string
			source++;
			skip_blank();
			return let_string(i);
		case '#': // float
			source++;
			skip_blank();
			return let_float(i);
		case '(': // integer array
		default: // integer
			skip_blank();
			return let_integer(i);
	}
}

int print_statement(void) {
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	int e;
	unsigned char mode;
	unsigned char* sb;
	unsigned short* ob;
	while(1){
		sb=source;
		ob=object;
		mode=0x01;
		e=get_string();
		if (e || ','!=source[0] && ';'!=source[0] && ':'!=source[0] && 0x00!=source[0]) {
			source=sb;
			object=ob;
			mode=0x00;
			e=get_integer();
		}
		if (e || ','!=source[0] && ';'!=source[0] && ':'!=source[0] && 0x00!=source[0]) {
			source=sb;
			object=ob;
			mode=0x02;
			e=get_float();
		}
		if (e) return e;
		skip_blank();
		if (';'==source[0]) {
			mode|=0x10;
			source++;
		} else if (','==source[0]) {
			mode|=0x20;
			source++;
		}
		check_object(1);
		(object++)[0]=0x2100|mode; // movs	r1, #xxxx
		e=call_lib_code(LIB_PRINT);
		if (e) return e;
		if (0x00==mode&0xf0) break;
		skip_blank();
		if (0x00==source[0] || ':'==source[0]) break;
	}
	return 0;
}

int debug_statement(void){
	return call_lib_code(LIB_DEBUG);
}

int return_statement(void){
	check_object(1);
	(object++)[0]=0xbd00;//      	pop	{pc}
	return 0;
}

int end_statement(void){
	return call_lib_code(LIB_END);
}
