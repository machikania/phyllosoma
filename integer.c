/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

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
		// Variable or function
		vn=get_var_number();
		if (0<=vn) {
			// This is a variable
			// TODO: support array
			return variable_to_r0(vn);
		} else {
			// This must be a function
			i=integer_functions();
			if (i) return i;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	}
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
