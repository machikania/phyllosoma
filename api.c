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
#undef setcursor

// graphlib.c
extern unsigned char *cursor;

static int g_cursor=0;

void _putchar(unsigned char c){
	switch(c){
		case 0x08: // BS
			putchar(c);
			if (0<g_cursor) g_cursor--;
			break;
		case 0x0d: // CR
			g_cursor/=80;
			g_cursor*=80;
			putchar(c);
			break;
		case 0x0a: // LF
			g_cursor+=80;
			g_cursor/=80;
			g_cursor*=80;
			putchar(c);
			break;
		default:
			g_cursor++;
			putchar(c);
			break;
	}
	while(80*24<=g_cursor) g_cursor-=80;
}

void _printchar(unsigned char c){
	_putchar(c);
	printchar(c);
}

void _printstr(unsigned char *s){
	unsigned char c;
	printstr(s);
	while(c=(s++)[0]) _putchar(c);
}

void _printnum(unsigned int n){
	unsigned char c;
	char* buff=(char*)&g_scratch[0];
	snprintf(buff,sizeof g_scratch,"%d",n);
	while(c=(buff++)[0]) _putchar(c);
	printnum(n);
}

void _printnum2(unsigned int n,unsigned char e){
	unsigned char c;
	int i;
	char* buff=(char*)&g_scratch[0];
	i=snprintf(buff,sizeof g_scratch,"%d",n);
	e-=i;
	for(i=0;i<e;i++) _putchar(' ');
	while(c=(buff++)[0]) _putchar(c);
	printnum2(n,e);
}

void _cls(void){
	int i;
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	for(i=0;i<23;i++) {
		// Go up
		putchar(0x1b); putchar(0x5b); putchar(0x41);
	}
	g_cursor=0;
	cls();
}

/*
up key:    1b 5b 41
down key:  1b 5b 42
right key: 1b 5b 43
left key:  1b 5b 44
*/
void _setcursor(unsigned char x,unsigned char y,unsigned char c){
	int i,cx,cy;
	cy=g_cursor/80;
	cx=g_cursor-cy*80;
	if (y<cy) {
		// Go up
		for(i=0;i<cy-y;i++) {
			putchar(0x1b); putchar(0x5b); putchar(0x41);
		}
	} else if (cy<y) {
		// Go down
		for(i=0;i<y-cy;i++) {
			putchar(0x1b); putchar(0x5b); putchar(0x42);
		}
	}
	if (x<cx) {
		// Go left
		for(i=0;i<cx-x;i++) {
			putchar(0x1b); putchar(0x5b); putchar(0x44);
		}
	} else if (cx<x) {
		// Go right
		for(i=0;i<x-cx;i++) {
			putchar(0x1b); putchar(0x5b); putchar(0x43);
		}
	}
	g_cursor=x+y*80;
	setcursor(x,y,c);
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
