/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	LABEL/GOTO/GOSUB/RETURN statements
	
	CMPDATA_LINENUM
		type:      CMPDATA_LINENUM
		len:       2
		data16:    line number
		record[1]: destinaion address
	
	CMPDATA_LABEL
		type:      CMPDATA_LABEL
		len:       2
		data16:    id
		record[1]: destinaion address
	
	CMPDATA_LABELNAME
		type:      CMPDATA_LABELNAME
		len:       n+1
		data16:    id
		record[1]: hash
		record[2]: string
		record[3]: string (continued)
		...
		record[n]: end of string
	
	CMPDATA_GOTO_NUM_BL
		type:      CMPDATA_GOTO_NUM_BL
		len:       2
		data16:    line number
		record[1]: BL assembly address
	
	CMPDATA_GOTO_LABEL_BL
		type:      CMPDATA_GOTO_LABEL_BL
		len:       2
		data16:    id
		record[1]: BL assembly address

*/

int get_label_id(void){
	int* data;
	int num,e;
	unsigned short id;
	// Check label name
	if (source[0]<'A' || 'Z'<=source[0]) return ERROR_SYNTAX;
	for(num=1;'A'<=source[num] && source[num]<='Z' || '_'==source[num] || '0'<=source[num] && source[num]<='9';num++);
	// Check if registered
	data=cmpdata_nsearch_string_first(CMPDATA_LABELNAME,source,num);
	if (!data) {
		// Register new CMPDATA_LABELNAME record
		id=cmpdata_get_id();
		e=cmpdata_insert_string(CMPDATA_LABELNAME,id,source,num);
		if (e) return e;
		// Reload CMPDATA_LABELNAME data
		data=cmpdata_nsearch_string_first(CMPDATA_LABELNAME,source,num);
		if (!data) return ERROR_UNKNOWN;
		if ((data[0]&0xffff)!=id) return ERROR_UNKNOWN;
	}
	source+=num;
	return data[0]&0xffff;
}

int label_statement(void){
	int e;
	int id;
	int* data;
	short* bl;
	// Get label id
	id=get_label_id();
	if (id<0) return id;
	// Check if this is the first time
	if (cmpdata_findfirst_with_id(CMPDATA_LABEL,id)) return ERROR_LABEL_DUPLICATED;
	// Register new CMPDATA_LABEL record
	g_scratch_int[0]=(int)object;
	e=cmpdata_insert(CMPDATA_LABEL,id,(int*)g_scratch_int,1);
	if (e) return e;
	// Resolve all CMPDATA_GOTO_LABEL_BL(s)
	while(data=cmpdata_findfirst_with_id(CMPDATA_GOTO_LABEL_BL,id)){
		// Found a CMPDATA_GOTO_LABEL_BL
		bl=(short*)data[1];
		// Update it
		update_bl(bl,object);
		// Delete the cmpdata record
		cmpdata_delete(data);
	}
	// All done
	return 0;
}

int goto_label(void){
	int id,e;
	int* data;
	// Get label id
	id=get_label_id();
	if (id<0) return id;
	// Check if LABEL already set
	data=cmpdata_findfirst_with_id(CMPDATA_LABEL,id);
	check_object(2);
	if (data) {
		// BL destination is known
		update_bl(object,(short*)data[1]);
		object+=2;
	} else {
		// BL destination is not known yet
		g_scratch_int[0]=(int)object;
		(object++)[0]=0xf000;// bl
		(object++)[0]=0xf800;// bl (continued)
		e=cmpdata_insert(CMPDATA_GOTO_LABEL_BL,id,(int*)g_scratch_int,1);
		if (e) return e;
	}
	// All done
	return 0;
}

int goto_line(int id){
	int e;
	int* data;
	// Check if line is already set
	data=cmpdata_findfirst_with_id(CMPDATA_LINENUM,id);
	check_object(2);
	if (data) {
		// BL destination is known
		update_bl(object,(short*)data[1]);
		object+=2;
	} else {
		// BL destination is not known yet
		g_scratch_int[0]=(int)object;
		(object++)[0]=0xf000;// bl
		(object++)[0]=0xf800;// bl (continued)
		e=cmpdata_insert(CMPDATA_GOTO_NUM_BL,id,(int*)g_scratch_int,1);
		if (e) return e;
	}
	// All done
	return 0;
}

