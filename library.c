/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int kmbasic_library(int r0, int r1, int r2, int r3){
	switch(r3){
		case LIB_PRINT_STR:
			printstr((unsigned char*)r0);
			return r0;
		default:
			return r0;
	}
}
