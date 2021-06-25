/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int args_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(2);
	(object++)[0]=0x3002; // adds	r0, #2
	(object++)[0]=0x0080; // lsls	r0, r0, #2
	(object++)[0]=0x5830; // ldr	r0, [r6, r0]
	return 0;
}
int gosub_function(void){
	return gosub_statement();
}

int debug_function(void){
	return call_lib_code(LIB_DEBUG);
}

int integer_functions(void){
	if (instruction_is("ARGS(")) return args_function();
	if (instruction_is("GOSUB(")) return gosub_function();
	if (instruction_is("DEBUG(")) return debug_function();
	return ERROR_SYNTAX;
}

int float_functions(void){
	if (instruction_is("ARGS#(")) return args_function();
	if (instruction_is("GOSUB#(")) return gosub_function();
	if (instruction_is("DEBUG#(")) return debug_function();
	return ERROR_SYNTAX;
}

int string_functions(void){
	if (instruction_is("HEX$(")) return hex_function();
	if (instruction_is("ARGS$(")) return args_function();
	if (instruction_is("GOSUB$(")) return gosub_function();
	if (instruction_is("DEBUG$(")) return debug_function();
	return ERROR_SYNTAX;
}