int goto_statement(void){
	unsigned short* obefore;
	char* sbefore=source;
	int e;
	obefore=object;
	e=get_integer();
	if (e) {
		// Label is used
		rewind_object(obefore);
		source=sbefore;
		return goto_label();
	} else if (g_constant_value_flag) {
		// Label number is used
		rewind_object(obefore);
		return goto_line(g_constant_int);
	} else {
		// Label is flexible
		e=call_lib_code(LIB_LINE_NUM);
		if (e) return e;
		check_object(2);
		(object++)[0]=0x3001; // adds	r0, #1
		(object++)[0]=0x4700; // bx	r0
		return 0;
	}
}

int gosub_statement_main(void){
	unsigned short* opos1;
	unsigned short* opos2;
	int e;
	check_object(3);
	opos1=object;
	(object++)[0]=0xf000; //   bl lbl2
	(object++)[0]=0xf800; //   bl (continued)
	opos2=object;         // lbl1:
	(object++)[0]=0xb500; //   push	{lr}
	e=goto_statement();   //   bl (GOTO statement)
	if (e) return e;
	update_bl(opos1,object);
	check_object(2);      // lbl2:
	update_bl(object,opos2);
	object+=2;            //   bl lbl1
	return 0;
}

int gosub_statement(void){
	char* safter;
	char* sbefore=source;
	short* obefore=object;
	int i,e;
	// Check the jump destination, first
	e=get_integer();
	if (e) e=get_label_id();
	if (e<0) return e;
	skip_blank();
	// Rewind object
	rewind_object(obefore);
	// Prepare argument array (R6 as the pointer)
	check_object(3);
	(object++)[0]=0xb080; //      	sub	sp, #xx (this will be updated; see below)
	(object++)[0]=0x9601; //      	str	r6, [sp, #4]
	(object++)[0]=0x466e; //      	mov	r6, sp
	for(i=3;','==source[0];i++){
		source++;
		e=get_string_int_or_float();
		if (e) return e;
		check_object(1);
		(object++)[0]=0x6030 | (i<<6); // str	r0, [r6, #xx]
	}
	obefore[0]|=i; // Update sub sp,#xx assembly
	// GOSUB again
	safter=source;
	source=sbefore;
	e=gosub_statement_main();
	if (e) return e;
	source=safter;
	// Delete argeuement array
	(object++)[0]=0x6876;   // ldr	r6, [r6, #4]
	(object++)[0]=0xb000|i; // add	sp, #xx
	// All done
	return 0;
}

int return_statement(void){
	int e;
	skip_blank();
	if (':'!=source[0] && 0x00!=source[0]) {
		// There is a return value;
		e=get_string_int_or_float();
		if (e) return e;
	}
	check_object(1);
	(object++)[0]=0xbd00; // pop	{pc}
	return 0;
}

/*
	BREAK/CONTINUE statements

	CMPDATA_CONTINUE structure, used for storing address for BL statement destination
		type:      CMPDATA_CONTINUE
		len:       2
		data16:    depth
		record[1]: destinaion address
	
	CMPDATA_BREAK_BL structure, used for storing address of BL statement
		type:      CMPDATA_BREAK_BL
		len:       2
		data16:    depth
		record[1]: BL code address

*/

int break_statement(void){
	g_scratch_int[0]=(int)&object[0];
	// BL instruction
	check_object(2);
	(object++)[0]=0xf000;// bl
	(object++)[0]=0xf800;// bl (continued)
	// Insert a CMPDATA_BREAK_BL
	return cmpdata_insert(CMPDATA_BREAK_BL,g_fordepth,(int*)&g_scratch_int[0],1);
}

int continue_statement(void){
	int* data;
	// Find the CMPDATA_CONTINUE
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_CONTINUE)){
		if ((data[0]&0xffff)==g_fordepth) break;
	}
	if (!data) return ERROR_SYNTAX;
	// Temporarily store "data" for deleting (see contine_end_loop())
	g_scratch_int[0]=(int)data;
	// Jump to the found address
	check_object(2);
	update_bl(object,(short*)data[1]);
	object+=2;
	return 0;
}

/*
	DO/LOOP/WHILE/WEND statements
*/

int do_statement(void){
	int e;
	g_fordepth++;
	unsigned short* obefore=object;
	if (instruction_is("WHILE")) {
		e=get_int_or_float();
		if (e) return e;
		check_object(2);
		(object++)[0]=0x2800;// cmp	r0, #0
		(object++)[0]=0xd101;// bne.n	skip
		e=break_statement();
		                     // skip:
		if (e) return e;
	} else if (instruction_is("UNTIL")) {
		e=get_int_or_float();
		if (e) return e;
		check_object(2);
		(object++)[0]=0x2800;// cmp	r0, #0
		(object++)[0]=0xd001;// beq.n	skip
		e=break_statement();
		                     // skip:
		if (e) return e;
	}
	// Insert a CMPDATA_CONTINUE
	g_scratch_int[0]=(int)&obefore[0];
	return cmpdata_insert(CMPDATA_CONTINUE,g_fordepth,(int*)&g_scratch_int[0],1);
}

