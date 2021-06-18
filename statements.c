/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	IF/ELSEIF/ELSE/ENDIF statements
*/

int insert_if_bl(void){
	g_scratch_int[0]=(int)&object[0];
	// BL instruction
	check_object(2);
	(object++)[0]=0xf000;// bl
	(object++)[0]=0xf800;// bl (continued)
	// Insert a CMPDATA_IF_BL
	return cmpdata_insert(CMPDATA_IF_BL,g_ifdepth,(int*)&g_scratch_int[0],1);
}

int insert_endif_bl(void){
	g_scratch_int[0]=(int)&object[0];
	// BL instruction
	check_object(2);
	(object++)[0]=0xf000;// bl
	(object++)[0]=0xf800;// bl (continued)
	// Insert a CMPDATA_ENDIF_BL
	return cmpdata_insert(CMPDATA_ENDIF_BL,g_ifdepth,(int*)&g_scratch_int[0],1);
}

int resolve_if_bl(void){
	int* data;
	short* bl;
	// Find a CMPDATA_IF_BL
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_IF_BL)){
		if ((data[0]&0xffff)==g_ifdepth) break;
	}
	if (!data) return ERROR_SYNTAX;
	// Found a CMPDATA_IF_BL
	bl=(short*)data[1];
	// Update it
	update_bl(bl,object);
	// Delete the cmpdata record
	cmpdata_delete(data);
	return 0;
}

int else_statement(void){
	// 1. Insert CMPDATA_ENDIF_BL
	// 2. Resolve a CMPDATA_IF_BL
	int e;
	e=insert_endif_bl();
	if (e) return e;
	return resolve_if_bl();
	
// DEBUG around here.
// Why ERROR_OBJ_TOO_LARGE after ELSE statement?
}

int elseif_statement(void){
	// 1. Insert CMPDATA_ENDIF_BL
	// 2. Resolve a CMPDATA_IF_BL
	// 3. Insert CMPDATA_ENDIF_BL
	return 0;
}

int endif_statement(void){;
	// 1. Resolve a CMPDATA_IF_BL
	// 2. Resolve all CMPDATA_ENDIF_BL(s)
	// 3. Check if not remaining
	int* data;
	short* bl;
	int e;
	// Resolve a CMPDATA_IF_BL
	// It may not exist as ELSE might take it
	resolve_if_bl();
	// Resolve all CMPDATA_ENDIF_BL(s)
	while(1){
		cmpdata_reset();
		while(data=cmpdata_find(CMPDATA_ENDIF_BL)){
			if ((data[0]&0xffff)==g_ifdepth) break;
		}
		if (!data) break;
		// Found a CMPDATA_ENDIF_BL
		bl=(short*)data[1];
		// Update it
		update_bl(bl,object);
		// Delete the cmpdata record
		cmpdata_delete(data);
	}
	// Confirm not remaining
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_IF_BL)){
		if ((data[0]&0xffff)==g_ifdepth) return ERROR_UNKNOWN;
	}
	// All done. Lower the depth
	g_ifdepth--;
	return 0;
}

int if_statement(void){
	// 1. Insert CMPDATA_IF_BL
	int e;
	char* before=source;
	// Increment the depth first
	g_ifdepth++;
	// Get int or float
	e=get_integer();
	if (0!=e || (!instruction_is("THEN"))) {
		source=before;
		e=get_float();
		if (e) return e;
		if (!instruction_is("THEN")) return ERROR_SYNTAX;
	}
	// r0 is set. Let's branch here
	check_object(2);
	(object++)[0]=0x2800;// cmp	r0, #0
	(object++)[0]=0xd101;// bne.n	skip
	// Inseret BL instruction and CMPDATA_IF_BL
	e=insert_if_bl();
	                     // skip:
	if (e) return e;
	// Check if statemet(s) remain(s)
	skip_blank();
	if (0==source[0]) {
		// No more statement. This is a multiline IF-ENDIF block
		return 0;
	} else {
		// This is a single line IF-THEN statement
		// TODO: support label/line number
		while(1){
			e=compile_statement();
			if (e) return e; // An error occured
			// Skip blank
			skip_blank();
			// Check null as the end of line
			if (source[0]==0x00) break;
			// Check ELSE
			if (instruction_is("ELSE")) {
				e=else_statement();
				if (e) return e;
				continue;
			}
			// Check ':'
			if (source[0]!=':') return ERROR_SYNTAX;
			// Continue this  line
			source++;
		}
		// ENDIF is omitted in this case
		return endif_statement();
	}
}

