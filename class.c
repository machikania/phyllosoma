/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "./compiler.h"
#include "./api.h"

/*
	CMPDATA_CLASSNAME  : class name and id
		data16   : class id
		data[1]  : hash
		data[2]- : class name string
	
	CMPDATA_FIELDNAME  : class field and method names and id
		data16   : variable number/method id
		data[1]  : hash
		data[2]- : field name string
	
	CMPDATA_CLASS      : class structure and id
		data16 :   class id
		data[1]:   upper half: method/field information, lower half: variable number/method id
		data[2]:   upper half: method/field information, lower half: variable number/method id
		data[3]:   ....
		data[n-1]: upper half: method/field information, lower half: variable number/method id
	
			method/field information (16 bit):
				#define CLASS_METHOD 0x00010000
				#define CLASS_FIELD  0x00020000
				#define CLASS_PUBLIC 0x00100000
				#define CLASS_STATIC 0x00200000
	
	CMPDATA_CLASS_ADDRESS: Address of class structure
		data16 : class id
		data[1]: address of class structure
		data[2]: address of empty object
	
	CMPDATA_METHOD     : class method address and id
		data16 : method id
		data[1]: method address to call
	
	CMPDATA_STATIC     : class static field var number
		data16 : class id
		data[1]: upper half: field id, lower half: var number
	
	To get a class structure from class name,
		1. CMPDATA_CLASSNAME to get class id
		2. CMPDATA_CLASS
	
	To get a field information from field name
		1. CMPDATA_FIELDNAME to get field id
		2. CMPDATA_CLASS to check if this class contain the field with this id
		3. continue 2. until corresponding class will be found
	
	To get a method information
		1. CMPDATA_FIELDNAME to get field id
		2. CMPDATA_CLASS to check if this class contain the field with this id
		3. continue 2. until corresponding class will be found
		4. CMPDATA_METHOD to find address to call
	
	To get a static field information
		1. CMPDATA_CLASSNAME to get class id
		2. Get class information from CMPDATA_CLASS
		3. Pick up variable numbers of static field from class structure
		4. CMPDATA_FIELDNAME to get field id
		5. Check if id of 4 matches to id of 3
		6. Resolve var number by using CMPDATA_STATIC
	
*/

/*
	Structure of object
		data[0]:   upper half: length of object (n), lower half: class id
		data[1]:   private/public field value
		data[2]:   private/public field value
		...
		data[n-1]: private/public field value
*/

/*
	Inilialize compiler
*/

int init_class_compiling(void){
	int e,i,num;
	// Let's open the class file
	num=length_of_field();
	g_class_file=cmpdata_insert_string_stack(num+5); // add ".bas\0"
	if (!g_class_file) return ERROR_UNKNOWN;
	// Create the file name to open
	for(i=0;i<num;i++) g_class_file[i]=source[i];
	g_class_file[i++]='.';
	g_class_file[i++]='B';
	g_class_file[i++]='A';
	g_class_file[i++]='S';
	g_class_file[i++]=0x00;
	// Determine class id
	g_class_id=cmpdata_get_id();
	// Register class name
	e=cmpdata_insert_string(CMPDATA_CLASSNAME,g_class_id,source,num);
	if (e) return e;
	source+=num;
	// Register empty class information
	e=cmpdata_insert(CMPDATA_CLASS,g_class_id,0,0);
	// Return to start opening class file
	return ERROR_COMPILE_CLASS;
}

/*
	Post compiling a class and classes

*/

