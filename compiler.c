/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Usage of ARM registers
	
	R0-R3
		R0: integer/float calculation, argument for function call
		R1: integer/float calculation, argument for function call
		R2: argument for function call
		R3: argument for function call (library number)
	R4-R7
		R4: Not used yet
		R5: &kmbasic_variables[0], pointer to array containing variables values
		R6: Pointer to array containing arguments etc ()
			R6[0]: pointer to class object
			R6[1]: pointer to previous R6
			R6[2]: number of arguments
			R6[3]: argument 1
			R6[4]: argument 2
			...
		R7: &kmbasic_data[0], pointer to array containing KMBASIC data
			kmbasic_data[0]: stack pointer value for ending program
			kmbasic_data[1]: address to return for ending program
			kmbasic_data[2]: &kmbasic_var_size[0]
			kmbasic_data[3]: return address from executing library function
			kmbasic_data[4]: used for error-handling
*/

#include <string.h>
#include "./compiler.h"
#include "./api.h"

void init_compiler(void){
	// Initialize variables
	object=&kmbasic_object[0];
	g_objmax=&kmbasic_object[(sizeof kmbasic_object)/2];
	// Initialize CMPDATA
	cmpdata_init();
	// Initialize variable
	variable_init();
	// Start compiling non-class file
	g_class_id=0;
}

int post_compile(void){
	int e;
	e=post_compilling_classes();
	if (e) return e;
	return 0;
}

void begin_file_compiler(void){
	// Initialize followings every file
	g_ifdepth=0;
	g_fordepth=0;
	g_linenum=0;
	g_error_linenum=0;
	g_multiple_statement=0;
	g_before_classcode=0;
	g_after_classcode=0;
}

int end_file_compiler(void){
	int* data;
	int i;
	do {
		// Check errors
		if (data=cmpdata_findfirst(CMPDATA_GOTO_NUM_BL)) {
			printstr("Line number not found");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_GOTO_LABEL_BL)) {
			printstr("Label not found");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_DATA_LABEL_BL)) {
			printstr("Label not found");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_BREAK_BL)) {
			printstr("FOR/WHILE/DO loop not terminated");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_CONTINUE)) {
			printstr("FOR/WHILE/DO loop not terminated");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_IF_BL)) {
			printstr("IF not terminated");
			break;
		}
		if (data=cmpdata_findfirst(CMPDATA_ENDIF_BL)) {
			printstr("IF not terminated");
			break;
		}
		// No error found. Remove some CMPDATAs
		cmpdata_delete_all(CMPDATA_VARNAME);
		cmpdata_delete_all(CMPDATA_LABEL);
		// All done
		return 0;
	} while(0);
	// Error occured
	i=line_number_from_address(data[1]);
	if (0<i) {
		printstr(" at line ");
		printint(i);
	}
	printchar('\n');
	return ERROR_OTHERS;
}

void rewind_object(unsigned short* objpos){
	object=objpos;
	cmpdata_delete_invalid();
}

int check_if_reserved(char* str, int num){
	int i;
	int hash=cmpdata_nhash(str,num);
	for(i=0;i<sizeof g_hash_resereved_words/sizeof g_hash_resereved_words[0];i++){
		// Compare hash values
		if (hash!=g_hash_resereved_words[i]) continue;
		// Compare length
		if (0!=g_reserved_words[i][num]) continue;
		// Compare strings
		if (strncmp(str,g_reserved_words[i],num)) continue;
		// String matches to a reserved word
		return ERROR_RESERVED_WORD;
	}
	return 0;
}

void update_bl(short* bl,short* destination){
	int i=(int)destination - ((int)bl + 4);
	i>>=1;
	bl[1]=0xf800 | (i&0x7ff);
	i>>=11;
	bl[0]=0xf000 | (i&0x7ff);
}

int call_lib_code(int lib_number){
	if (lib_number<0 || 255<lib_number) return ERROR_UNKNOWN;
	check_object(2);
	(object++)[0]=0x2300 | lib_number;// movs	r3, #1
	(object++)[0]=0x47c0;             // blx	r8
	return 0;
}

int instruction_is(unsigned char* instruction){
	int n;
	// Skip blank first
	skip_blank();
	// Count number
	for(n=0;instruction[n];n++);
	// Compare strings
	if (strncmp(source,instruction,n)) return 0;
	// If this is function, everything is alright.
	if (instruction[n-1]=='(') {
		source+=n;
		skip_blank();
		return 1;
	}
	// Next code must not be character for statement
	switch(source[n]){
		case 0x20:
		case 0x00:
		case ':':
		case ',':
			source+=n;
			skip_blank();
			return 1;
		default:
			// Instructin didn't match
			return 0;
	}
}

