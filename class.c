/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "./compiler.h"

/*
	CMPDATA_CLASSNAME  : class name and id
		data16   : class id
		data[1]- : class name string
	
	CMPDATA_FIELDNAME  : class field and method names and id
		data16   : variable number/method id
		data[1]- : field name string
	
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
	
	CMPDATA_METHOD     : class method address and id
		data16 : method id
		data[1]: method address to call
	
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
		2. CMPDATA_CLASS
		3. Pick up variable numbers of static field from class structure
		4. CMPDATA_FIELDNAME to get variable number
		5. Check if id of 4 matches to id of 3
	
*/

/*
	Structure of object
		data[0]:   upper half: length of object (n), lower half: class id
		data[1]:   private/public field value
		data[2]:   private/public field value
		...
		data[n-1]: private/public field value
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

int class_method(int method_address, int static_flag){
	// R0 is the address of method
	// static_flag is set when static method will be called
	return ERROR_SYNTAX;
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
		source++;
		data=cmpdata_findfirst_with_id(CMPDATA_METHOD,class[i]&0xffff);
		if (!data) return ERROR_UNKNOWN;
		// Compile method
		i=class_method(data[1],0);
		if (i) return i;
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		g_class_mode=CLASS_PUBLIC | CLASS_METHOD;
		return 0;
	}
	// Check if static field
	if (!(class[i]&CLASS_STATIC)) return ERROR_SYNTAX;
	if (!(class[i]&CLASS_FIELD)) return ERROR_NOT_FIELD;
	// This is a public static field
	g_class_mode=CLASS_PUBLIC | CLASS_STATIC | CLASS_FIELD;
	g_scratch_int[0]=class[1]&0xffff; // Store variable name to scratch int
	return variable_to_r0(class[1]&0xffff);
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

int method_or_property(char stringorfloat){
	// '.' has been detected before comming this line
	// stringorfloat is either 0, '$', or '#' for integer, string, or float
	// Pointer to object is in r0 register
	int num,len,i,fid;
	int* data;
	int* class;
	// Get field name
	num=length_of_field();
	if (!num) return ERROR_NOT_OBJECT;
	// Get field/method name
	data=cmpdata_nsearch_string_first(CMPDATA_FIELDNAME,source,num);
	if (!data) return ERROR_NOT_FIELD;
	fid=data[0]&0xffff;
	// Check if multiple field/method names
	if (cmpdata_nsearch_string(CMPDATA_FIELDNAME,source,num)) {
		// There are the same filed/method name more than once
		return ERROR_SYNTAX;
	}
	// There is only one field/method with this name
	source+=num;
	if (stringorfloat) {
		if (stringorfloat!=source[0]) return ERROR_SYNTAX;
		source++;
	}
	// Find the class
	cmpdata_reset();
	while(class=cmpdata_find(CMPDATA_CLASS)){
		len=(class[0]>>16)&0xff;
		// Check out class fields/methods
		for(i=1;i<len;i++){
			if ((class[i]&0xffff)==fid) break;
		}
		if (i<len) break;
	}
	if (!class) return ERROR_NOT_FIELD;
	// Check if public
	if (!(class[i]&CLASS_PUBLIC)) return ERROR_NOT_PUBLIC;
	// Check if method
	if (class[i]&CLASS_METHOD) {
		// This is public class method.
		if ('('!=source[0]) return ERROR_SYNTAX;
		source++;
		data=cmpdata_findfirst_with_id(CMPDATA_METHOD,class[i]&0xffff);
		if (!data) return ERROR_UNKNOWN;
		i=class_method(data[1],0);
		if (i) return i;
		if (')'!=source[0]) return ERROR_SYNTAX;
		source++;
		return 0;
	}
	// Check if not static field
	if (class[i]&CLASS_STATIC) return ERROR_SYNTAX;
	if (!(class[i]&CLASS_FIELD)) return ERROR_NOT_FIELD;
	// This is a public field. Count the position of field in object
	num=0;
	for(i=1;i<len;i++){
		if (class[i]&CLASS_STATIC) continue;
		if (class[i]&CLASS_FIELD) num++;
	}
	if (num<32) {
		check_object(1);
		(object++)[0]=0x6800 | (num<<6); // ldr	r0, [r0, #xx]
	} else {
		i=set_value_in_register(1,num<<2);
		if (i) return i;
		check_object(1);
		(object++)[0]=0x5840;            // ldr	r0, [r0, r1]
	}
	return 0;
}

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
	int* data;
	unsigned short fid;
	int e;
	// Get the var name
	data=cmpdata_findfirst_with_id(CMPDATA_VARNAME,var_number);
	if (!data) return ERROR_UNKNOWN;
	// Add CMPDATA_FIELDNAME (copy from CMPDATA_VARNAME)
	e=cmpdata_insert(CMPDATA_FIELDNAME,var_number,(int*)&data[1],((data[0]>>16)&0xff)-1);
	if (e) return e;
	return 0;
}

int register_class_field(int var_number, int fieldinfo){
	// fieldinfo is either 
	//   CLASS_FIELD | CLASS_PUBLIC
	// or
	//   CLASS_FIELD:
	int e;
	// Update CMPDATA_FIELDNAME
	e=create_fieldname(var_number);
	if (e) return e;
	// Update CMPDATA_CLASS
	return update_cmpdata_class(fieldinfo | var_number);
}

int register_class_static_field(int var_number){
	int e;
	// Update CMPDATA_FIELDNAME
	e=create_fieldname(var_number);
	if (e) return e;
	// Update CMPDATA_CLASS
	return update_cmpdata_class(CLASS_PUBLIC | CLASS_STATIC | CLASS_FIELD | var_number);
}

int new_function(void){
	int cn,e;
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
	if (','==source[0]) {
		source++;
		// TODO: prepare arguments
		return ERROR_SYNTAX;
	}
	// TODO call constructor here
	return 0;
}

int lib_new(int r0, int r1, int r2){
	int num,i;
	int* data;
	// Get class structure
	data=cmpdata_findfirst_with_id(CMPDATA_CLASS,r0);
	if (!data) stop_with_error(ERROR_NOT_OBJECT);
	// Count the number of fields
	num=1;
	for(i=1;i<((data[0]>>16)&0xff);i++){
		if (data[i]&CLASS_FIELD) num++;
	}
	// Create the object
	i=get_permanent_block_number();
	if (i<0) stop_with_error(ERROR_OBJ_TOO_MANY);
	data=calloc_memory(num,i);
	data[0]=num<<16|r0;
	return (int)data;
}