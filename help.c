/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "./compiler.h"
#include "./api.h"

char g_help_file[64]="/docume~1/help-e.txt";

int ini_file_help(char* line){
	int i;
	if (!strncmp(line,"HELPFILE=",9)) {
		line+=9;
		for(i=0;line[i] && i<63;i++) g_help_file[i]=line[i];
		g_help_file[i]=0;
	} else {
		return 0;
	}
	return 1;
}

char* get_help(const char* word){
	// Longest words are I2CWRITEDATA/SPIWRITEDATA (12 bytes)
	int i,j;
	char* ret;
	FIL fpo;
	FIL* fp=&fpo;
	// Open the help file
	if (f_open(fp,g_help_file,FA_READ)) return 0;
	// Search the help file
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer, g_file_buffer_size, fp)) continue;
		// Check the string
		for(i=0;i<12;i++){
			if (word[i]!=g_file_buffer[i]) break;
		}
		if (0x00!=word[i]) continue;
		switch(g_file_buffer[i]){
			case 0x20: case 0x0d: case 0x0a:
			case '(': case '$': case '#':
				// The word found. Add the next line
				for(i=0;g_file_buffer[i] && i<g_file_buffer_size-1;i++); // Goto \x0
				f_gets(&g_file_buffer[i], g_file_buffer_size-i, fp);
				// Close the file and return pointer
				f_close(fp);
				return g_file_buffer;
			default:
				// The word not yet found
				break;
		}
	}
	// The word not found
	// Close the file and return null
	f_close(fp);
	return 0;
}
