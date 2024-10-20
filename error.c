/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./api.h"

static char* g_error_file;
static int g_error_line;

static const char* g_error_text[26]={
	"No error",
	"Syntax error",                  // #define ERROR_SYNTAX _throw_error(-1)
	"Unknown error",                 // #define ERROR_UNKNOWN _throw_error(-2)
	"",                              // #define ERROR_OTHERS (-3)
	"Same variable name used twice", // #define ERROR_VARNAME_USED _throw_error(-4)
	"Too many variables",            // #define ERROR_TOO_MANY_VARS _throw_error(-5)
	"Reserved word",                 // #define ERROR_RESERVED_WORD _throw_error(-6)
	"Duplicated label",              // #define ERROR_LABEL_DUPLICATED _throw_error(-7)
	"String too complicated",        // #define ERROR_NO_TEMP_VAR _throw_error(-8)
	"Out of memory",                 // #define ERROR_OUT_OF_MEMORY _throw_error(-9)
	"Not class or object",           // #define ERROR_NOT_OBJECT _throw_error(-10)
	"DATA not found",                // #define ERROR_DATA_NOT_FOUND  _throw_error(-11)
	"Too many objects",              // #define ERROR_OBJ_TOO_MANY  _throw_error(-12)
	"File error",                    // #define ERROR_FILE  _throw_error(-13)
	"",                              // #define ERROR_COMPILE_CLASS (-14)
	"Not class field/method",        // #define ERROR_NOT_FIELD  _throw_error(-15)
	"Not public field/method",       // #define ERROR_NOT_PUBLIC  _throw_error(-16)
	"",                              // #define ERROR_STATEMENT_NOT_DETECTED (-17)
	"Class file not found",          // #define ERROR_NO_CLASS_FILE _throw_error(-18)
	"Compile object too large",      // #define ERROR_OBJ_TOO_LARGE _throw_error(-19)
	"Invalid value or setting",      // #define ERROR_INVALID _throw_error(-20)
	"Music/sound error",             // #define ERROR_MUSIC (-21)
	"File path too long",            // #define ERROR_PATH_TOO_LONG _throw_error(-22)
	"",                              // #define ERROR_OPTION_CLASSCODE (-23)
	"Exception",                     // #define ERROR_EXCEPTION (-24)
	"Function call error",           // #define ERROR_FUNCTION_CALL (-25)
};

int show_error(int e, int pos){
	e=-e;
	printstr("\n");
#ifdef MACHIKANIA_DEBUG_MODE
	if (g_error_file) printstr(g_error_file);
	printstr(" at ");
	if (g_error_line) printint(g_error_line);
	printstr(": ");
#endif
	if (e<((sizeof g_error_text)/4)) {
		printstr((char*)g_error_text[e]);
	} else {
		printstr("Error #");
		printint(e);
	}
	if (g_linenum) {
		printstr(" in line ");
		printint(g_linenum);
		printstr(" at ");
		printint(pos);
	}
	printstr("\n");
	return e;
}

int throw_error(int e,int line, char* file){
	int i;
	g_error_file=file;
	for(i=0;file[i];i++){
		if ('a'<=file[i]&& file[i]<='z') continue;
		if ('.'==file[i]) continue;
		if ('_'==file[i]) continue;
		g_error_file=file+i+1;
	}
	g_error_line=line;
	return e;
}

int line_number_from_address(int addr){
	int* data;
	cmpdata_reset();
	while(data=cmpdata_find(CMPDATA_LINENUM)){
		if (addr<data[1]) continue;
		// Found
		return data[0]&0xffff;
	}
	// Not found
	return -1;
}

void stop_with_error(int e){
	int* data;
	e=-e;
	printstr("\n");
	if (e<((sizeof g_error_text)/4)) {
		printstr((char*)g_error_text[e]);
	} else {
		printstr("Error #");
		printint(e);
	}
	// Show line number
	e=line_number_from_address(kmbasic_data[3]);
	if (e<0) {
		// Line number not found
		printstr(" at ");
		printhex32(kmbasic_data[3]);
		printstr("\n");
	} else {
		printstr(" in line ");
		printint(e);
		printstr("\n");
	}
	// End BASIC program
	lib_end(0,0,0);
}
