/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_simple_integer(void){
	int i;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_integer();
		if (i) return i;
		check_object(1);
		(object++)[0]=0x4240; // negs	r0, r0
		return 0;
	}
	if ('$'==source[0] || '0'==source[0] && 'X'==source[1]) {
		// Hex value
		source+=('$'==source[0]) ? 1:2;
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=(i<<4)|(source[0]-'0');
				source++;
			} else if ('A'<=source[0] && source[0]<='F') {
				i=(i<<4)|(source[0]-'A'+10);
				source++;
			} else {
				break;
			}
		}
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9') {
		// Decimal value
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=i*10+(source[0]-'0');
				source++;
			} else {
				break;
			}
		}
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9' || '_'==source[0]) {
		// Variable or function
		// TODO: here
		return ERROR_UNKNOWN;
	}
	return 0;
}

/*
// r0=1-r0
100003bc:	2301      	movs	r3, #1
100003be:	1a18      	subs	r0, r3, r0

// NOT
100003bc:	4243      	negs	r3, r0
100003be:	4158      	adcs	r0, r3


100003c0:	b081      	sub	sp, #4
100003c2:	b082      	sub	sp, #8
100003c4:	b001      	add	sp, #4
100003c6:	b002      	add	sp, #8
*/

int get_integer_sub(int pr){
	unsigned char* prevpos;
	int e,op;
	skip_blank();
	// Get a value in r0
	e=get_simple_integer();
	if (e) return e;
	while(1){
		// Get the operator in op. If not valid operator, simply return without error.
		prevpos=source;
		op=get_operator();
		if (op<0) return 0;
		// Compair current and previous operators.
		// If the previous operator has higher priolity, return.
		if (pr>=priority(op)) {
			source=prevpos;
			return 0;
		}
		// Store r0 in stack
		check_object(1);
		(object++)[0]=0x9000 | g_sdepth; // str	r0, [sp, #xx]
		g_sdepth++;
		if (g_maxsdepth<g_sdepth) g_maxsdepth=g_sdepth;
		// Get next value.
		e=get_integer_sub(priority(op));
		if (e) return e;
		// Get value from stack to $v1.
		g_sdepth--;
		check_object(1);
		(object++)[0]=0x9900 | g_sdepth; // ldr	r1, [sp, #xx]
		// Calculation. Result will be in $v0.
		e=calculation(op);
		if (e) return e;
	}
}

int get_integer(void){
	// This is only the public function.
	// Note that this can be called recursively.
	// Value may contain function with a parameter of another value.
	int e;
	unsigned short s;
	unsigned short* prevpos=object;
	if (g_sdepth==0) {
		// Initialize stack handler
		g_maxsdepth=0;
		// Stack decrement command will be filled later
		check_object(1);
		(object++)[0]=0x46c0; // nop			; (mov r8, r8)
	}
	e=get_integer_sub(priority(OP_VOID));
	if (e) return e;
	if (0==g_sdepth) {
		if (g_maxsdepth) {
			// Stack was used.
			prevpos[0]=0xb080 | g_maxsdepth;    // sub	sp, #xx
			check_object(1);
			(object++)[0]=0xb000 | g_maxsdepth; // add	sp, #4
		} else if (g_allow_shift_obj) {
			// Stack wasn't used.
			// Delete the nop and shift the code
			while(prevpos<object) {
				s=prevpos[1];
				(prevpos++)[0]=s;
			}
			object--;
		}
	}
	return 0;
}
