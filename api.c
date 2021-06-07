/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
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
