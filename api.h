/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./interface/graphlib.h"
#include "./interface/LCDdriver.h"
#include "./interface/ff.h"

#ifndef KMBASIC_COMPILER_H
#error "compiler.h" must be included before "api.h"
#endif

// fileselect.c
extern unsigned char path[];
void init_buttons(void);
unsigned char *fileselect(void);

// api.c
void fileselect_init(void);

// Prototypings follow
void printint(int i);
void printhex4(unsigned char c);
void printhex8(unsigned char c);
void printhex16(unsigned short s);
void printhex32(unsigned int i);

void _printchar(unsigned char n);
void printchar(unsigned char n);
#define printchar(a) _printchar(a)

void _printstr(unsigned char *s);
void printstr(unsigned char *s);
#define printstr(a) _printstr(a)

void _printnum(unsigned int n);
void printnum(unsigned int n);
#define printnum(a) _printnum(a)

void _printnum2(unsigned int n,unsigned char e);
void printnum2(unsigned int n,unsigned char e);
#define printnum2(a,b) _printnum2(a,b)

void _cls(void);
void cls(void);
#define cls() _cls()

void _setcursor(unsigned char x,unsigned char y,unsigned char c);
void setcursor(unsigned char x,unsigned char y,unsigned char c);
#define setcursor(a,b,c) _setcursor(a,b,c)
