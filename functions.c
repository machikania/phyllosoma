/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	General function call using library.
	Arguments can be optional
	Constants:
		ARG_NONE    0
		ARG_INTEGER 1
		ARG_FLOAT   2
		ARG_STRING  3
		ARG_OPTIONAL         4
		ARG_INTEGER_OPTIONAL 5
		ARG_FLOAT_OPTIONAL   6
		ARG_STRING_OPTIONAL  7
		ARG1      0
		ARG2      3
		ARG3      6
		ARG4      9
		ARG5      12
		ARG6      15
		LIBOPTION 24
	For example, if arg1 is integer and not optional, arg2 is optional integer, and arg3 is optional string:
		ARG_INTEGER<<ARG1 | ARG_INTEGER_OPTIONAL<<ARG2 | ARG_STRING_OPTIONAL<<ARG3
	If LIBOPTION is also used in r2 register, use like:
		ARG_INTEGER<<ARG1 | option<<LIBOPTION
	One argument call:
		r0: argument 1
	Two argument call:
		r0: argument 2
		r1: argument 1
	Three argument call:
		r0: argument 3
		r1: argument 2
		r2: argument 1
	More than three argument call:
		r0: last argument
		r1: pointer to array
	For optional arguments, set the default value(s) in:
		g_default_args[1]: default of 1st argument
		g_default_args[2]: default of 1st argument
		...
	Special feature of omitting artuments:
		Sequence of 0x80000001 and 0x80000002 as defult arguments indicates that the two parameters are omitted simultaneously.
		For example, "LINE ,50,20" means first two arguments are omitted. "LINE ,,50,20" will cause syntax error in this case.
*/

int arg_to_r0(int mode, int argpos){
	int e;
	int (*f)(void) = g_callback_args[argpos+1];
	mode>>=ARG2*argpos;
	mode&=(1<<ARG2)-1;
	skip_blank();
	if (argpos<0 || 5<argpos) return ERROR_UNKNOWN; // Max argument number is 6
	if (ARG_OPTIONAL<mode && ','==source[0]) {
		source++;
		if (g_default_args[argpos+1]==0x80000001 && g_default_args[argpos+2]==0x80000002) source--;
		return set_value_in_register(0,g_default_args[argpos+1]);
	} else if (ARG_OPTIONAL<mode && end_of_value()) {
		return set_value_in_register(0,g_default_args[argpos+1]);
	} else {
		switch(mode & (ARG_OPTIONAL-1)){
			case ARG_INTEGER:
				e=get_integer();
				break;
			case ARG_FLOAT:
				e=get_float();
				break;
			case ARG_STRING:
				e=get_string();
				break;
			default:
				if (ARG_CALLBACK==mode) e=f();
				else return ERROR_UNKNOWN;
		}
		if (e) return e;
		if (')'==source[0] || end_of_statement()) return 0;
		if (','!=source[0]) return ERROR_SYNTAX;
		source++;
		return 0;
	}
}

int arg0_function(int lib,int mode){
	mode>>=LIBOPTION;
	if (mode) set_value_in_register(2,mode);
	return call_lib_code(lib);
}

int arg1_function(int lib,int mode){
	int e;
	e=arg_to_r0(mode,0);
	if (e) return e;
	mode>>=LIBOPTION;
	if (mode) set_value_in_register(2,mode);
	return call_lib_code(lib);
}

int arg2_function(int lib,int mode){
	int e;
	e=arg_to_r0(mode,0);
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401; // push	{r0}
	e=arg_to_r0(mode,1);
	if (e) return e;
	check_object(1);
	(object++)[0]=0xbc02; // pop	{r1}
	mode>>=LIBOPTION;
	if (mode) set_value_in_register(2,mode);
	return call_lib_code(lib);
}

int arg3_function(int lib,int mode){
	int e;
	e=arg_to_r0(mode,0);
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401; // push	{r0}
	e=arg_to_r0(mode,1);
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401; // push	{r0}
	e=arg_to_r0(mode,2);
	if (e) return e;
	check_object(2);
	(object++)[0]=0xbc02; // pop	{r1}
	(object++)[0]=0xbc04; // pop	{r2}
	return call_lib_code(lib);
}

int argn_function(int lib,int mode){
	int i,e,n;
	// Count argument number
	for(n=0;((mode&((1<<LIBOPTION)-1))>>(ARG2*n));n++);
	switch(n){
		case 0:
			return arg0_function(lib,mode);
		case 1:
			return arg1_function(lib,mode);
		case 2:
			return arg2_function(lib,mode);
		case 3:
			if (0==mode>>LIBOPTION) return arg3_function(lib,mode);
		case 4:
		case 5:
		case 6:
			break;
		default:
			return ERROR_UNKNOWN;
	}
	// Prepare stack
	check_object(1);
	(object++)[0]=0xb080 | (n-1); // sub	sp, #xx
	for(i=0;i<n-1;i++){
		// Get the argument in r0
		e=arg_to_r0(mode,i);
		if (e) return e;
		// Store it in stack
		check_object(1);
		(object++)[0]=0x9000 | i; // str	r0, [sp, #xx]
	}
	// Get the last argument
	e=arg_to_r0(mode,i);
	if (e) return e;
	// Prepare r1 as pointer to the stack
	check_object(1);
	(object++)[0]=0x4669;         // mov	r1, sp
	// Set lib option if needed in r2
	mode>>=LIBOPTION;
	if (mode) set_value_in_register(2,mode);
	// Call library
	e=call_lib_code(lib);
	if (e) return e;
	// Delete stack
	check_object(1);
	(object++)[0]=0xb000 | (n-1); // add	sp, #xx
	return 0;
}

int args_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(3);
	(object++)[0]=0x3002; // adds	r0, #2
	(object++)[0]=0x0080; // lsls	r0, r0, #2
	(object++)[0]=0x5830; // ldr	r0, [r6, r0]
	return 0;
}

int gosub_function(void){
	return gosub_statement();
}

int debug_function(void){
#ifdef DEBUG_MODE
	g_default_args[1]=0;
	g_default_args[2]=0;
	g_default_args[3]=0;
	return argn_function(LIB_DEBUG,ARG_INTEGER_OPTIONAL<<ARG1 | ARG_INTEGER_OPTIONAL<<ARG2 | ARG_INTEGER_OPTIONAL<<ARG3);
#else
	return ERROR_SYNTAX;
#endif
}