int post_compilling_a_class(void){
	int* data;
	int* class_structure;
	int* empty_object;
	int i,j,num;
	// Construct the class structure
	data=cmpdata_findfirst_with_id(CMPDATA_CLASS,g_class_id);
	num=(data[0]>>16)&0xff;
	if (((int)object)&0x02) {
		// Class structure is 32 bit array though object is 16 bit array
		check_object(1);
		object++;
	}
	class_structure=(int*)object;
	check_object(num*2);
	for(i=j=1;i<num;i++) {
		// Check if not static field
		if (data[i]&CLASS_STATIC) continue;
		// This is not the static field, so it must be either normal field or method
		class_structure[j++]=data[i];
	}
	num=j;
	object+=num*2;
	class_structure[0]=num-1;
	// Note: CMPDATA_CLASS record will be needed in new function
	// Construct an empty object
	empty_object=(int*)object;
	check_object(num*2);
	object+=num*2;
	empty_object[0]=(int)class_structure;
	for(i=1;i<num;i++){
		if (class_structure[i]&CLASS_FIELD) {
			// Initialize field value of 0
			empty_object[i]=0;
		} else if (class_structure[i]&CLASS_METHOD) {
			// Initialize method pointer
			data=cmpdata_findfirst_with_id(CMPDATA_METHOD,class_structure[i]&0xffff);
			if (!data) return ERROR_UNKNOWN;
			empty_object[i]=data[1];
			cmpdata_delete(data); // This CMPDATA_METHOD record won't be needed any more
		}
	}
	// Construct CMPDATA_CLASS_ADDRESS
	i=cmpdata_insert(CMPDATA_CLASS_ADDRESS,g_class_id,0,3);
	if (i) return i;
	data=cmpdata_current_record();
	data[1]=(int)class_structure;
	data[2]=(int)empty_object;
	// All done
	return 0;
}

int post_compilling_classes(void){
	int* data;
	int i,num;
	// Construct g_class_id_list array
	g_class_id_list=object;
	cmpdata_reset();
	num=0;
	for(num=0;data=cmpdata_find(CMPDATA_CLASSNAME);num++){
		check_object(1);
		(object++)[0]=data[0]&0xffff;
	}
	check_object(1);
	(object++)[0]=0;
	if (((int)object)&0x02) {
		// Following structures are 32 bit arrays though object is 16 bit array
		check_object(1);
		object++;
	}
	// Construct g_class_list array and g_empty_object_list array
	check_object(num*4);
	g_class_list=(int*)object;
	object+=num*2;
	g_empty_object_list=(int*)object;
	object+=num*2;
	for(i=0;i<num;i++){
		data=cmpdata_findfirst_with_id(CMPDATA_CLASS_ADDRESS,g_class_id_list[i]);
		if (!data) return ERROR_UNKNOWN;
		g_class_list[i]=data[1];
		g_empty_object_list[i]=data[2];
	}
	// All done
	return 0;
}
/*
	Library functions
*/

int lib_resolve_field_address(int r0, int r1, int r2){
	// This function resolves the address of public field or method address
	// r0: address of object
	// r1: field id
	unsigned int* object=(unsigned int*)r0;
	unsigned short field_id=r1;
	unsigned int* class_structure=(unsigned int*)object[0];
	int num=class_structure[0];
	int i;
	for(i=1;1<=num;i++){
		if (field_id==(class_structure[i]&0xffff)) break;
	}
	if (num<i) stop_with_error(ERROR_NOT_FIELD);
	if (!(class_structure[i]&CLASS_PUBLIC)) stop_with_error(ERROR_NOT_PUBLIC);
	return (int)(&object[i]);
}

int lib_resolve_method_address(int r0, int r1, int r2){
	r0=lib_resolve_field_address(r0,r1,r2);
	return ((int*)r0)[0];
}

int lib_new(int r0, int r1, int r2){
	unsigned short class_id=r0;
	int* data;
	int* object;
	int i,num;
	// Get class structure
	for(i=0;g_class_id_list[i];i++){
		if (g_class_id_list[i]==class_id) break;
	}
	if (!g_class_id_list[i]) stop_with_error(ERROR_NOT_OBJECT);
	data=(int*)g_class_list[i];
	num=data[0];
	// Get empty object
	data=(int*)g_empty_object_list[i];
	// Create the object
	i=get_permanent_block_number();
	object=calloc_memory(num+1,i);
	// Update the object
	for(i=0;i<=num;i++) object[i]=data[i];
	return (int)object;
}

void obj2var(int* obj){
	// Update variables by object fields
	int i;
 	unsigned char var_num;
	int* class=(int*)obj[0];
	int num=class[0];
	for(i=1;i<=num;i++){
		// Must be field
		if (class[i]&CLASS_FIELD) {
			// Get variable number
			var_num=class[i]>>24;
			// Update variable
			kmbasic_variables[var_num]=obj[i];
		}
	}
}