int contine_end_loop(void){
	int e;
	int* data;
	unsigned short* bl;
	// Continue
	e=continue_statement();
	// Delete the CMPDATA_CONTINUE (see continue_statement())
	cmpdata_delete((int*)g_scratch_int[0]);
	if (e) return e;
	// Resolve all CMPDATA_BREAK_BL(s)
	while(1){
		cmpdata_reset();
		while(data=cmpdata_find(CMPDATA_BREAK_BL)){
			if ((data[0]&0xffff)==g_fordepth) break;
		}
		if (!data) break;
		// Found a CMPDATA_BREAK_BL
		bl=(short*)data[1];
		// Update it
		update_bl(bl,object);
		// Delete the cmpdata record
		cmpdata_delete(data);
	}
	// All done
	g_fordepth--;
	return 0;
}

int loop_statement(void){
	int e;
	if (instruction_is("WHILE")) {
		e=get_int_or_float();
		if (e) return e;
		check_object(2);
		(object++)[0]=0x2800;// cmp	r0, #0
		(object++)[0]=0xd001;// beq.n	skip
	} else if (instruction_is("UNTIL")) {
		e=get_int_or_float();
		if (e) return e;
		check_object(2);
		(object++)[0]=0x2800;// cmp	r0, #0
		(object++)[0]=0xd101;// bne.n	skip
	}
	// Continue and end the loop
	return contine_end_loop();
		                     // skip:
}

int while_statement(void){
	int e;
	g_fordepth++;
	unsigned short* obefore=object;
	e=get_int_or_float();
	if (e) return e;
	check_object(2);
	(object++)[0]=0x2800;// cmp	r0, #0
	(object++)[0]=0xd101;// bne.n	skip
	e=break_statement();
	                     // skip:
	if (e) return e;
	// Insert a CMPDATA_CONTINUE
	g_scratch_int[0]=(int)&obefore[0];
	return cmpdata_insert(CMPDATA_CONTINUE,g_fordepth,(int*)&g_scratch_int[0],1);
}

int wend_statement(void){
	// Continue and end the loop
	return contine_end_loop();
}

/*
	FOR/NEXT statements
*/

int for_statement(void){
	int e,vn;
	unsigned short* bl;
	char* sbefore;
	g_fordepth++;
	// Get var number first
	vn=get_var_number();
	if (vn<0) return vn;
	// Check '='
	skip_blank();
	if ('='!=source[0]) return ERROR_SYNTAX;
	source++;
	// Get integer
	e=get_integer();
	if (e) return e;
	// Store value to variable
	e=r0_to_variable(vn);
	if (e) return e;
	// Check "TO"
	if (!instruction_is("TO")) return ERROR_SYNTAX;
	// Get integer
	sbefore=source;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401;// push	{r0}
	// Ger r0 from var
	e=variable_to_r0(vn);
	if (e) return e;
	// Insert a BL instruction here to skip codes
	bl=object;
	object+=2;
	// Insert a CMPDATA_CONTINUE
	g_scratch_int[0]=(int)&object[0];
	e=cmpdata_insert(CMPDATA_CONTINUE,g_fordepth,(int*)&g_scratch_int[0],1);
	if (e) return e;
	// Get "TO" value again
	source=sbefore;
	e=get_integer();
	if (e) return e;
	// Push r0 as "TO" value
	check_object(1);
	(object++)[0]=0xb401;// push	{r0}
	// Check "STEP"
	if (instruction_is("STEP")) {
		// Get integer
		e=get_integer();
		if (e) return e;
		// Move r0 to r2
		check_object(1);
		(object++)[0]=0x0002;// movs	r2, r0
		// Get r0 from variable
		e=variable_to_r0(vn);
		if (e) return e;
		// Add r2 to r0
		check_object(1);
		(object++)[0]=0x1880;// adds	r0, r0, r2
		// Store r0 to variable
		e=r0_to_variable(vn);
		if (e) return e;
		// BL jump here
		update_bl(bl,object);
		// Pop r1 as "TO" value
		check_object(6);
		(object++)[0]=0xbc02;// pop	{r1}
		// if r2<0 exchange r0 and r1
		(object++)[0]=0x2a00;// cmp	r2, #0
		(object++)[0]=0xda02;// bge.n	skip1
		(object++)[0]=0x0002;// movs	r2, r0
		(object++)[0]=0x0008;// movs	r0, r1
		(object++)[0]=0x0011;// movs	r1, r2
		                     // skip1:
		// Compare r0 and r1, and break if needed
		check_object(2);
		(object++)[0]=0x4281;// cmp	r1, r0
		(object++)[0]=0xda01;// bge.n	skip2
		return break_statement();
		                     // skip2:
		return 0;
	} else {
		// STEP 1
		// Get r0 from variable
		e=variable_to_r0(vn);
		if (e) return e;
		// Inc r0
		check_object(1);
		(object++)[0]=0x3001;// adds	r0, #1
		// Store r0 to var
		e=r0_to_variable(vn);
		if (e) return e;
		// BL jump to here
		update_bl(bl,object);
		// Pop r1 as "TO" value
		check_object(1);
		(object++)[0]=0xbc02;// pop	{r1}
		// Compare r0 and r1, and break if needed
		check_object(2);
		(object++)[0]=0x4281;// cmp	r1, r0
		(object++)[0]=0xda01;// bge.n	skip
		return break_statement();
		                     // skip:
	}
}

