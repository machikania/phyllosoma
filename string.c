/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int read_str_function(void){
	return argn_function(LIB_READ_STR,ARG_NONE);
}

int chr_function(void){
	return argn_function(LIB_CHR,ARG_INTEGER<<ARG1);
}

int dec_function(void){
	return argn_function(LIB_DEC,ARG_INTEGER<<ARG1);
}

int float_str_function(void){
	return argn_function(LIB_FLOAT_STRING,ARG_FLOAT<<ARG1);
}

int sprintf_function(void){
	return argn_function(LIB_SPRINTF,ARG_STRING<<ARG1 | ARG_FLOAT<<ARG2);
}

int mid_string(int vn){
	g_default_args[2]=-1;
	// Need to increment vn to support the variable 'A'. See lib_mid().
	return argn_function(LIB_MID,ARG_INTEGER<<ARG1 | ARG_INTEGER_OPTIONAL<<ARG2 | (vn+1)<<LIBOPTION);
}

int hex_function(void){
	g_default_args[2]=0;
	return argn_function(LIB_HEX,ARG_INTEGER<<ARG1 | ARG_INTEGER_OPTIONAL<<ARG2);
}

int input_function(void){
	return argn_function(LIB_INPUT,ARG_NONE);
}

int string_functions(void){
	if (instruction_is("ARGS$(")) return args_function();
	if (instruction_is("CHR$(")) return chr_function();
	if (instruction_is("DEBUG$(")) return debug_function();
	if (instruction_is("DEC$(")) return dec_function();
	if (instruction_is("FLOAT$(")) return float_str_function();
	if (instruction_is("GOSUB$(")) return gosub_function();
	if (instruction_is("HEX$(")) return hex_function();
	if (instruction_is("INPUT$(")) return input_function();
	if (instruction_is("READ$(")) return read_str_function();
	if (instruction_is("SPRINTF$(")) return sprintf_function();
	if (instruction_is("SYSTEM$(")) return system_function();
	// File functions
	if (instruction_is("FINPUT$(")) return finput_function();
	if (instruction_is("GETDIR$(")) return getdir_function();
	return ERROR_SYNTAX;
}

int get_byte(void){
	unsigned char c;
	if ('0'<=source[0] && source[0]<='9') c=source[0]-'0';
	else if ('A'<=source[0] && source[0]<='F') c=source[0]-'A'+10;
	else if ('a'<=source[0] && source[0]<='f') c=source[0]-'a'+10;
	else return ERROR_SYNTAX;
	source++;
	c<<=4;
	if ('0'<=source[0] && source[0]<='9') c|=source[0]-'0';
	else if ('A'<=source[0] && source[0]<='F') c|=source[0]-'A'+10;
	else if ('a'<=source[0] && source[0]<='f') c|=source[0]-'a'+10;
	else return ERROR_SYNTAX;
	source++;
	return (int)c;
}

int string_char(void){
	unsigned char c;
	c=source[0];
	source++;
	switch(c){
		case '"':
			return 0;
		case '\\':
			break;
		case 0:
			return ERROR_SYNTAX;
		default:
			return (int)c;
	}
	// \ is found
	c=source[0];
	source++;
	switch(c){
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case '0':
			return '\0';
		case '\\':
			return '\\';
		case 'x':
		case 'X':
			return get_byte();
		default:
			source--;
			return '\\';
	}
}

int get_simple_string(void){
	int e,i,vn;
	unsigned char c;
	skip_blank();
	if ('"'==source[0]) {
		// Constant string
		i=0;
		source++;
		// Get string pointer in R0
		object[i++]=0x4678;//      	mov	r0, pc
		object[i++]=0x3002;//      	adds	r0, #2
		// Write branch instruction
		object[i++]=0xe000; // b.n xxxx
		while(1){
			e=string_char();
			if (e<0) return e;
			if (0==e) {
				object[i++]=0x0000;
				break;
			}
			c=e;
			e=string_char();
			if (e<0) return e;
			if (0==e) {
				object[i++]=c;
				break;
			}
			object[i++]=(e<<8)|c;
		}
		// Update branch instruction
		object[2]|=i-2-2;
		// Check if object area remains
		check_object(i);
		// Move object position
		object+=i;
		// Return without error
		return 0;
	} else if ('A'<=source[0] && source[0]<='Z' || '_'==source[0]) {
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) {
			vn=static_method_or_property(vn,'$');
			if (vn<=0) return vn; // Error (vn==0) or method (vn<0)
		} else {
			// Variable or function
			vn=get_var_number();
		}
		if (0<=vn) {
			if ('.'==source[0]) {
				source++;
				e=variable_to_r0(vn);
				if (e) return e;
				return method_or_property('$');
			}
			// This is a variable
			if ('$'!=source[0]) return ERROR_SYNTAX;
			source++;
			if ('('==source[0]) {
				source++;
				// Part of string variable
				e=mid_string(vn);
				if (e) return e;
				if (')'!=source[0]) return ERROR_SYNTAX;
				source++;
				return 0;
			} else {
				// Simple string variable
				return variable_to_r0(vn);
			}
		} else {
			// This must be a function
			e=string_functions();
			if (e) return e;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	}
	return ERROR_SYNTAX;
}

int get_string(void){
	int e;
	e=get_simple_string();
	if (e) return e;
	skip_blank();
	// Only '+' can be used as an operator
	while('+'==source[0]) {
		source++;
		check_object(1);
		(object++)[0]=0xb401; // push	{r0}
		e=get_simple_string();
		if (e) return e;
		check_object(1);
		(object++)[0]=0xbc02; // pop	{r1}
		e=call_lib_code(LIB_ADD_STRING);
		if (e) return e;
		skip_blank();
	}
	return 0;
}
