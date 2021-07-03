/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"

#undef printchar
#undef printstr
#undef printnum
#undef printnum2
#undef cls

void _printstr(unsigned char *s){
	printf("%s",s);
	printstr(s);
}

void _printchar(unsigned char c){
	putchar(c);
	printchar(c);
}

void _printnum(unsigned int n){
	printf("%d",n);
	printnum(n);
}

void _printnum2(unsigned int n,unsigned char e){
	int i;
	char* buff=(char*)&g_scratch[0];
	i=snprintf(buff,sizeof g_scratch,"%d",n);
	e-=i;
	for(i=0;i<e;i++) putchar(' ');
	printf("%s",buff);
}

void _cls(void){

}

void printint(int i){
	_printnum(i);
}

void printhex4(unsigned char c){
	_printchar("0123456789ABCDEF"[c&0x0f]);
}

void printhex8(unsigned char c){
	printhex4(c>>4);
	printhex4(c);
}

void printhex16(unsigned short s){
	printhex8(s>>8);
	printhex8(s);
}

void printhex32(unsigned int i){
	printhex16(i>>16);
	printhex16(i);
}