int usevar_statement(void){
	int e,num;
	short data16;
	int* data;
	unsigned char* str;
	while(1) {
		skip_blank();
		// Check the var name
		if (source[0]<'A' || 'Z'<source[0]) return ERROR_SYNTAX;
		for(num=1;'A'<=source[num] && source[num]<='Z' || '_'==source[num] || '0'<=source[num] && source[num]<='9';num++);
		if (1==num) return ERROR_SYNTAX;
		// It must not be registered yet
		if (cmpdata_nsearch_string_first(CMPDATA_VARNAME,source,num)) return ERROR_VARNAME_USED;
		// Get a new var number
		data16=get_new_varnum();
		if (!data16) return ERROR_TOO_MANY_VARS;
		// Insert a new cmpdata
		e=cmpdata_insert_string(CMPDATA_VARNAME,data16,source,num);
		if (e) return e;
		// Done. Check next
		source+=num;
		if ('$'==source[0] || '#'==source[0]) source++;
		skip_blank();
		if (source[0]!=',') break;
		source++;
	}
	return 0;
}

int let_integer(int vn){
	int e;
	switch((source++)[0]){
		case '=': // sinple integer
			e=get_integer();
			if (e) return e;
			return r0_to_variable(vn);
		case '(': // integer array
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_string(int vn){
	int e;
	switch((source++)[0]){
		case '=': // simple string
			e=get_string();
			if (e) return e;
			e=var_num_to_r1(vn);
			if (e) return e;
			return call_lib_code(LIB_LET_STR);
		case '(': // string array (not supported)
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_float(int vn){
	int e;
	switch((source++)[0]){
		case '=': // sinple float
			e=get_float();
			if (e) return e;
			return r0_to_variable(vn);
		case '(': // float array
		default:
			source--;
			return ERROR_SYNTAX;
	}
}
int let_statement(void){
	int i;
	// Check if there is '='
	for(i=0;source[i]!='=';i++){
		if (':'==source[i] || 0x00==source[i] || '"'==source[i]) return ERROR_SYNTAX;
	}
	i=get_var_number();
	if (i<0) return i;
	switch(source[0]){
		case '$': // string
			source++;
			skip_blank();
			return let_string(i);
		case '#': // float
			source++;
			skip_blank();
			return let_float(i);
		case '(': // integer array
		default: // integer
			skip_blank();
			return let_integer(i);
	}
}

int print_statement(void) {
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	int e;
	unsigned char mode;
	unsigned char* sb;
	unsigned short* ob;
	while(1){
		sb=source;
		ob=object;
		mode=0x01;
		e=get_string();
		if (e || ','!=source[0] && ';'!=source[0] && ':'!=source[0] && 0x00!=source[0]) {
			source=sb;
			object=ob;
			mode=0x00;
			e=get_integer();
		}
		if (e || ','!=source[0] && ';'!=source[0] && ':'!=source[0] && 0x00!=source[0]) {
			source=sb;
			object=ob;
			mode=0x02;
			e=get_float();
		}
		if (e) return e;
		skip_blank();
		if (';'==source[0]) {
			mode|=0x10;
			source++;
		} else if (','==source[0]) {
			mode|=0x20;
			source++;
		}
		check_object(1);
		(object++)[0]=0x2100|mode; // movs	r1, #xxxx
		e=call_lib_code(LIB_PRINT);
		if (e) return e;
		if (0x00==mode&0xf0) break;
		skip_blank();
		if (0x00==source[0] || ':'==source[0]) break;
	}
	return 0;
}

int debug_statement(void){
	return call_lib_code(LIB_DEBUG);
}

int return_statement(void){
	check_object(1);
	(object++)[0]=0xbd00;//      	pop	{pc}
	return 0;
}

int end_statement(void){
	return call_lib_code(LIB_END);
}

int compile_statement(void){
	int e;
	// Initialize
	unsigned short* bobj=object;
	unsigned char* bsrc=source;
	// Check LET statement, first
	if (instruction_is("LET")) return let_statement();
	// "LET" may be omitted.
	e=let_statement();
	if (!e) return 0;
	object=bobj;
	source=bsrc;
	// It's not LET statement. Let's continue for possibilities of the other statements.
	if (instruction_is("PRINT")) return print_statement();
	if (instruction_is("END")) return end_statement();
	if (instruction_is("DEBUG")) return debug_statement();
	if (instruction_is("USEVAR")) return usevar_statement();
	if (instruction_is("IF")) return if_statement();
	if (instruction_is("ELSE")) return else_statement();
	if (instruction_is("ELSEIF")) return elseif_statement();
	if (instruction_is("ENDIF")) return endif_statement();
	// Finally, try let statement again as syntax error may be in LET statement.
	return let_statement();
}

