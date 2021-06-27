/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int hex_function(void){
	int e;
	e=get_integer();
	if (','==source[0]) {
		source++;
		check_object(1);
		(object++)[0]=0xb401; // push	{r0}
		e=get_integer();
		if (e) return e;
		check_object(1);
		(object++)[0]=0xbc02; // pop	{r1}
	
	} else {
		(object++)[0]=0x0001; // movs	r1, r0
		(object++)[0]=0x2000; // movs	r0, #0
	}
	return call_lib_code(LIB_HEX);
}

int get_byte(void){
	unsigned char c;
	if ('0'<=source[0] && source[0]<='9') c=source[0]-'0';
	else if ('A'<=source[0] && source[0]<='F') c=source[0]-'A'+10;
	else if ('a'<=source[0] && source[0]<='f') c=source[0]-'a'+10;
	else return ERROR_SYNTAX;
	source++;
	c<<=4;
	if ('0'<=source[0] && source[0]<='9') c|=source[0]-'0';
	else if ('A'<=source[0] && source[0]<='F') c|=source[0]-'A'+10;
	else if ('a'<=source[0] && source[0]<='f') c|=source[0]-'a'+10;
	else return ERROR_SYNTAX;
	source++;
	return (int)c;
}

int string_char(){
	unsigned char c;
	c=source[0];
	source++;
	switch(c){
		case '"':
			return 0;
		case '\\':
			break;
		case 0:
			return ERROR_SYNTAX;
		default:
			return (int)c;
	}
	// \ is found
	c=source[0];
	source++;
	switch(c){
		case 'n':
			return '\n';
		case '0':
			return '\0';
		case 'x':
			return get_byte();
		default:
			return (int)c;
	}
}

int get_simple_string(void){
	int e,i,vn;
	unsigned char c;
	skip_blank();
	if ('"'==source[0]) {
		// Constant string
		i=0;
		source++;
		// Get string pointer in R0
		object[i++]=0x4678;//      	mov	r0, pc
		object[i++]=0x3002;//      	adds	r0, #2
		// Write branch instruction
		object[i++]=0xe000; // b.n xxxx
		while(1){
			e=string_char();
			if (e<0) return e;
			if (0==e) {
				object[i++]=0x0000;
				break;
			}
			c=e;
			e=string_char();
			if (e<0) return e;
			if (0==e) {
				object[i++]=c;
				break;
			}
			object[i++]=(e<<8)|c;
		}
		// Update branch instruction
		object[2]|=i-2-2;
		// Check if object area remains
		check_object(i);
		// Move object position
		object+=i;
		// Return without error
		return 0;
	} else if ('A'<=source[0] && source[0]<'Z' || '_'==source[0]) {
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) return static_method_or_property(vn,'$');
		// Variable or function
		vn=get_var_number();
		if (0<=vn) {
			if ('.'==source[0]) {
				source++;
				return method_or_property(vn,'$');
			}
			// This is a variable
			if ('$'!=source[0]) return ERROR_SYNTAX;
			source++;
			// TODO: support part of string
			return variable_to_r0(vn);
		} else {
			// This must be a function
			e=string_functions();
			if (e) return e;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	}
	return ERROR_SYNTAX;
}

int get_string(void){
	int e;
	e=get_simple_string();
	if (e) return e;
	skip_blank();
	// Only '+' can be used as an operator
	while('+'==source[0]) {
		source++;
		check_object(1);
		(object++)[0]=0xb401; // push	{r0}
		e=get_simple_string();
		if (e) return e;
		check_object(1);
		(object++)[0]=0xbc02; // pop	{r1}
		e=call_lib_code(LIB_ADD_STRING);
		if (e) return e;
		skip_blank();
	}
	return 0;
}