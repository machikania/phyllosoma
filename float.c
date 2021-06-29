/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include "./compiler.h"

/*
TODO:

ACOS#(x#)
	x# の逆余弦を実数値で返す。
ARGS#(x)
	サブルーチン中で、GOSUBもしくはメソッドに渡されたx番目の引数を実数値として取
	り出す。
ASIN#(x#)
	x# の逆正弦を実数値で返す。
ATAN#(x#)
	x# の逆正接を実数値で返す。
ATAN2#(y#,x#)
	y#/x# の逆正接を実数値で返す。
CEIL#(x#)
	x# 以上の最小の整数を実数値で返す。
COS#(x#)
	x# の余弦を実数値で返す。
COSH#(x#)
	x# の双曲線余弦を実数値で返す。
EXP#(x#)
	eを底とする x# の指数関数値を実数値で返す。
FABS#(x#)
	x# の絶対値を実数値で返す。
FLOOR#(x#)
	x# 以下の最大の整数を実数値で返す。
FMOD#(x#,y#)
	x# を y# で割った剰余を実数値で返す。
LOG#(x#)
	x# の自然対数を実数値で返す。
LOG10#(x#)
	x# の常用対数を実数値で返す。
MODF#(x#)
	x# の小数部を実数値で返す。
PI#
	3.141593を返す。
POW#(x#,y#)
	x# の y# 乗を実数値で返す。
SIN#(x#)
	x# の正弦を実数値で返す。
SINH#(x#)
	x# の双曲線正弦を実数値で返す。
SQRT#(x#)
	x# の平方根を実数値で返す。
TAN#(x#)
	x# の正接を実数値で返す。
TANH#(x#)
	x# の双曲線正接を実数値で返す。
*/

int float_function(void){
	return argn_function(LIB_FLOAT,ARG_INTEGER<<ARG1);
}

int val_float_function(void){
	return argn_function(LIB_VAL_FLOAT,ARG_STRING<<ARG1);
}

int float_functions(void){
	if (instruction_is("FLOAT#(")) return float_function();
	if (instruction_is("VAL#(")) return val_float_function();
	if (instruction_is("ARGS#(")) return args_function();
	if (instruction_is("GOSUB#(")) return gosub_function();
	if (instruction_is("DEBUG#(")) return debug_function();
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
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) return static_method_or_property(vn,'#');
		// Variable or function
		vn=get_var_number();
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
				return method_or_property(0);
			}
			if ('#'!=source[0]) return ERROR_SYNTAX;
			source++;
			return 0;
		} else {
			// This must be a function
			i=float_functions();
			if (i) return i;
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
