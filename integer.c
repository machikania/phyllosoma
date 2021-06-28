/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
TODO:

INT(x#)
	実数値x#を整数値に変換して返す。
LEN(x$)
	文字列の長さを返す。
RND()
	0から32767までの擬似乱数を返す。
STRNCMP(x$,y$,z)
	２つの文字列のうちz文字分を比較し、結果を返す。同じ文字列の場合は０。
VAL(x$)
	１０進数もしくは１６進数文字列としてのx$の値を、整数値で返す。
*/

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
	int e;
	e=get_string();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x7800; //      	ldrb	r0, [r0, #0]
	return 0;
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

int integer_functions(void){
	if (instruction_is("ABS(")) return abs_function();
	if (instruction_is("ASC(")) return asc_function();
	if (instruction_is("NOT(")) return not_function();
	if (instruction_is("PEEK(")) return peek_function();
	if (instruction_is("PEEK16(")) return peek16_function();
	if (instruction_is("PEEK32(")) return peek32_function();
	if (instruction_is("SGN(")) return sgn_function();
	if (instruction_is("ARGS(")) return args_function();
	if (instruction_is("GOSUB(")) return gosub_function();
	if (instruction_is("DEBUG(")) return debug_function();
	return ERROR_SYNTAX;
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
		i=get_simple_integer();
		if (i) return i;
		check_object(1);
		(object++)[0]=0x4240; // negs	r0, r0
		g_constant_int=0-i;
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
	} else if ('0'<=source[0] && source[0]<'9') {
		// Decimal value
		i=get_positive_decimal_value();
		if (i<0) return i;
		g_constant_int=i;
		return set_value_in_register(0,i);
	} else if ('A'<=source[0] && source[0]<'Z' || '_'==source[0]) {
		// Lower constant flag
		g_constant_value_flag=0;
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) return static_method_or_property(vn,0);
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
			return 0;
		} else {
			// TODO: support class static property
			// This must be a function
			i=integer_functions();
			if (i) return i;
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
		return 0;
	}
	return ERROR_SYNTAX;
}

/*
// r0=1-r0
100003bc:	2301      	movs	r3, #1
100003be:	1a18      	subs	r0, r3, r0

// NOT
100003bc:	4243      	negs	r3, r0
100003be:	4158      	adcs	r0, r3

*/

int get_integer(void){
	return get_value(VAR_MODE_INTEGER);
}
