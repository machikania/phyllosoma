/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./display.h"

/*
	Class
*/

int useclass_statement(void){
	int i,e;
	do {
		e=get_class_number();
		if (e<0) {
			// Class not found.
			// Compile it
			return init_class_compiling();
		}
		skip_blank();
	} while (','==(source++)[0]);
	source--;
	return 0;
}

/*
	POKE statements
*/

int poke_main(unsigned int code){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401; // push	{r0}
	if (','!=source[0]) return ERROR_SYNTAX;
	source++;
	e=get_integer();
	if (e) return e;
	check_object(2);
	(object++)[0]=0xbc02; // pop	{r1}
	(object++)[0]=code;   // str/strh/strb	r0, [r1, #0]
	return 0;
}

int poke_statement(void){
	return poke_main(0x7008); // strb	r0, [r1, #0]
}

int poke16_statement(void){
	return poke_main(0x8008); // strh	r0, [r1, #0]
	return 0;
}

int poke32_statement(void){
	return poke_main(0x6008); // str	r0, [r1, #0]
}

/*
	VAR statement
	
	r1 is the pointer to array containing variable data
	r1[0]; Lower half: variable number; Upper half: permanent block to store string data
	r1[1]; Variable value
	r2 is number of variables to push
	
	Structure is as follows:
		1. create stack
		2. lib_var_push
		3. bl skip
		4. lib_var_pop
		5. delete stack
		6. pop {pc} (return to the next of original gosub code)
		skip:
		7. push {lr}
		...
		(some code here)
		...
		pop {pc} (return to lib_var_pop above, the next of bl above)
*/

int var_statement(void){
	int i,e,vn;
	unsigned short* subsp;
	unsigned short* bl;
	// Push routine follows
	check_object(1);
	subsp=object;
	object++; // sub	sp, #xx
	i=0;
	do {
		vn=get_var_number();
		if (vn<0) return vn;
		if ('#'==source[0] || '$'==source[0]) source++;
		e=set_value_in_register(0,vn);
		if (e<0) return e;
		check_object(1);
		(object++)[0]=0x9000 | i*2; // str	r0, [sp, #xx]
		skip_blank();
		i++;
	} while(','==(source++)[0]);
	source--;
	subsp[0]=0xb080 | i*2; // sub	sp, #xx
	e=set_value_in_register(2,i);
	if (e<0) return e;
	check_object(1);
	(object++)[0]=0x4669;         // mov	r1, sp
	e=call_lib_code(LIB_VAR_PUSH);
	if (e<0) return e;
	// BL instruction
	bl=object;
	object+=2;
	// Return destination is here
	// POP routine follows
	e=set_value_in_register(2,i);
	if (e<0) return e;
	check_object(1);
	(object++)[0]=0x4669;         // mov	r1, sp
	e=call_lib_code(LIB_VAR_POP);
	if (e<0) return e;
	// Delete stack
	check_object(2);
	(object++)[0]=0xb000 | i*2; // add	sp, #xx
	// Return
	(object++)[0]=0xbd00; // pop	{pc}
	// BL jump destination is here
	update_bl(bl,object);
	check_object(1);
	(object++)[0]=0xb500; //   push	{lr}
	// All done
	return 0;
}

/*
	DATA/CDATA/RESTORE statements

	} else if (g_constant_value_flag) {
		// Label number is used
		rewind_object(obefore);
		return goto_line(g_constant_int);

*/

