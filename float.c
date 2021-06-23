/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include "./compiler.h"

int get_simple_float(void){
	int i,vn;
	unsigned char* err;
	float f;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_float();
		if (i) return i;
		g_constant_float=0-g_constant_float;
		check_object(2);
		(object++)[0]=0x2100;          // movs	r1, #0
		(object++)[0]=0x2200 | OP_SUB; // movs	r2, #OP_SUB
		return call_lib_code(LIB_CALC_FLOAT);
	}
	if ('0'<=source[0] && source[0]<'9' || '.'==source[0]) {
		// Float literal
		f=strtof((const char*)&source[0],(char**)&err);
		i=err-source;
		if (0==i) return ERROR_SYNTAX;
		source+=i;
		g_scratch_float[0]=f;
		g_constant_float=f;
		return set_value_in_register(0,g_scratch_int[0]);
	} else if ('A'<=source[0] && source[0]<'Z' || '_'==source[0]) {
		// Lower constant flag
		g_constant_value_flag=0;
		// Variable or function
		vn=get_var_number();
		if (0<=vn) {
			// This is a variable
			if ('#'!=source[0]) return ERROR_SYNTAX;
			source++;
			// TODO: support array
			return variable_to_r0(vn);
		} else {
			// This must be a function
			i=float_functions();
			if (i) return i;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	}
}
int get_float(void){
	return get_value(VAR_MODE_FLOAT);
}
