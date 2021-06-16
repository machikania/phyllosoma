/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"

// Prototypings follow
void printstr(unsigned char *s);
void printchar(unsigned char c);
void printint(int i);
void printhex4(unsigned char c);
void printhex8(unsigned char c);
void printhex16(unsigned short s);
void printhex32(unsigned int i);
