/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./display.h"

int strncmp_function(void){
	return argn_function(LIB_STRNCMP,
		ARG_STRING<<ARG1 |
		ARG_STRING<<ARG2 |
		ARG_INTEGER<<ARG3 );
}

int val_function(void){
	return argn_function(LIB_VAL,ARG_STRING<<ARG1);
}

int len_function(void){
	return argn_function(LIB_LEN,ARG_STRING<<ARG1);
}

int int_function(void){
	return argn_function(LIB_INT,ARG_FLOAT<<ARG1);
}

int rnd_function(void){
	return call_lib_code(LIB_RND);
}


int peek_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x7800; //      	ldrb	r0, [r0, #0]
	return 0;
}

int peek16_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x8800; //      	ldrh	r0, [r0, #0]
	return 0;
}

int peek32_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0x6800; //      	ldr	r0, [r0, #0]
	return 0;
}

int asc_function(void){
	return argn_function(LIB_ASC,ARG_STRING<<ARG1);
}

int sgn_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(5);
	(object++)[0]=0x17c3; //      	asrs	r3, r0, #31
	(object++)[0]=0x1a1b; //      	subs	r3, r3, r0
	(object++)[0]=0x0fdb; //      	lsrs	r3, r3, #31
	(object++)[0]=0x0fc0; //      	lsrs	r0, r0, #31
	(object++)[0]=0x1a18; //      	subs	r0, r3, r0
	return 0;
}

int abs_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(3);
	(object++)[0]=0x17c3; //      	asrs	r3, r0, #31
	(object++)[0]=0x18c0; //      	adds	r0, r0, r3
	(object++)[0]=0x4058; //      	eors	r0, r3
	return 0;
}

int not_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(2);
	(object++)[0]=0x4243; //      	negs	r3, r0
	(object++)[0]=0x4158; //      	adcs	r0, r3
	return 0;
}

int inkey_function(void){
	g_default_args[1]=0;
	return argn_function(LIB_INKEY,ARG_INTEGER_OPTIONAL<<ARG1);
}

int keys_function(void){
	g_default_args[1]=63;
	return argn_function(LIB_KEYS,ARG_INTEGER_OPTIONAL<<ARG1);
}

int readkey_function(void){
	return call_lib_code(LIB_READKEY);
}

int system_function(void){
	int e;
	e=get_integer();
	if (e) return e;
	skip_blank();
	if (','==source[0]) {
		source++;
		check_object(1);
		(object++)[0]=0xb401; // push	{r0}
		e=get_integer();
		if (e) return e;
		check_object(2);
		(object++)[0]=0x0001; // movs	r1, r0
		(object++)[0]=0xbc01; // pop	{r0}
	}
	return call_lib_code(LIB_SYSTEM);
}

int dataaddress_function(void){
	int id,e,val;
	int* data;
	// Get label id
	id=get_label_id();
	if (id<0) return id;
	// Check if LABEL already set
	data=cmpdata_findfirst_with_id(CMPDATA_LABEL,id);
	if (data) val=data[1];
	else val=0;
	if ((int)object&0x03) {
		// Lower 2 bit of object is 0b10
		check_object(5);
		(object++)[0]=0x4801;        // ldr    r0, [pc, #4]
		(object++)[0]=0xe002;        // b.n    <skip>
		(object++)[0]=0x46c0;        // nop            ; (mov r8, r8)
		g_scratch_int[0]=(int)object;
		(object++)[0]=val&0xffff;    // lower 16 bits
		(object++)[0]=val>>16;       // upper 16 bits
		                             // <skip>:	
	} else {
		// Lower 2 bit of object is 0b00
		check_object(4);
		(object++)[0]=0x4800;        // ldr    r0, [pc, #4]
		(object++)[0]=0xe001;        // b.n    <skip>
		g_scratch_int[0]=(int)object;
		(object++)[0]=val&0xffff;    // lower 16 bits
		(object++)[0]=val>>16;       // upper 16 bits
		                             // <skip>:	
	}
	if (!val) {
		e=cmpdata_insert(CMPDATA_DATA_LABEL_BL,id,(int*)g_scratch_int,1);
		if (e) return e;
	}
	return 0;
}

int funcaddress_function(void){
	int e;
	e=dataaddress_function();
	check_object(1);
	(object++)[0]=0x3001; //	adds	r0, #1
	return 0;
}