void var2obj(int* obj){
	// Update object fields by variables
	int i;
	unsigned char var_num;
	int* class=(int*)obj[0];
	int num=class[0];
	for(i=1;i<=num;i++){
		// Must be field
		if (class[i]&CLASS_FIELD) {
			// Get variable number
			var_num=class[i]>>24;
			// Update object field
			obj[i]=kmbasic_variables[var_num];
		}
	}
}

int lib_pre_method(int r0, int r1, int r2){
	int* r6=(int*)r1;
	int* obj=(int*)r6[0];
	int* prev_r6=(int*)r6[1];
	int* caller=(int*)prev_r6[0];
	// If method calling in an object method, store variables to object
	if (caller) var2obj(caller);
	// Get variables from object
	if (obj) obj2var(obj);
	return r0;
}

int lib_post_method(int r0, int r1, int r2){
	int* r6=(int*)r1;
	int* obj=(int*)r6[0];
	int* prev_r6=(int*)r6[1];
	int* caller=(int*)prev_r6[0];
	// Store variables to object
	if (obj) var2obj(obj);
	// If medthod calling in an object method, get variables from object
	if (caller) obj2var(caller);
	return r0;
}

/*
	General functions follow
*/

int length_of_field(void){
	int num;
	if ('0'<=source[0] && source[0]<='9') return 0;
	for(num=0;'_'==source[num] || 'A'<=source[num] && source[num]<='Z' || '0'<=source[num] && source[num]<='9'; num++);
	return num;
}

int get_class_number(void){
	int num;
	int* data;
	skip_blank();
	num=length_of_field();
	if (!num) return ERROR_NOT_OBJECT;
	data=cmpdata_nsearch_string_first(CMPDATA_CLASSNAME,source,num);
	if (!data) return ERROR_NOT_OBJECT;
	source+=num;
	return data[0]&0xffff;
}

/*
	Method calling routine
*/

int static_method(int method_address){
	// Note that '(' remains at the source position
	// static_flag is set when static method will be called
	// R0 contains the pointer to object
	int e,argnum;
	unsigned short* opos2;
	// No object
	check_object(1);
	(object++)[0]=0x2000; // movs	r0, #0
	// Prepare arguments
	argnum=gosub_arguments();
	if (argnum<0) return argnum;
	// Call the method
	check_object(6);
	(object++)[0]=0xe002; // b.n    <lbl2>
	                      // lbl1:
	(object++)[0]=0xb500; // push	{lr}
	opos2=object;
	object++;             // bl
	object++;             // bl
	                      // lbl2:
	(object++)[0]=0xF7FF; // bl <lbl1>
	(object++)[0]=0xFFFB; // bl (continued)
	update_bl(opos2,(unsigned short*)method_address);
	// Post gosub and return
	g_class_mode=CLASS_PUBLIC | CLASS_METHOD | CLASS_STATIC;
	return post_gosub_statement(argnum);
}

/*
	Static method/field
*/

int resolve_var_number_from_id(unsigned short cn, unsigned short id){
	int* data;
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_STATIC)){
		if ((data[0]&0xffff)!=cn) continue;
		if (((data[1]>>16)&0xffff)!=id) continue;
		// Found the CMPDATA
		break;
	}
	if (!data) return ERROR_UNKNOWN;
	return data[1]&0xffff;
}

