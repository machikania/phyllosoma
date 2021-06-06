/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int print_statement(void) {
	int e;
	e=get_string();
	if (e) return e;
	return call_lib_code(LIB_PRINT_STR);
}

int return_statement(void){
	(object++)[0]=0xbd00;//      	pop	{pc}
	return 0;
}

int end_statement(void){
	// Return to C code
	// 1) Restore stack pointer
	// 2) Pop program counter
	// See also init_compiler() code
	(object++)[0]=0x6838;//      	ldr	r0, [r7, #0]
	(object++)[0]=0x4685;//      	mov	sp, r0
	(object++)[0]=0xbd00;//      	pop	{pc}
	return 0;
}