int integer_functions(void){
	int e;
	if (instruction_is("ABS(")) return abs_function();
	if (instruction_is("ARGS(")) return args_function();
	if (instruction_is("ASC(")) return asc_function();
	if (instruction_is("CREAD(")) return cread_function();
	if (instruction_is("DEBUG(")) return debug_function();
	if (instruction_is("DATAADDRESS(")) return dataaddress_function();
	if (instruction_is("DRAWCOUNT(")) return drawcount_function();
	if (instruction_is("FUNCADDRESS(")) return funcaddress_function();
	if (instruction_is("GOSUB(")) return gosub_function();
	if (instruction_is("INKEY(")) return inkey_function();
	if (instruction_is("INT(")) return int_function();
	if (instruction_is("KEYS(")) return keys_function();
	if (instruction_is("LEN(")) return len_function();
	if (instruction_is("NEW(")) return new_function();
	if (instruction_is("NOT(")) return not_function();
	if (instruction_is("PEEK(")) return peek_function();
	if (instruction_is("PEEK16(")) return peek16_function();
	if (instruction_is("PEEK32(")) return peek32_function();
	if (instruction_is("READ(")) return read_function();
	if (instruction_is("READKEY(")) return readkey_function();
	if (instruction_is("RND(")) return rnd_function();
	if (instruction_is("SGN(")) return sgn_function();
	if (instruction_is("STRNCMP(")) return strncmp_function();
	if (instruction_is("SYSTEM(")) return system_function();
	if (instruction_is("VAL(")) return val_function();
	// File functions
	if (instruction_is("FOPEN(")) return fopen_function();
	if (instruction_is("FEOF(")) return feof_function();
	if (instruction_is("FGET(")) return fget_function();
	if (instruction_is("FGETC(")) return fgetc_function();
	if (instruction_is("FINFO(")) return finfo_function();
	if (instruction_is("FLEN(")) return flen_function();
	if (instruction_is("FPUT(")) return fput_function();
	if (instruction_is("FPUTC(")) return fputc_function();
	if (instruction_is("FREMOVE(")) return fremove_function();
	if (instruction_is("FSEEK(")) return fseek_function();
	if (instruction_is("SETDIR(")) return setdir_function();
	if (instruction_is("FRENAME(")) return frename_function();
	if (instruction_is("MKDIR(")) return mkdir_function();
	// Timer functions
	if (instruction_is("CORETIMER(")) return coretimer_function();
	if (instruction_is("TIMER(")) return timer_function();
	// Music functions
	if (instruction_is("MUSIC(")) return music_function();
	if (instruction_is("PLAYWAVE(")) return playwave_function();
	// IO functions
	e=io_functions();
	if (e!=ERROR_STATEMENT_NOT_DETECTED) return e;
	// Wifi functions
	e=wifi_int_functions();
	if (e!=ERROR_STATEMENT_NOT_DETECTED) return e;
	// Aux functions
	e=aux_int_functions();
	if (e!=ERROR_STATEMENT_NOT_DETECTED) return e;
	// Environment
	return display_functions();
}

int get_positive_decimal_value(void){
	int i;
	if (source[0]<'0' || '9'<source[0]) return ERROR_SYNTAX;
	for(i=0;'0'<=source[0] && source[0]<='9';source++) i=i*10+(source[0]-'0');
	return i;
}

int get_simple_integer(void){
	int i,vn;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_value(VAR_MODE_INTEGER);
		if (i) return i;
		check_object(1);
		(object++)[0]=0x4240; // negs	r0, r0
		g_constant_int=0-g_constant_int;
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
		g_constant_int=i;
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<='9') {
		// Decimal value
		i=get_positive_decimal_value();
		if (i<0) return i;
		g_constant_int=i;
		return set_value_in_register(0,i);
	} else if ('A'<=source[0] && source[0]<='Z' || '_'==source[0]) {
		// Lower constant flag
		g_constant_value_flag=0;
		// Class static property or method
		vn=get_class_number();
		if (0<=vn) {
			vn=static_method_or_property(vn,0);
			if (vn<=0) return vn; // Error (vn==0) or method (vn<0)
		} else {
			// Variable or function
			vn=get_var_number();
		}
		if (0<=vn) {
			// Get variable value
			i=variable_to_r0(vn);
			if (i) return i;
			// Check if an array
			if ('('==source[0]) {
				source++;
				i=get_dim_value();
				if (i) return i;
				if (')'!=source[0]) return ERROR_SYNTAX;
				source++;
			}
			// Check if an object
			if ('.'==source[0]) {
				source++;
				i=method_or_property(0);
				g_constant_value_flag=0;
				return i;
			}
			g_constant_value_flag=0;
			return 0;
		} else {
			// This must be a function
			i=integer_functions();
			if (i) return i;
			g_constant_value_flag=0;
			if (')'==(source++)[0]) return 0;
			source--;
			return ERROR_SYNTAX;
		}
	} else if ('&'==source[0]) {
		// '&' operator
		source++;
		vn=get_var_number();
		if (vn<0) return vn;
		check_object(3);
		(object++)[0]=0x2000 | vn; // movs	r0, #xx
		(object++)[0]=0x0080;      // lsls	r0, r0, #2
		(object++)[0]=0x1940;      // adds	r0, r0, r5
		g_constant_value_flag=0;
		return 0;
	}
	return ERROR_SYNTAX;
}

int get_integer(void){
	return get_value(VAR_MODE_INTEGER);
}
