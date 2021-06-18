/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./api.h"
#include "./compiler.h"

static char* g_error_file;
static int g_error_line;

static const char* g_error_text[]={
	"No error", 
	"Syntax error",
	"Unknown error",
	"Compile object too large",
	"Same variable name used twice",
	"Too many variables",
};

void show_error(int e, int pos){
	e=-e;
	printstr("\n");
	if (g_error_file) printstr(g_error_file);
	printstr(" at ");
	if (g_error_line) printint(g_error_line);
	printstr(": ");
	if (e<((sizeof g_error_text)/4)) {
		printstr((char*)g_error_text[e]);
	} else {
		printstr("Error #");
		printint(e);
	}
	printstr(" in line ");
	printint(g_linenum);
	printstr(" at ");
	printint(pos);
	printstr("\n");
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
