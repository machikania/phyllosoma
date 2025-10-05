/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./api.h"

char g_help_file[64]="/docume~1/help-e.txt";
//char g_help_file[64]="help-e.txt";

char* get_help(const char* word){
	// Longest words are I2CWRITEDATA/SPIWRITEDATA (12 bytes)
	char uword[13];
	int i,j;
	char* ret;
	FIL fpo;
	FIL* fp=&fpo;
	// Seek the first character
	while('A'<=word[-1] && word[-1]<='Z' || 'a'<=word[-1] && word[-1]<='z' || '0'<=word[-1] && word[-1]<='9') word--;
	// Convert to uppercases
	for(i=0;i<12;i++){
		if ('A'<=word[i] && word[i]<='Z' || '0'<=word[i] && word[i]<='9') {
			uword[i]=word[i];
		} else if ('a'<=word[i] && word[i]<='z') {
			uword[i]=word[i]-0x20;
		} else {
			break;
		}
	}
	uword[i]=0;
	// Open the help file
	if (f_open(fp,g_help_file,FA_READ)) return 0;
	// Search the help file
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer, g_file_buffer_size, fp)) continue;
		// Check the string
		for(i=0;i<12;i++){
			if (uword[i]!=g_file_buffer[i]) break;
		}
		if (0x00!=uword[i]) continue;
		if (0x20!=g_file_buffer[i] && 0x0d!=g_file_buffer[i] && 0x0a!=g_file_buffer[i]) continue;
		// The word found. Add the next line
		for(i=0;g_file_buffer[i] && i<g_file_buffer_size-1;i++); // Goto \x0
		f_gets(&g_file_buffer[i], g_file_buffer_size-i, fp);
		// Close the file and return pointer
		f_close(fp);
		return g_file_buffer;
	}
	// The word not found
	// Close the file and return null
	f_close(fp);
	return 0;
}
