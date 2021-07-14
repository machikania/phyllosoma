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
		data16   : field id
		data[1]- : field name string
	
	CMPDATA_FIELD      : field information
		data16 : field id
		data[1]: upper half: method/field information, lower half: class id
	
	CMPDATA_CLASS      : class structure and id
		data16 :   class id
		data[1]:   upper half: method/field information, lower half: field id
		data[2]:   upper half: method/field information, lower half: field id
		data[3]:   ....
		data[n-1]: upper half: method/field information, lower half: field id
	
			method/field information (16 bit):
				#define CLASS_METHOD 0x00010000
				#define CLASS_FIELD  0x00020000
				#define CLASS_PUBLIC 0x00100000
				#define CLASS_STATIC 0x01000000
	
	CMPDATA_METHOD     : class method address and id
		data16 : field id
		data[1]: method address to call
	
	CMPDATA_STATICFIELD: static field information
		data16: field id
		data[1]: upper half: class id, lower half: variable number
	
	To get a class structure,
		1. CMPDATA_CLASSNAME to get class id
		2. CMPDATA_CLASS
	
	To get a field information
		1. CMPDATA_FIELDNAME to get field id
		2. CMPDATA_FIELD
	
	To get a method information
		1. CMPDATA_FIELDNAME to get field id
		2. CMPDATA_FIELD to get class id and check if it's a method of the class
		3. CMPDATA_METHOD
		4. CMPDATA_CLASS to obtain class structure
	
	To get a static field/method information
		1. CMPDATA_CLASSNAME to get class id
		2. CMPDATA_CLASS
		3. Pick up field ids of static field from class structure
		4. CMPDATA_FIELDNAME to get field id
		5. Check if id of 4 matches to id of 3
	
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
		if (strncmp(source,(char*)&data[1],num)) continue;
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
		return 0;
	}
	// Check if static field
	if (!(class[i]&CLASS_STATIC)) return ERROR_SYNTAX;
	if (!(class[i]&CLASS_FIELD)) return ERROR_NOT_FIELD;
	// This is a public static field
	data=cmpdata_findfirst_with_id(CMPDATA_STATICFIELD,class[i]&0xffff);
	if (!data) return ERROR_UNKNOWN;
	if (((data[1]>>16)&0xffff) != cn) return ERROR_UNKNOWN;
	return variable_to_r0(data[1]&0xffff);
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
