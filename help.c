/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "./compiler.h"
#include "./api.h"

extern FATFS g_FatFs;

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
	int i,num_lines;
	unsigned int hash;
	FIL fpo;
	FIL* fp=&fpo;
	// Get hash
	hash=cmpdata_nhash(word,0);
	// Special cases
	num_lines=1;
	switch(hash){
		case 0x00047392: //FOR
		case 0x0000154f: //TO
		case 0x01495110: //STEP
		case 0x013c4654: //NEXT
			word="FOR";
			num_lines=2;
			break;
		case 0x0000114f: //DO
		case 0x0134e390: //LOOP
		case 0x543d520c: //UNTIL
			word="DO";
			num_lines=8;
			break;
		case 0x56248345: //WHILE
		case 0x015843c4: //WEND
			word="WHILE";
			num_lines=2;
			break;
		case 0x00001206: //IF
		case 0x0154910e: //THEN
		case 0x0110d485: //ELSE
		case 0x0d484217: //ELSEIF
		case 0x443c5206: //ENDIF
			word="IF";
			num_lines=8;
			break;
		case 0x0004d114: //LET
			word="[LET]";
			break;
		case 0x130576fa: //INTERRUPT
		case 0x47204344: //FIELD
		case 0x95015217: //STATIC
			num_lines=2;
			break;
			num_lines=2;
			break;
		case 0x98495119: //SYSTEM
			num_lines=3;
			break;
		default:
			break;
	}
	// Open the help file
	if (f_open(fp,g_help_file,FA_READ)) {
		// Mount and open again
		if (f_mount(&g_FatFs, "", 0)) return 0;
		if (f_open(fp,g_help_file,FA_READ)) return 0;
	}
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
				while(i<g_file_buffer_size-1 && !f_eof(fp)) {
					f_gets(&g_file_buffer[i], g_file_buffer_size-i, fp);
					if ('\t'!=g_file_buffer[i]) {
						// Next statement description started
						if ((--num_lines)<1) break;
					}
					// Statement description continued
					for(i=i;g_file_buffer[i] && i<g_file_buffer_size-1;i++); // Goto \x0
				}
				g_file_buffer[i]=0x00;
				if (g_file_buffer_size-1<=i) g_file_buffer[i-3]=g_file_buffer[i-2]=g_file_buffer[i-1]='.'; // Exceeds 1024 bytes
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