int goto_label(void);
int goto_line(int id);
int restore_statement(void){
	unsigned short* obefore;
	char* sbefore=source;
	int e;
	obefore=object;
	e=get_integer();
	if (e) {
		// Label is used
		rewind_object(obefore);
		source=sbefore;
		// Get BL instruction code in r0 register
		e=set_value_in_register(0,0xf800f000);
		if (e) return e;
		// Replace "0xf800f000" with BL instruction
		object-=2;
		e=goto_label();
		if (e) return e;
		// Get pc in r1 and call library
		check_object(1);
		(object++)[0]=0x4679; //      	mov	r1, pc
		return call_lib_code(LIB_RESTORE);
	} else if (g_constant_value_flag) {
		// Label number is used
		rewind_object(obefore);
		// Get BL instruction code in r0 register
		e=set_value_in_register(0,0xf800f000);
		if (e) return e;
		// Replace "0xf800f000" with BL instruction
		object-=2;
		e=goto_line(g_constant_int);
		if (e) return e;
		// Get pc in r1 and call library
		check_object(1);
		(object++)[0]=0x4679; //      	mov	r1, pc
		return call_lib_code(LIB_RESTORE);
	} else {
		// Label is flexible
		e=call_lib_code(LIB_LINE_NUM);
		if (e) return e;
		return call_lib_code(LIB_RESTORE);
	}
}

int data_statement(void){
	unsigned char c;
	int e;
	char* sbefore;
	unsigned short* obefore;
	static unsigned short* obegin;
	if (0==g_multiple_statement) {
		// Initialize DATA statement
		obegin=object;
		check_object(3);
		// BL instruction
		object+=2;
		(object++)[0]=0x462d; // Marker for DATA statement (MOV R5,R5)
	} else if (data_statement==g_multiple_statement) {
		g_multiple_statement=0;
	} else return ERROR_UNKNOWN;
	do {
		skip_blank();
		// Check if the end of line for multiple DATA statement
		if (0x00==source[0]) {
			g_multiple_statement=data_statement;
			return 0;
		}
		sbefore=source;
		obefore=object;
		if ('"'==source[0]) {
			// String
			source++;
			while(1){
				e=string_char();
				if (e<0) return e;
				if (0==e) {
					(object++)[0]=0x0000;
					break;
				}
				c=e;
				e=string_char();
				if (e<0) return e;
				if (0==e) {
					(object++)[0]=c;
					break;
				}
				(object++)[0]=(e<<8)|c;
			}
		} else {
			// 32 bit integer
			e=get_integer();
			if (e) return e;
			// Rewind object
			rewind_object(obefore);
			if (!g_constant_value_flag) {
				source=sbefore;
				return ERROR_SYNTAX;
			}
			// Got 4 byte data in g_constant_int
			check_object(2);
			(object++)[0]=g_constant_int & 0xffff;
			(object++)[0]=g_constant_int >>16;
		}
	} while (','==(source++)[0]);
	source--;
	// Update BL
	update_bl(obegin,object);
	return 0;
}