int next_statement(void){
	// Continue and end the loop
	return contine_end_loop();
}

/*
	IF/ELSEIF/ELSE/ENDIF statements

	CMPDATA_IF_BL structure
		type:      CMPDATA_IF_BL
		len:       2
		data16:    depth
		record[1]: BL code address
	
	CMPDATA_ENDIF_BL structure
		type:      CMPDATA_ENDIF_BL
		len:       2
		data16:    depth
		record[1]: BL code address

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
	char* sbefore=source;
	unsigned short* obefore=object;
	// Increment the depth first
	g_ifdepth++;
	// Get int or float
	e=get_integer();
	if (0!=e || (!instruction_is("THEN"))) {
		source=sbefore;
		rewind_object(obefore);
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

int elseif_statement(void){
	// 1. Insert CMPDATA_ENDIF_BL
	// 2. Resolve a CMPDATA_IF_BL
	// 3. Insert CMPDATA_ENDIF_BL (as new IF statement)
	int e;
	e=insert_endif_bl();
	if (e) return e;
	e=resolve_if_bl();
	if (e) return e;
	// Handle as a new IF statement without changing the depth
	g_ifdepth--;
	return if_statement();
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
		// It must not be a reserved word
		e=check_if_reserved(source,num);
		if (e) return e;
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
			e=variable_to_r0(vn);
			if (e) return e;
			e=get_dim_pointer();
			if (e) return e;
			if (')'!=source[0]) return ERROR_SYNTAX;
			source++;
			check_object(1);
			(object++)[0]=0xb401; // push	{r0}
			skip_blank();
			if ('='!=source[0]) return ERROR_SYNTAX;
			source++;
			e=get_integer();
			if (e) return e;
			check_object(1);
			(object++)[0]=0xbc02; // pop	{r1}
			(object++)[0]=0x6008; // str	r0, [r1, #0]
			return 0;
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
			e=variable_to_r0(vn);
			if (e) return e;
			e=get_dim_pointer();
			if (e) return e;
			if (')'!=source[0]) return ERROR_SYNTAX;
			source++;
			check_object(1);
			(object++)[0]=0xb401; // push	{r0}
			skip_blank();
			if ('='!=source[0]) return ERROR_SYNTAX;
			source++;
			e=get_float();
			if (e) return e;
			check_object(1);
			(object++)[0]=0xbc02; // pop	{r1}
			(object++)[0]=0x6008; // str	r0, [r1, #0]
			return 0;
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
			rewind_object(ob);
			mode=0x00;
			e=get_integer();
		}
		if (e || ','!=source[0] && ';'!=source[0] && ':'!=source[0] && 0x00!=source[0]) {
			source=sb;
			rewind_object(ob);
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
	rewind_object(bobj);
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
	if (instruction_is("DO")) return do_statement();
	if (instruction_is("LOOP")) return loop_statement();
	if (instruction_is("WHILE")) return while_statement();
	if (instruction_is("WEND")) return wend_statement();
	if (instruction_is("BREAK")) return break_statement();
	if (instruction_is("CONTINUE")) return continue_statement();
	if (instruction_is("FOR")) return for_statement();
	if (instruction_is("NEXT")) return next_statement();
	if (instruction_is("LABEL")) return label_statement();
	if (instruction_is("GOTO")) return goto_statement();
	if (instruction_is("GOSUB")) return gosub_statement();
	if (instruction_is("RETURN")) return return_statement();
	// Finally, try let statement again as syntax error may be in LET statement.
	return let_statement();
}