int static_method_or_property(int cn, char stringorfloat){
	// This function returns variable number for static property
	// or returns zero when method is compiled
	int num,len,i;
	int* data;
	int* class;
	// "::" has not been detected before comming to this line
	// stringorfloat is either 0, '$', or '#' for integer, string, or float
	if (cn<0) return cn;
	if (':'!=source[0] || ':'!=source[1]) return ERROR_SYNTAX;
	source+=2;
	// Get field name
	num=length_of_field();
	if (!num) return ERROR_NOT_OBJECT;
	// Get class structure
	class=cmpdata_findfirst_with_id(CMPDATA_CLASS,cn);
	if (!class) return ERROR_NOT_OBJECT;
	len=(class[0]>>16)&0xff;
	// Check out class fields/methods
	for(i=1;i<len;i++){
		// Get field/method name
		data=cmpdata_findfirst_with_id(CMPDATA_FIELDNAME,class[i]&0xffff);
		if (!data) continue;
		// check if the names match
		if (strncmp(source,(char*)&data[2],num)) continue;
		// The name matches. Let's break.
		break;
	}
	if (len<=i) return ERROR_NOT_FIELD;
	source+=num;
	if (stringorfloat) {
		if (stringorfloat!=source[0]) return ERROR_SYNTAX;
		source++;
	}
	// Check if public
	if (!(class[i]&CLASS_PUBLIC)) return ERROR_NOT_PUBLIC;
	// Check if method
	if (class[i]&CLASS_METHOD) {
		// This is public class method.
		if ('('!=source[0]) return ERROR_SYNTAX;
		data=cmpdata_findfirst_with_id(CMPDATA_METHOD,class[i]&0xffff);
		if (!data) return ERROR_UNKNOWN;
		// Compile method
		i=static_method(data[1]);
		if (i) return i;
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		g_class_mode=CLASS_PUBLIC | CLASS_STATIC | CLASS_METHOD;
		return 0;
	}
	// Check if static field
	if (!(class[i]&CLASS_STATIC)) return ERROR_SYNTAX;
	if (!(class[i]&CLASS_FIELD)) return ERROR_NOT_FIELD;
	// This is a public static field
	// Resolve var number from field ID
	num=resolve_var_number_from_id(cn,class[i]&0xffff);
	if (num<0) return num; // Error
	// All done. Let's finish.
	g_class_mode=CLASS_PUBLIC | CLASS_STATIC | CLASS_FIELD;
	g_scratch_int[0]=num; // Store variable name to scratch int
	return variable_to_r0(num);
}

int static_property_var_num(int cn){
	int e;
	char* sbefore=source;
	unsigned short* obefore=object;
	e=static_method_or_property(cn,0);
	if (e) return e;
	object=obefore;
	if (g_class_mode!=(CLASS_PUBLIC | CLASS_STATIC | CLASS_FIELD)) {
		source=sbefore;
		return ERROR_SYNTAX;
	}
	return g_scratch_int[0]; // See above
}

/*
	Calling method/field
*/

int field_id(void){
	// This function will return field id if valid filed
	// '.' has been detected before comming this line
	int num;
	int* data;
	// Get field name
	num=length_of_field();
	if (!num) return ERROR_NOT_OBJECT;
	// Get field/method name
	data=cmpdata_nsearch_string_first(CMPDATA_FIELDNAME,source,num);
	if (!data) return ERROR_NOT_FIELD;
	// Found an field id
	source+=num;
	return data[0]&0xffff;
}

int get_pointer_to_field(void){
	// After calling this function, R0 will be the pointer to field
	// R0 must be pointer to object before calling this function
	// '.' has been detected before comming this line
	int e,fid;
	// Get the field id
	fid=field_id();
	if (fid<0) return fid; // ERROR
	// R1 will be field id
	e=set_value_in_register(1,fid);
	if (e) return e;
	// Now, R0 is the pointer to object, R1 is field id. Let's call the library
	return call_lib_code(LIB_OBJ_FIELD);
}

int call_object_method(int fid){
	int argnum,e;
	// R0 is the pointer to object, now
	// Prepare the arguments in R6
	argnum=gosub_arguments();
	if (argnum<0) return argnum; // error
	// Get the pointer to object back in R0
	check_object(1);
	(object++)[0]=0x6830; // ldr	r0, [r6, #0]
	// R1 will be field/method id
	e=set_value_in_register(1,fid);
	if (e) return e;
	// Now, R0 is the pointer to object, R1 is field/method id. Let's call the library
	// to get the address to method in R0
	e=call_lib_code(LIB_OBJ_METHOD);
	if (e) return e;
	// Now, R0 is the address of method. Call it
	check_object(6);
	(object++)[0]=0xe002; // b.n    <lbl2>
	                      // lbl1:
	(object++)[0]=0xb500; // push	{lr}
	(object++)[0]=0x4700; // bx 	r0
	(object++)[0]=0x46c0; // nop
	                      // lbl2:
	(object++)[0]=0xF7FF; // bl <lbl1>
	(object++)[0]=0xFFFB; // bl (continued)
	// Post gosub and return
	return post_gosub_statement(argnum);
}