int cdata_statement(void){
	int e;
	char* sbefore;
	unsigned short* obefore;
	static unsigned short* obegin;
	static int odd;
	if (0==g_multiple_statement) {
		// Initialize CDATA statement
		obegin=object;
		check_object(3);
		// BL instruction
		object+=2;
		(object++)[0]=0x4636; // Marker for DATA statement (MOV R6,R6)
		                      // It may be 463f (MOV R7,R7; in the case of odd number of CDATA)
		odd=0;
	} else if (cdata_statement==g_multiple_statement) {
		g_multiple_statement=0;
	} else return ERROR_UNKNOWN;
	do {
		// Check if the end of line for multiple CDATA statement
		if (0x00==source[0]) {
			g_multiple_statement=cdata_statement;
			return 0;
		}
		// Continue
		sbefore=source;
		obefore=object;
		e=get_integer();
		if (e) return e;
		// Rewind object
		rewind_object(obefore);
		if (!g_constant_value_flag) {
			source=sbefore;
			return ERROR_SYNTAX;
		}
		// Check if 8 bit data
		if (g_constant_int<0 || 255<g_constant_int) {
			source=sbefore;
			return ERROR_SYNTAX;
		}
		// Got 1 byte data in g_constant_int
		if (0==odd) {
			odd=1;
			check_object(1);
			(object++)[0]=g_constant_int;
		} else {
			odd=0;
			object[-1]|=g_constant_int<<8;
		}
	} while (','==(source++)[0]);
	source--;
	if (odd) obegin[2]=0x463f;
	// Update BL
	update_bl(obegin,object);
	return 0;
}

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
	if (source[0]<'A' || 'Z'<source[0]) return ERROR_SYNTAX;
	for(num=1;'A'<=source[num] && source[num]<='Z' || '_'==source[num] || '0'<=source[num] && source[num]<='9';num++);
	// Check if reserved name
	if (check_if_reserved(source,num)) return ERROR_RESERVED_WORD;
	// It must not be variable
	if (1==num) return ERROR_SYNTAX;
	// Check if var name or class name
	if (cmpdata_nsearch_string_first(CMPDATA_VARNAME,source,num)) return ERROR_SYNTAX;
	if (cmpdata_nsearch_string_first(CMPDATA_CLASSNAME,source,num)) return ERROR_SYNTAX;
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
		g_constant_value_flag=1;
		return goto_label();
	} else if (g_constant_value_flag) {
		// Label number is used
		rewind_object(obefore);
		return goto_line(g_constant_int);
	} else {
		// Label number is flexible
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

int gosub_arguments(void){
	// Prepare argument array (R6 as the pointer)
	int e,i;
	short* obefore=object;
	// Prepare argument array (R6 as the pointer)
	// R0 may be the pointer to object
	check_object(4);
	(object++)[0]=0xb080; //      	sub	sp, #xx (this will be updated; see below)
	(object++)[0]=0x9601; //      	str	r6, [sp, #4]
	(object++)[0]=0x9000; //        str	r0, [sp, #0]
	for(i=3;','==source[0] || '('==source[0] ;i++){
		source++;
		skip_blank();
		if (3==i && ')'==source[0]) break;
		e=get_string_int_or_float();
		if (e) return e;
		check_object(1);
		(object++)[0]=0x9000 | i; // str	r0, [sp, #xx]
	}
	obefore[0]|=i; // Update sub sp,#xx assembly
	// Set number of variables
	e=set_value_in_register(0,i-3);
	if (e) return e;
	check_object(2);
	(object++)[0]=0x466e; //      	mov	r6, sp
	(object++)[0]=0x6030 | (2<<6); // str	r0, [r6, #xx]
	return i;
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
	check_object(1);      // Copy previous object
	(object++)[0]=0x6830; // ldr	r0, [r6, #0]
	i=gosub_arguments();
	if (i<0) return i;
	// GOSUB again
	safter=source;
	source=sbefore;
	e=gosub_statement_main();
	if (e) return e;
	source=safter;
	// Delete argeuement array
	return post_gosub_statement(i);
}

int post_gosub_statement(int i){
	int e;
	// Garbage collection
	check_object(1);
	(object++)[0]=0x0031;   // movs	r1, r6
	e=call_lib_code(LIB_POST_GOSUB);
	if (e) return e;
	// Delete argeuement array
	check_object(2);
	(object++)[0]=0x6876;   // ldr	r6, [r6, #4]
	(object++)[0]=0xb000|i; // add	sp, #xx
	return 0;
}

int return_statement(void){
	int e;
	if (!end_of_statement()) {
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
	if (e) return e;
	// Delete the CMPDATA_CONTINUE (see continue_statement())
	cmpdata_delete((int*)g_scratch_int[0]);
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
	// Check STEP
	if (instruction_is("STEP")) {
		// This is required for the first time checking using r2 register (see below)
		// Get integer
		e=get_integer();
		if (e) return e;
		// Move r0 to r2
		check_object(1);
		(object++)[0]=0x0002;// movs	r2, r0
	}
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
		check_object(2);
		(object++)[0]=0x0002;// movs	r2, r0
		(object++)[0]=0xb404;// push	{r2}
		// Get r0 from variable
		e=variable_to_r0(vn);
		if (e) return e;
		// Add r2 to r0
		check_object(1);
		(object++)[0]=0x1880;// adds	r0, r0, r2
		// Store r0 to variable
		e=r0_to_variable(vn);
		if (e) return e;
		check_object(1);
		(object++)[0]=0xbc04;// pop	{r2}
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
		sbefore=source;
		obefore=object;
		// Support THEN label or THEN line number
		// Note that flexible line number cannot be used
		e=goto_statement();
		if (0==e && g_constant_value_flag && end_of_statement()) {
			if (instruction_is("ELSE")) {
				e=else_statement();
				if (e) return e;
				e=goto_statement();
				if (e) return e;
				if ((!g_constant_value_flag) || !end_of_statement()) return ERROR_SYNTAX;
			}
		} else {
			// Ordinary statemen(s) follow(s) after THEN
			source=sbefore;
			rewind_object(obefore);
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

int usevar_statement_main(int for_class){
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
		if (check_if_reserved(source,num)) return ERROR_RESERVED_WORD;
		// It must not be registered yet
		if (cmpdata_nsearch_string_first(CMPDATA_VARNAME,source,num)) return ERROR_VARNAME_USED;
		// Get a new var number
		data16=get_new_varnum();
		if (!data16) return ERROR_TOO_MANY_VARS;
		// Insert a new cmpdata
		e=cmpdata_insert_string(CMPDATA_VARNAME,data16,source,num);
		if (e) return e;
		// Class related
		switch(for_class){
			case CLASS_STATIC:
				e=register_class_static_field(data16);
				if (e) return e;
				break;
			case CLASS_FIELD | CLASS_PUBLIC:
			case CLASS_FIELD:
				e=register_class_field(data16,for_class);
				if (e) return e;
				break;
			default:
				break;
		}
		// Done. Check next
		source+=num;
		if ('$'==source[0] || '#'==source[0]) source++;
		skip_blank();
		if (source[0]!=',') break;
		source++;
	}
	return 0;
}

int usevar_statement(void){
	return usevar_statement_main(0);
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
			e=set_value_in_register(1,vn);
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
		if (0x00==source[i] || '"'==source[i]) return ERROR_SYNTAX;
		if (':'==source[i]) {
			if (':'!=source[i]) return ERROR_SYNTAX;
			i++;
		}
	}
	i=get_var_number();
	if (i<0) {
		// Try class
		i=get_class_number();
		if (i<0) return i;
		// Try class static property
		i=static_property_var_num(i);
		if (i<0) return i;
		// It's a class static property
		// i is the var number of this
	}
	switch(source[0]){
		case '$': // string
			source++;
			skip_blank();
			return let_string(i);
		case '#': // float
			source++;
			skip_blank();
			return let_float(i);
		case '.': // object
			source++;
			return let_object(i);
		case '(': // integer array
		default: // integer
			skip_blank();
			return let_integer(i);
	}
}

int print_statement(int lib) {
	// Mode; 0x00: ingeger, 0x01: string, 0x02: float
	// Mode; 0x00: CR, 0x10: ';', 0x20: ','
	int e;
	unsigned char mode;
	unsigned short* ob;
	unsigned char* sb=source;
	// Support PRINT without argument
	if (end_of_statement()) {
		source=sb;
		e=set_value_in_register(0,0);
		if (e) return e;
		e=set_value_in_register(1,1);
		if (e) return e;
		return call_lib_code(lib);
	}
	while(1){
		sb=source;
		ob=object;
		mode=0x01;
		e=get_string();
		if (e || ','!=source[0] && ';'!=source[0] && !end_of_statement()) {
			source=sb;
			rewind_object(ob);
			mode=0x00;
			e=get_integer();
		}
		if (e || ','!=source[0] && ';'!=source[0] && !end_of_statement()) {
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
		e=call_lib_code(lib);
		if (e) return e;
		if (0x00==mode&0xf0) break;
		if (end_of_statement()) break;
	}
	return 0;
}

/*
	Class related

*/

int field_statement(void){
	if (instruction_is("PRIVATE")) return usevar_statement_main(CLASS_FIELD);
	// "FIELD PUBLIC" is the same as "FIELD"
	instruction_is("PUBLIC");
	return usevar_statement_main(CLASS_FIELD | CLASS_PUBLIC);
}

int static_statement(void){
	// "STATIC PRIVATE" is the same as "USEVAR"
	if (instruction_is("PRIVATE")) return usevar_statement_main(0);
	// "STATIC PUBLIC" is the same as "STATIC"
	instruction_is("PUBLIC");
	return usevar_statement_main(CLASS_STATIC);
}

int call_statement(void){
	// This routine also checks the end of statement
	// If error occurs, rewind and return error
	char* sbefore=source;
	unsigned short* obefore=object;
	int e;
	e=get_integer();
	if (end_of_statement() && !e) return 0;
	source=sbefore;
	rewind_object(obefore);
	if (!e) return ERROR_SYNTAX;
	return e;
}

int method_statement(void){
	int e;
	e=method_statement_main();
	if (e) return e;
	return label_statement();
}

int system_statement(void){
	int e;
	e=get_integer();
	if (e) return e;
	check_object(1);
	(object++)[0]=0xb401; // push	{r0}
	skip_blank();
	if (','!=source[0]) return ERROR_SYNTAX;
	source++;
	e=get_integer();
	if (e) return e;
	check_object(2);
	(object++)[0]=0x0001; // movs	r1, r0
	(object++)[0]=0xbc01; // pop	{r0}
	return call_lib_code(LIB_SYSTEM);
}

/*
	Misc
*/

int debug_statement(void){
#ifdef DEBUG_MODE
	g_default_args[1]=0;
	g_default_args[2]=0;
	g_default_args[3]=0;
	return argn_function(LIB_DEBUG,ARG_INTEGER_OPTIONAL<<ARG1 | ARG_INTEGER_OPTIONAL<<ARG2 | ARG_INTEGER_OPTIONAL<<ARG3);
#else
	return ERROR_SYNTAX;
#endif
}

int idle_statement(void){
	check_object(1);
	(object++)[0]=0xbf30; // wfi
	return 0;
}

int wait_statement(void){
	return argn_function(LIB_WAIT,ARG_INTEGER<<ARG1);
}

int rem_statement(void){
	while(source[0]) source++;
	return 0;
}

int end_statement(void){
	return call_lib_code(LIB_END);
}

int dim_statement(void){
	unsigned short* obefore;
	int i,e,vn;
	do {
		vn=get_var_number();
		if (vn<0) return vn;
		if ('#'==source[0]) source++;
		if ('('!=source[0]) return ERROR_SYNTAX;
		source++;
		obefore=object++; // sub sp,#xx
		i=0;
		do {
			e=get_integer();
			if (e) return e;
			(object++)[0]=0x9000 | i; // str	r0, [sp, #xx]
			i++;
		} while (','==(source++)[0]);
		source--;
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		// R1 is var number
		e=set_value_in_register(1,vn);
		if (e) return e;
		// R0 is number of integer values
		set_value_in_register(0,i);
		// R2 is pointer of data array
		check_object(1);
		(object++)[0]=0x466a; // mov	r2, sp
		e=call_lib_code(LIB_DIM);
		obefore[0]=0xb080 | i; // sub	sp, #xx
		(object++)[0] =0xb000 | i; // add	sp, #xx
	} while (','==(source++)[0]);
	source--;
	return 0;
}

int end_of_statement(void){
	skip_blank();
	if (0x00==source[0]) return 1;
	if (':'==source[0] && ':'!=source[1]) return 1;
	if ('E'==source[0] && 'L'==source[1] && 'S'==source[2] && 'E'==source[3] && ' '==source[4]) return 1;
	if ('T'==source[0] && 'H'==source[1] && 'E'==source[2] && 'N'==source[3] && (' '==source[4] || 0x00==source[4])) return 1;
	if ('S'==source[0] && 'T'==source[1] && 'E'==source[2] && 'P'==source[3] && ' '==source[4]) return 1;
	if ('T'==source[0] && 'O'==source[1] && ' '==source[2]) return 1;
	return 0;
}

int compile_statement(void){
	int (*f)(void) = g_multiple_statement;
	int e;
	// Initialize
	unsigned short* bobj=object;
	unsigned char* bsrc=source;
	// Check if multiple statement, first
	if (g_multiple_statement) return f();
	// Check LET statement, first
	if (instruction_is("LET")) return let_statement();
	// "LET" may be omitted.
	e=let_statement();
	if (!e) return 0;
	rewind_object(bobj);
	source=bsrc;
	// It's not LET statement. Let's continue for possibilities of the other statements.
	if (instruction_is("BREAK")) return break_statement();
	if (instruction_is("CALL")) return call_statement();
	if (instruction_is("CDATA")) return cdata_statement();
	if (instruction_is("CONTINUE")) return continue_statement();
	if (instruction_is("DATA")) return data_statement();
	if (instruction_is("DEBUG")) return debug_statement();
	if (instruction_is("DELETE")) return delete_statement();
	if (instruction_is("DIM")) return dim_statement();
	if (instruction_is("DO")) return do_statement();
	if (instruction_is("DRAWCOUNT")) return drawcount_statement();
	if (instruction_is("ELSE")) return else_statement();
	if (instruction_is("ELSEIF")) return elseif_statement();
	if (instruction_is("END")) return end_statement();
	if (instruction_is("ENDIF")) return endif_statement();
	if (instruction_is("FIELD")) return field_statement();
	if (instruction_is("FOR")) return for_statement();
	if (instruction_is("GOSUB")) return gosub_statement();
	if (instruction_is("GOTO")) return goto_statement();
	if (instruction_is("IDLE")) return idle_statement();
	if (instruction_is("IF")) return if_statement();
	if (instruction_is("LABEL")) return label_statement();
	if (instruction_is("LOOP")) return loop_statement();
	if (instruction_is("METHOD")) return method_statement();
	if (instruction_is("NEXT")) return next_statement();
	if (instruction_is("POKE")) return poke_statement();
	if (instruction_is("POKE16")) return poke16_statement();
	if (instruction_is("POKE32")) return poke32_statement();
	if (instruction_is("PRINT")) return print_statement(LIB_PRINT);
	if (instruction_is("REM")) return rem_statement();
	if (instruction_is("RESTORE")) return restore_statement();
	if (instruction_is("RETURN")) return return_statement();
	if (instruction_is("STATIC")) return static_statement();
	if (instruction_is("USECLASS")) return useclass_statement();
	if (instruction_is("USEVAR")) return usevar_statement();
	if (instruction_is("VAR")) return var_statement();
	if (instruction_is("WAIT")) return wait_statement();
	if (instruction_is("WEND")) return wend_statement();
	if (instruction_is("WHILE")) return while_statement();
	// File statements
	if (instruction_is("FCLOSE")) return fclose_statement();
	if (instruction_is("FGET")) return fget_function();
	if (instruction_is("FILE")) return file_statement();
	if (instruction_is("FOPEN")) return fopen_function();
	if (instruction_is("FPRINT")) return print_statement(LIB_FPRINT);
	if (instruction_is("FPUT")) return fput_function();
	if (instruction_is("FPUTC")) return fputc_function();
	if (instruction_is("FREMOVE")) return fremove_function();
	if (instruction_is("FSEEK")) return fseek_statement();
	if (instruction_is("SETDIR")) return setdir_function();
	// Timer and interrupt statements
	if (instruction_is("USETIMER")) return usetimer_statement();
	if (instruction_is("TIMER")) return timer_statement();
	if (instruction_is("INTERRUPT")) return interrupt_statement();
	if (instruction_is("CORETIMER")) return coretimer_statement();
	// Music statements
	if (instruction_is("MUSIC")) return music_statement();
	if (instruction_is("SOUND")) return sound_statement();
	// IO statements
	e=io_statements();
	if (e!=ERROR_STATEMENT_NOT_DETECTED) return e;
	// Environment statements
	e=display_statements();
	if (e!=ERROR_STATEMENT_NOT_DETECTED) return e;
	// Try call statement
	if (!call_statement()) return 0;
	// Finally, try let statement as error may occur in LET statement.
	return let_statement();
}
