/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_class_number(void){
	return ERROR_NOT_OBJECT;
}

int static_method_or_property(int cn, char stringorfloat){
	// "::" has not been detected before comming to this line
	// stringorfloat is either 0, '$', or '#' for integer, string, or float
	if (cn<0) return cn;
	if (':'!=source[0] || ':'!=source[1]) return ERROR_SYNTAX;
	source+=2;
	// TODO: detect static property
	// TODO: detect method
	return ERROR_SYNTAX;
}

int method_or_property(char stringorfloat){
	// '.' has been detected before comming this line
	// stringorfloat is either 0, '$', or '#' for integer, string, or float
	// Pointer to object is in r0 register
	// TODO: detect property
	// TODO: detect method
	return ERROR_SYNTAX;
}