int method_or_property(char stringorfloat){
	// '.' has been detected before comming this line
	// stringorfloat is either 0, '$', or '#' for integer, string, or float
	// Pointer to object is in r0 register
	int e,argnum,fid;
	// Get field/method id
	fid=field_id();
	if (fid<0) return fid; // ERROR
	// Check if "." exists
	if ('.'==source[0]) {
		// "." found. The field must be an object
		source++;
		return method_or_property(stringorfloat);
	}
	// Check if "$", "#" etc
	if (stringorfloat) {
		if (stringorfloat!=source[0]) return ERROR_SYNTAX;
		source++;
	}
	skip_blank();
	// Check which method or field
	if ('('==source[0]) { // This is a method
		e=call_object_method(fid);
		if (e) return e;
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		return 0;
	} else { // This is a field
		// R1 will be field id
		e=set_value_in_register(1,fid);
		if (e) return e;
		// Now, R0 is the pointer to object, R1 is field id. Let's call the library
		e=call_lib_code(LIB_OBJ_FIELD);
		if (e) return e;
		// Now, R0 is the pointer to field
		// Let's read from the address
		check_object(1);
		(object++)[0]=0x6800; //      	ldr	r0, [r0, #0]
		// Now R0 is the field value
		return 0;
	}
}

/*
	Preparations of class and object information follow
	Note that these will be called in the class file
*/

int update_cmpdata_class(int data){
	int* olddata;
	int e,num;
	// Get old recod
	olddata=cmpdata_findfirst_with_id(CMPDATA_CLASS,g_class_id);
	if (!olddata) return ERROR_UNKNOWN;
	// Number of data in new record
	num=(olddata[0]>>16)&0xff;
	// Create new recrd
	e=cmpdata_insert(CMPDATA_CLASS,g_class_id,&olddata[1],num);
	if (e) return e;
	// Update last data
	cmpdata_current_record()[num]=data;
	// Delete old record
	cmpdata_delete(olddata);
	return 0;
}

int create_fieldname(int var_number){
	// This function returns Field ID
	// If error occured, return error number as negative value
	int* data;
	int* data2;
	int e;
	// Get the var name
	data=cmpdata_findfirst_with_id(CMPDATA_VARNAME,var_number);
	if (!data) return ERROR_UNKNOWN;
	// If field name already exists, return (only a CMPDATA_FIELDNAME exists for a field name)
	data2=cmpdata_search_string_first(CMPDATA_FIELDNAME,(unsigned char*)&data[2]);
	if (data2) return data2[0]&0xffff;
	// Add CMPDATA_FIELDNAME (copy from CMPDATA_VARNAME)
	e=cmpdata_insert(CMPDATA_FIELDNAME,var_number,(int*)&data[1],((data[0]>>16)&0xff)-1);
	if (e) return e;
	return var_number;
}

int register_class_field(int var_number, int fieldinfo){
	// fieldinfo is either 
	//   CLASS_FIELD | CLASS_PUBLIC
	// or
	//   CLASS_FIELD:
	int id;
	// Update CMPDATA_FIELDNAME
	id=create_fieldname(var_number);
	if (id<0) return id; // Error
	// Update CMPDATA_CLASS
	return update_cmpdata_class((var_number<<24) | fieldinfo | id);
}

int register_class_static_field(int var_number){
	int id,e;
	// Update CMPDATA_FIELDNAME
	id=create_fieldname(var_number);
	if (id<0) return id; // Error
	// Update CMPDATA_CLASS
	e=update_cmpdata_class(CLASS_PUBLIC | CLASS_STATIC | CLASS_FIELD | id);
	if (e) return e;
	// Insert CMPDATA_STATIC
	g_scratch_int[0]=(id<<16) | var_number;
	return cmpdata_insert(CMPDATA_STATIC,g_class_id,(int*)&g_scratch_int[0],1);
}

/*
	New function to return pointer to object
*/

