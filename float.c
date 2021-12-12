/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include "./compiler.h"

int float_arg1_function(int func){
	return argn_function(LIB_MATH,ARG_FLOAT<<ARG1 | func<<LIBOPTION);
}

int float_arg2_function(int func){
	return argn_function(LIB_MATH,ARG_FLOAT<<ARG1 | ARG_FLOAT<<ARG2 | func<<LIBOPTION);
}

int pi_function(void){
	g_scratch_float[0]=3.141593;
	return set_value_in_register(0,g_scratch_int[0]);
}

int float_function(void){
	return argn_function(LIB_FLOAT,ARG_INTEGER<<ARG1);
}

int val_float_function(void){
	return argn_function(LIB_VAL_FLOAT,ARG_STRING<<ARG1);
}

int float_functions(void){
	if (instruction_is("ACOS#(")) return float_arg1_function(MATH_ACOS);
	if (instruction_is("ARGS#(")) return args_function();
	if (instruction_is("ASIN#(")) return float_arg1_function(MATH_ASIN);
	if (instruction_is("ATAN#(")) return float_arg1_function(MATH_ATAN);
	if (instruction_is("ATAN2#(")) return float_arg2_function(MATH_ATAN2);
	if (instruction_is("CEIL#(")) return float_arg1_function(MATH_CEIL);
	if (instruction_is("COS#(")) return float_arg1_function(MATH_COS);
	if (instruction_is("COSH#(")) return float_arg1_function(MATH_COSH);
	if (instruction_is("DEBUG#(")) return debug_function();
	if (instruction_is("EXP#(")) return float_arg1_function(MATH_EXP);
	if (instruction_is("FABS#(")) return float_arg1_function(MATH_FABS);
	if (instruction_is("FLOAT#(")) return float_function();
	if (instruction_is("FLOOR#(")) return float_arg1_function(MATH_FLOOR);
	if (instruction_is("FMOD#(")) return float_arg2_function(MATH_FMOD);
	if (instruction_is("GOSUB#(")) return gosub_function();
	if (instruction_is("LOG#(")) return float_arg1_function(MATH_LOG);
	if (instruction_is("LOG10#(")) return float_arg1_function(MATH_LOG10);
	if (instruction_is("MODF#(")) return float_arg1_function(MATH_MODF);
	if (instruction_is("POW#(")) return float_arg2_function(MATH_POW);
	if (instruction_is("SIN#(")) return float_arg1_function(MATH_SIN);
	if (instruction_is("SINH#(")) return float_arg1_function(MATH_SINH);
	if (instruction_is("SQRT#(")) return float_arg1_function(MATH_SQRT);
	if (instruction_is("TAN#(")) return float_arg1_function(MATH_TAN);
	if (instruction_is("TANH#(")) return float_arg1_function(MATH_TANH);
	if (instruction_is("VAL#(")) return val_float_function();
	return ERROR_SYNTAX;
}

int get_simple_float(void){
	int i,vn;
	unsigned char* err;
	float f;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_value(VAR_MODE_FLOAT);
		if (i) return i;
		g_constant_float=0-g_constant_float;
		check_object(2);
		(object++)[0]=0x2100;          // movs	r1, #0
		(object++)[0]=0x2200 | OP_SUB; // movs	r2, #OP_SUB
		return call_lib_code(LIB_CALC_FLOAT);
	}
	if ('0'<=source[0] && source[0]<='9' || '.'==source[0]) {
		// Float literal
		f=strtof((const char*)&source[0],(char**)&err);
		i=err-source;
		if (0==i) return ERROR_SYNTAX;
		source+=i;
		g_scratch_float[0]=f;
		g_constant_float=f;
		return set_value_in_register(0,g_scratch_int[0]);
	} else if ('A'<=source[0] && source[0]<='Z' || '_'==source[0]) {
		// Lower constant flag
		g_constant_value_flag=0;
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) {
			vn=static_method_or_property(vn,'#');
			if (vn<=0) return vn; // Error (vn==0) or method (vn<0)
		} else {
			// Variable or function
			vn=get_var_number();
		}
		if (0<=vn) {
			// Get variable value
			i=variable_to_r0(vn);
			if (i) return i;
			// Check if an object
			if ('.'==source[0]) {
				source++;
				i=method_or_property('#');
				g_constant_value_flag=0;
				return i;
			}
			if ('#'!=source[0]) return ERROR_SYNTAX;
			source++;
			// Check if an array
			if ('('==source[0]) {
				source++;
				i=get_dim_value();
				if (i) return i;
				if (')'!=source[0]) return ERROR_SYNTAX;
				source++;
			}
			g_constant_value_flag=0;
			return 0;
		} else if ('P'==source[0] && 'I'==source[1] && '#'==source[2]) {
			// PI#
			source+=3;
			return pi_function();
		} else {
			// This must be a function
			i=float_functions();
			if (i) return i;
			g_constant_value_flag=0;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	}
	return ERROR_SYNTAX;
}
int get_float(void){
	return get_value(VAR_MODE_FLOAT);
}
