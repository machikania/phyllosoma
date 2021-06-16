/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./api.h"

void printstr(unsigned char *s){
	printf(s);
	sleep_ms(1);
}

void printchar(unsigned char c){
	printf("%c",c);
	sleep_ms(1);
}

void printint(int i){
	printf("%d",i);
	sleep_ms(1);
}

void printhex4(unsigned char c){
	printchar("0123456789ABCDEF"[c&0x0f]);
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