/*
	Change all lower case charaters to upper ones
	Change tabs to spaces
	Change all enter characters to null
	Use kmbasic_variables[256] area for the buffer as this area isn't used when compilling
*/
unsigned char* code2upper(unsigned char* code){
	int i;
	unsigned char c;
	unsigned char* result=g_compile_buffer;
	char instring=0;
	for(i=0;i<1024;i++){
		c=code[i];
		if (0x0d==c || 0x0a==c || 0x00==c) {
				result[i]=0;
				break;
		} else if (instring) {
			if ('"'==c) instring=0;
		} else {
			if ('a'<=c && c<='z') {
				c=c-0x20;
			} else if ('"'==c) {
				instring=1;
			} else if (0x09==c) {
				c=0x20;
			} else if (1023==i) {
				c=0x00;
			}
		}
		result[i]=c;
	}
	return result;
}

/*
	New line number handling. See also label_statement
*/

int register_line_number(int id){
	int* data;
	short* bl;
	// Note that line number can be duplicated as multiple files are compiled when using classes
	// Register new CMPDATA_LINENUM record
	g_scratch_int[0]=(int)object;
	return cmpdata_insert(CMPDATA_LINENUM,id,(int*)g_scratch_int,1);
}

int handle_line_number(int id){
	int e;
	int* data;
	short* bl;
	if (e) return e;
	// Resolve all CMPDATA_GOTO_NUM_BL(s)
	while(data=cmpdata_findfirst_with_id(CMPDATA_GOTO_NUM_BL,id)){
		// Found a CMPDATA_GOTO_NUM_BL
		bl=(short*)data[1];
		// Update it
		update_bl(bl,object);
		// Delete the cmpdata record
		cmpdata_delete(data);
	}
	// All done
	return 0;
}

/* 
	This function returns number of bytes being compiled.
	If an error occured. this returns negative value as error code.
*/
int compile_line(unsigned char* code){
	int e;
	unsigned char* before;
	// Initialize
	g_linenum++;
	before=source=code2upper(code);
	// Get line number if exists
	if (g_multiple_statement) e=ERROR_OTHERS;
	else e=get_positive_decimal_value();
	if (0<=e) {
		// Line number exists
		g_linenum=e;
		if (' '!=source[0]) return ERROR_SYNTAX;
		source++;
		e=register_line_number(g_linenum);
		if (e) return e;
		e=handle_line_number(g_linenum);
	} else {
		// Line number does not exist
		e=register_line_number(g_linenum);
	}
	if (e<0) {
		// Error happened
		show_error(e,source-before);
		return e;
	}
	// Compile the statement(s)
	skip_blank();
	if (g_before_classcode) {
		// Compiling a class file as the same file as BASIC main file
		// The codes before "OPTION CLASSCODE" will be ignored
		if (strncmp(source,"OPTION ",7)) return 0;
	}
	if (0x00!=source[0]) while(1){
		e=compile_statement();
		if (e) break; // An error occured
		// Skip blank
		skip_blank();
		// Check null as the end of line
		if (source[0]==0x00) break;
		// Check ':'
		if (source[0]!=':') {
			e=ERROR_SYNTAX;
			break;
		}
		// Continue this  line
		source++;
	}
	switch(e){
		case 0:
			// No error
			break;
		case ERROR_COMPILE_CLASS:
		case ERROR_OPTION_CLASSCODE:
			return e;
		default:
			// Error happened
			show_error(e,source-before);
			return e;
	}
	// Error didn't happen
	e=source-before;
	// End of string is null
	if (0x00==code[e]) return e;
	// End of string is lf
	if (0x0a==code[e]) return e+1;
	// End of string is cr or crlf
	return 0x0a==code[e+1] ? e+2:e+1;
}

/*
	MISC
*/

int filename_strcmpi(const char *string1, const char *string2){
	char lcstr1[13];
	char lcstr2[13];
	int i;
	char c;
	for(i=0;i<12 && (c=string1[i]);i++) {
		if (0x61<=c && c<=0x7a) lcstr1[i]=c-0x20;
		else lcstr1[i]=c;
	}
	lcstr1[i]=0;
	for(i=0;i<12 && (c=string2[i]);i++) {
		if (0x61<=c && c<=0x7a) lcstr2[i]=c-0x20;
		else lcstr2[i]=c;
	}
	lcstr2[i]=0;
	return strcmp(lcstr1,lcstr2);
}