int new_function(void){
	int cn,e,i,fid,num;
	int* data;
	// Get class number
	cn=get_class_number();
	if (cn<0) return cn;
	// R0 will be the class number
	e=set_value_in_register(0,cn);
	if (e) return e;
	// Call library
	e=call_lib_code(LIB_NEW);
	if (e) return e;
	// Prepare arguments for constructor
	skip_blank();
	// Call the constructor (INIT method) if exists
	data=cmpdata_search_string_first(CMPDATA_FIELDNAME,"INIT");
	if (!data) return 0; // No INIT method exists
	fid=data[0]&0xffff; // Field/method id
	// Get class information
	data=cmpdata_findfirst_with_id(CMPDATA_CLASS,cn);
	if (!data) return ERROR_UNKNOWN;
	num=(data[0]>>16)&0xff;
	for(i=1;i<num;i++){
		if ((CLASS_METHOD|CLASS_PUBLIC|fid)==data[i]) break; // INIT method found
	}
	if (num<=i) return 0; // INIT method not found
	// push r0
	check_object(1);
	(object++)[0]=0xb401; // push {r0}
	e=call_object_method(fid);
	if (e) return e;
	// Get the pointer to object back to r0
	check_object(1);
	(object++)[0]=0xbc01; // pop {r0}
	return 0;
}

int let_object(int vn){
	// '.' has been detected before reaching here
	int e;
	// R0 will be pointer to object;
	e=variable_to_r0(vn);
	if (e) return e;
	while(1){
		// Now, R0 is the pointer to object. Let's get pointer to object field
		e=get_pointer_to_field();
		if (e) return e;
		// Now, R0 is the pointer to object field
		check_object(1);
		(object++)[0]=0xb401;       // push	{r0}
		switch(source[0]){
			case '.': // object
				source++;
				object--;
				continue;
			case '$': // string
				source++;
				skip_blank();
				if ('='!=source[0]) return ERROR_SYNTAX;
				e=get_string();
				if (e) return e;
				e=call_lib_code(LIB_STR_TO_OBJECT);
				if (e) return e;
				break;
			case '#': // float
				source++;
				skip_blank();
				if ('='!=source[0]) return ERROR_SYNTAX;
				source++;
				e=get_float();
				if (e) return e;
				break;
			default: // integer
				skip_blank();
				if ('='!=source[0]) return ERROR_SYNTAX;
				source++;
				e=get_integer();
				if (e) return e;
				break;
		}
		check_object(2);
		(object++)[0]=0xbc02; // pop	{r1}
		(object++)[0]=0x6008; // str	r0, [r1, #0]
		return 0;
	}
}

/*
	DELETE statement
*/

int lib_delete(int r0, int r1, int r2){
	cmpdata_delete((int*)r0);
	return r0;
}

int delete_statement(void){
	int e;
	do {
		e=get_integer();
		if (e) return e;
		e=call_lib_code(LIB_DELETE);
		if (e) return e;
	} while(','==(source++)[0]);
	source--;
	return 0;
}

/*
	METHOD statement
*/

int method_statement_main(void){
	int e,num,id;
	int* data;
	// Get the address of method routine
	unsigned short* obefore=object;
	// Pre and post method functions
	check_object(10);
	(object++)[0]=0x1c31;           // adds r1, r6, #0
	call_lib_code(LIB_PRE_METHOD);  // 2 words
	(object++)[0]=0xf000;           // bl <lbl1>
	(object++)[0]=0xf804;           // bl (continued)
	(object++)[0]=0x1c31;           // adds	r1, r6, #0
	call_lib_code(LIB_POST_METHOD); // 2 words
	(object++)[0]=0xbd00;           // pop {pc}
	                                // lbl1:
	(object++)[0]=0xb500;           // push {lr}
	// Update CMPDATAs below
	num=length_of_field();
	if (!num) return ERROR_SYNTAX;
	// Update CMPDATA_FIELDNAME
	data=cmpdata_nsearch_string_first(CMPDATA_FIELDNAME,source,num);
	if (data) {
		// Field id already exists for this field name
		id=data[0]&0xffff;
	} else {
		// Create the field id for this field name
		id=cmpdata_get_id();
		e=cmpdata_insert_string(CMPDATA_FIELDNAME,id,source,num);
		if (e) return e;
	}
	// Update CMPDATA_METHOD
	g_scratch_int[0]=1+(int)obefore;
	e=cmpdata_insert(CMPDATA_METHOD,id,(int*)g_scratch_int,1);
	if (e) return e;
	// Update CMPDATA_CLASS
	return update_cmpdata_class(CLASS_PUBLIC | CLASS_METHOD | id);
}
