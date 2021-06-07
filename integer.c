/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_simple_integer(void){
	int i;
	skip_blank();
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
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9') {
		// Decimal value
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=i*10+(source[0]-'0');
				source++;
			} else {
				break;
			}
		}
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9' || '_'==source[0]) {
		// Variable or function
		// TODO: here
		return ERROR_UNKNOWN;
	}
	return 0;
}

int get_integer(void){
	skip_blank();
	return get_simple_integer();
	return 0;
}
