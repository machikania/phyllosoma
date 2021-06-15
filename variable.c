/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_var_number(void){
	// Only support A-Z now
	if (source[0]<'A' || 'Z'<source[0]) return ERROR_SYNTAX;
	if ('A'<=source[1] && source[1]<='Z' || '_'==source[1]) return ERROR_SYNTAX;
	return (source++)[0]-'A';
}

int var_num_to_r1(int vn){
	if (vn<256) {
		check_object(1);
		(object++)[0]=0x2100 | vn;      // movs	r1, #xx
	} else return ERROR_UNKNOWN;
}
int r0_to_variable(int vn){
	int e;
	if (vn<32) {
		check_object(1);
		(object++)[0]=0x6028 | (vn<<6); // str	r0, [r5, #xx]
		return 0;
	} else if (vn<256) {
		e=var_num_to_r1(vn);
		if (e) return e;
		check_object(1);
		(object++)[0]=0x5068;           // str	r0, [r5, r1]
		return 0;
	} else return ERROR_UNKNOWN;
}
int variable_to_r0(int vn){
	int e;
	if (vn<32) {
		check_object(1);
		(object++)[0]=0x6828 | (vn<<6); // ldr	r0, [r5, #xx]
		return 0;
	} else if (vn<256) {
		e=var_num_to_r1(vn);
		if (e) return e;
		check_object(1);
		(object++)[0]=0x5868;           // ldr	r0, [r5, r1]
		return 0;
	} else return ERROR_UNKNOWN;
}
