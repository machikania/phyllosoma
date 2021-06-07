/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_var_number(void){
	if (source[0]<'A' || 'Z'<source[0]) return ERROR_SYNTAX;
	return (source++)[0]-'A';
}
