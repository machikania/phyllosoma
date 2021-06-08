/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_byte(){
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

int get_string(void){
	int e;
	unsigned char c;
	skip_blank();
	int i=0;
	// Detect '"'
	if (source[0]!='"') return ERROR_SYNTAX;
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
	// Move object position
	object+=i;
	// Return without error
	return 0;
}
