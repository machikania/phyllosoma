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

/*
	READ(), CREAD(), READ#() functions
*/

int read_function(void){
	return argn_function(LIB_READ,ARG_NONE);
}

int cread_function(void){
	return argn_function(LIB_CREAD,ARG_NONE);
}

/*
	Array related functions
*/

int get_dim_value(void){
	int e;
	do {
		// Push pointer to variable data
		check_object(1);
		(object++)[0]=0xb401; // push	{r0}
		// Get index value
		e=get_integer();
		if (e) return e;
		check_object(3);
		(object++)[0]=0x0080; // lsls	r0, r0, #2
		(object++)[0]=0xbc02; // pop	{r1}
		(object++)[0]=0x5808; // ldr	r0, [r1, r0]
	} while (','==(source++)[0]); // Check if 2d, 3d, or more
	source--;
	return 0;
}

int get_dim_pointer(void){
	int e;
	e=get_dim_value();
	if (e) return e;
	// Replace the last assembly (see abve)
	object[-1]=0x1840; // adds	r0, r0, r1
	return 0;
}

/*
	General functions
*/

int get_int_or_float(void){
	char* sbefore=source;
	unsigned short* obefore=object;
	int e;
	// Get int or float
	e=get_integer();
	if (0!=e || !end_of_value()) {
		source=sbefore;
		rewind_object(obefore);
		e=get_float();
		if (e) return e;
		if (!end_of_value()) return ERROR_SYNTAX;
	}
	return 0;
}

int get_string_int_or_float(void){
	char* sbefore=source;
	unsigned short* obefore=object;
	int e;
	// Get string
	e=get_string();
	if (0!=e || !end_of_value()) {
		source=sbefore;
		rewind_object(obefore);
		e=get_int_or_float();
		if (e) return e;
		if (!end_of_value()) return ERROR_SYNTAX;
	}
	return 0;
}

int end_of_value(void){
	int i=end_of_statement();
	if (i) return i;
	if (','==source[0] || ')'==source[0]) return 1;
	return 0;
}

/*
	get_value() implementation
*/

// Pointer to stack subtraction code
static unsigned short* g_scodeaddr;

int value_push_r0(void){
	check_object(2);
	if (0==g_maxsdepth) {
		g_maxsdepth=1;
		g_scodeaddr=object;
		(object++)[0]=0xb080; // sub	sp, #xx
	}
	(object++)[0]=0x9000 | g_sdepth; // str	r0, [sp, #xx]
	g_sdepth++;
	if (g_maxsdepth<g_sdepth) g_maxsdepth=g_sdepth;
	return 0;
}

int value_pop_r1(void){
	check_object(1);
	g_sdepth--;
	(object++)[0]=0x9900 | g_sdepth; // ldr	r1, [sp, #xx]
	return 0;
}

int value_end_stack(void){
	if (0!=g_sdepth) return ERROR_UNKNOWN;
	if (0==g_maxsdepth) return 0;
	// Stack dec and inc codes
	check_object(1);
	g_scodeaddr[0]=0xb080 | g_maxsdepth; // sub	sp, #xx
	(object++)[0] =0xb000 | g_maxsdepth; // add	sp, #xx
	return 0;
}

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
		// Lower constant flag
		g_constant_value_flag=0;
		// Compair current and previous operators.
		// If the previous operator has higher priolity, return.
		if (pr>=priority(op)) {
			source=prevpos;
			return 0;
		}
		// Store r0 in stack
		e=value_push_r0();
		if (e) return e;
		// Get next value.
		e=get_value_sub(priority(op),vmode);
		if (e) return e;
		// Get value from stack to $v1.
		e=value_pop_r1();
		if (e) return e;
		// Calculation. Result will be in r0.
		e=calculation(op,vmode);
		if (e) return e;
	}
}

int get_value(int vmode){
	// Note that this can be called recursively.
	// Value may contain function with a parameter of another value.
	int e;
	// Prepare stack routine
	int prev_sdepth=g_sdepth;
	int prev_maxsdepth=g_maxsdepth;
	unsigned short* prev_scodeaddr=g_scodeaddr;
	g_sdepth=g_maxsdepth=0;
	// Raise constant flag, first
	g_constant_value_flag=1;
	// Get value
	e=get_value_sub(priority(OP_VOID),vmode);
	if (!e) {
		// End stack if used
		e=value_end_stack();
	}
	// Recall stack variables
	g_sdepth=prev_sdepth;
	g_maxsdepth=prev_maxsdepth;
	g_scodeaddr=prev_scodeaddr;
	// Everything done
	return e;
}
