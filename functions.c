/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int debug_function(void){
	return call_lib_code(LIB_DEBUG);
}

int integer_functions(void){
	if (instruction_is("DEBUG(")) return debug_function();
	return ERROR_SYNTAX;
}
