/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	Local prototyping
*/
int get_value_sub(int pr,int vmode);

int get_simple_value(int vmode){
	int e;
	skip_blank();
	if ('('==source[0]) {
		source++;
		e=get_value_sub(priority(OP_VOID),vmode);
		if (e) return e;
		skip_blank();
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		return 0;
	}
	switch(vmode){
		case VAR_MODE_INTEGER:
			return get_simple_integer();
		case VAR_MODE_FLOAT:
			return get_simple_float();
		default:
			return ERROR_UNKNOWN;
	}
}

int get_value_sub(int pr, int vmode){
	unsigned char* prevpos;
	int e,op;
	skip_blank();
	// Get a value in r0
	e=get_simple_value(vmode);
	if (e) return e;
	while(1){
		// Get the operator in op. If not valid operator, simply return without error.
		prevpos=source;
		op=get_operator(vmode);
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
		e=get_value_sub(priority(op),vmode);
		if (e) return e;
		// Get value from stack to $v1.
		g_sdepth--;
		check_object(1);
		(object++)[0]=0x9900 | g_sdepth; // ldr	r1, [sp, #xx]
		// Calculation. Result will be in $v0.
		e=calculation(op,vmode);
		if (e) return e;
	}
}

int get_value(int vmode){
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
	e=get_value_sub(priority(OP_VOID),vmode);
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
