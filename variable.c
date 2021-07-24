/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	CMPDATA_VARNAME structure
		type:      CMPDATA_VARNAME
		len:       n+1
		data16:    var number
		record[1]: hash
		record[2]: string
		record[3]: string (continued)
		...
		record[n]: end of string
*/

void variable_init(void){
	g_next_varnum=26;
}

int set_value_in_register(unsigned char r,int val){
	if (7<r) return ERROR_UNKNOWN;
	if (0<=val && val<=255) {
		check_object(1);
		(object++)[0]=0x2000 | val | (r<<8);      // movs	rx, #xx
		return 0;
	} else if (-255<=val && val<0) {
		val=0-val;
		check_object(2);
		(object++)[0]=0x2000 | val | (r<<8);      // movs	rx, #xx
		(object++)[0]=0x4240;                     // negs	r0, r0
		return 0;
	} else if ((int)object&0x03) {
		// Lower 2 bit of object is 0b10
		check_object(5);
		(object++)[0]=0x4801|(r<<8); // ldr    rx, [pc, #4]
		(object++)[0]=0xe002;        // b.n    <skip>
		(object++)[0]=0x46c0;        // nop            ; (mov r8, r8)
		(object++)[0]=val&0xffff;    // lower 16 bits
		(object++)[0]=val>>16;       // upper 16 bits
		                             // <skip>:	
		return 0;
	} else {
		// Lower 2 bit of object is 0b00
		check_object(4);
		(object++)[0]=0x4800|(r<<8); // ldr    rx, [pc, #4]
		(object++)[0]=0xe001;        // b.n    <skip>
		(object++)[0]=val&0xffff;    // lower 16 bits
		(object++)[0]=val>>16;       // upper 16 bits
		                             // <skip>:	
		return 0;
	}
}

short get_new_varnum(void){
	if (g_next_varnum<ALLOC_BLOCK_NUM-TEMPVAR_NUMBER) return g_next_varnum++;
	return 0;
}

int get_var_number(void){
	int num;
	int* data;
	// Only support A-Z now
	if (source[0]<'A' || 'Z'<source[0]) return ERROR_SYNTAX;
	if ('A'<=source[1] && source[1]<='Z' || '_'==source[1] || '0'<=source[1] && source[1]<='9') {
		// Long name
		for(num=2;'A'<=source[num] && source[num]<='Z' || '_'==source[num] || '0'<=source[num] && source[num]<='9';num++);
		data=cmpdata_nsearch_string_first(CMPDATA_VARNAME,source,num);
		if (!data) return ERROR_SYNTAX;
		source+=num;
		return data[0]&0x0000ffff;
	}
	// A-Z (short name)
	return (source++)[0]-'A';
}

int var_num_to_r1(int vn){
	// TODO: replace all var_num_to_r1() by set_value_in_register
	return set_value_in_register(1,vn);
/*	if (vn<256) {
		check_object(1);
		(object++)[0]=0x2100 | vn;      // movs	r1, #xx
		return 0;
	} else return ERROR_UNKNOWN;
*/
}
int r0_to_variable(int vn){
	int e;
	if (vn<32) {
		check_object(4);
		(object++)[0]=0x6028 | (vn<<6); // str	r0, [r5, #xx]
		(object++)[0]=0x2300;           // movs	r3, #0
		(object++)[0]=0x68ba;           // ldr	r2, [r7, #8]
		(object++)[0]=0x8013 | (vn<<6); // strh	r3, [r2, #xx]
		return 0;
	} else if (vn<256) {
		e=var_num_to_r1(vn*4);
		if (e) return e;
		check_object(4);
		(object++)[0]=0x5068;           // str	r0, [r5, r1]
		(object++)[0]=0x2300;           // movs	r3, #0
		(object++)[0]=0x68ba;           // ldr	r2, [r7, #8]
		(object++)[0]=0x5253;           // strh	r3, [r2, r1]
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
		e=var_num_to_r1(vn*4);
		if (e) return e;
		check_object(1);
		(object++)[0]=0x5868;           // ldr	r0, [r5, r1]
		return 0;
	} else return ERROR_UNKNOWN;
}
