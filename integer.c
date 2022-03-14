/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./display.h"

int strncmp_function(void){
	return argn_function(LIB_STRNCMP,
		ARG_STRING<<ARG1 |
		ARG_STRING<<ARG2 |
		ARG_INTEGER<<ARG3 );
}

int val_function(void){
	return argn_function(LIB_VAL,ARG_STRING<<ARG1);
}

int len_function(void){
	return argn_function(LIB_LEN,ARG_STRING<<ARG1);
}

int int_function(void){
	return argn_function(LIB_INT,ARG_FLOAT<<ARG1);
}

int rnd_function(void){
	return call_lib_code(LIB_RND);
}


int peek_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x7800; //      	ldrb	r0, [r0, #0]
	return 0;
}

int peek16_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x8800; //      	ldrh	r0, [r0, #0]
	return 0;
}

int peek32_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x6800; //      	ldr	r0, [r0, #0]
	return 0;
}

int asc_function(void){
	return argn_function(LIB_ASC,ARG_STRING<<ARG1);
}

int sgn_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(5);
	(object++)[0]=0x17c3; //      	asrs	r3, r0, #31
	(object++)[0]=0x1a1b; //      	subs	r3, r3, r0
	(object++)[0]=0x0fdb; //      	lsrs	r3, r3, #31
	(object++)[0]=0x0fc0; //      	lsrs	r0, r0, #31
	(object++)[0]=0x1a18; //      	subs	r0, r3, r0
	return 0;
}

int abs_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(3);
	(object++)[0]=0x17c3; //      	asrs	r3, r0, #31
	(object++)[0]=0x18c0; //      	adds	r0, r0, r3
	(object++)[0]=0x4058; //      	eors	r0, r3
	return 0;
}

int not_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(2);
	(object++)[0]=0x4243; //      	negs	r3, r0
	(object++)[0]=0x4158; //      	adcs	r0, r3
	return 0;
}

int inkey_function(void){
	g_default_args[1]=0;
	return argn_function(LIB_INKEY,ARG_INTEGER_OPTIONAL<<ARG1);
}

int drawcount_function(void){
	set_value_in_register(0,-1);
	return call_lib_code(LIB_DRAWCOUNT);
}

int keys_function(void){
	g_default_args[1]=63;
	return argn_function(LIB_KEYS,ARG_INTEGER_OPTIONAL<<ARG1);
}

int system_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	return call_lib_code(LIB_SYSTEM);
}

int integer_functions(void){
	if (instruction_is("ABS(")) return abs_function();
	if (instruction_is("ARGS(")) return args_function();
	if (instruction_is("ASC(")) return asc_function();
	if (instruction_is("CREAD(")) return cread_function();
	if (instruction_is("DEBUG(")) return debug_function();
	if (instruction_is("DRAWCOUNT(")) return drawcount_function();
	if (instruction_is("GOSUB(")) return gosub_function();
	if (instruction_is("INKEY(")) return inkey_function();
	if (instruction_is("INT(")) return int_function();
	if (instruction_is("KEYS(")) return keys_function();
	if (instruction_is("LEN(")) return len_function();
	if (instruction_is("NEW(")) return new_function();
	if (instruction_is("NOT(")) return not_function();
	if (instruction_is("PEEK(")) return peek_function();
	if (instruction_is("PEEK16(")) return peek16_function();
	if (instruction_is("PEEK32(")) return peek32_function();
	if (instruction_is("READ(")) return read_function();
	if (instruction_is("RND(")) return rnd_function();
	if (instruction_is("SGN(")) return sgn_function();
	if (instruction_is("STRNCMP(")) return strncmp_function();
	if (instruction_is("SYSTEM(")) return system_function();
	if (instruction_is("VAL(")) return val_function();
	// File functions
	if (instruction_is("FOPEN(")) return fopen_function();
	if (instruction_is("FEOF(")) return feof_function();
	if (instruction_is("FGET(")) return fget_function();
	if (instruction_is("FGETC(")) return fgetc_function();
	if (instruction_is("FLEN(")) return flen_function();
	if (instruction_is("FPUT(")) return fput_function();
	if (instruction_is("FPUTC(")) return fputc_function();
	if (instruction_is("FREMOVE(")) return fremove_function();
	if (instruction_is("FSEEK(")) return fseek_function();
	if (instruction_is("SETDIR(")) return setdir_function();
	// Environment
	return display_functions();
}

int get_positive_decimal_value(void){
	int i;
	if (source[0]<'0' || '9'<source[0]) return ERROR_SYNTAX;
	for(i=0;'0'<=source[0] && source[0]<='9';source++) i=i*10+(source[0]-'0');
	return i;
}

int get_simple_integer(void){
	int i,vn;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_value(VAR_MODE_INTEGER);
		if (i) return i;
		check_object(1);
		(object++)[0]=0x4240; // negs	r0, r0
		g_constant_int=0-g_constant_int;
		return 0;
	}
	if ('$'==source[0] || '0'==source[0] && 'X'==source[1]) {
		// Hex value
		source+=('$'==source[0]) ? 1:2;
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=(i<<4)|(source[0]-'0');
				source++;
			} else if ('A'<=source[0] && source[0]<='F') {
				i=(i<<4)|(source[0]-'A'+10);
				source++;
			} else {
				break;
			}
		}
		g_constant_int=i;
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<='9') {
		// Decimal value
		i=get_positive_decimal_value();
		if (i<0) return i;
		g_constant_int=i;
		return set_value_in_register(0,i);
	} else if ('A'<=source[0] && source[0]<='Z' || '_'==source[0]) {
		// Lower constant flag
		g_constant_value_flag=0;
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) {
			vn=static_method_or_property(vn,0);
			if (vn<=0) return vn; // Error (vn==0) or method (vn<0)
		} else {
			// Variable or function
			vn=get_var_number();
		}
		if (0<=vn) {
			// Get variable value
			i=variable_to_r0(vn);
			if (i) return i;
			// Check if an array
			if ('('==source[0]) {
				source++;
				i=get_dim_value();
				if (i) return i;
				if (')'!=source[0]) return ERROR_SYNTAX;
				source++;
			}
			// Check if an object
			if ('.'==source[0]) {
				source++;
				i=method_or_property(0);
				g_constant_value_flag=0;
				return i;
			}
			g_constant_value_flag=0;
			return 0;
		} else {
			// This must be a function
			i=integer_functions();
			if (i) return i;
			g_constant_value_flag=0;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	} else if ('&'==source[0]) {
		// '&' operator
		source++;
		vn=get_var_number();
		if (vn<0) return vn;
		check_object(3);
		(object++)[0]=0x2000 | vn; // movs	r0, #xx
		(object++)[0]=0x0080;      // lsls	r0, r0, #2
		(object++)[0]=0x1940;      // adds	r0, r0, r5
		g_constant_value_flag=0;
		return 0;
	}
	return ERROR_SYNTAX;
}

int get_integer(void){
	return get_value(VAR_MODE_INTEGER);
}
