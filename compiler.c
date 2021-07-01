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
			kmbasic_data[3]: 
			kmbasic_data[4]: 
*/

#include <string.h>
#include "./api.h"
#include "./compiler.h"

void init_compiler(void){
	// Initialize variables
	object=&kmbasic_object[0];
	g_objmax=&kmbasic_object[(sizeof kmbasic_object)/2];
	// Initialize CMPDATA
	cmpdata_init();
	// Initialize variable
	variable_init();
	// Initialize followings every file
	g_ifdepth=0;
	g_fordepth=0;
	g_linenum=0;
}

const int const g_r6_array[]={
	0,                         // Pointer to object
	(const int)&g_r6_array[0], // Pointer to previous argument array (recursive)
	0                          // Number of argument(s)
};

void run_code_main(void){
	// Push r0-r12
	asm("push {lr}");
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("mov r0,r8");
	asm("mov r1,r9");
	asm("mov r2,r10");
	asm("mov r3,r11");
	asm("mov r4,r12");
	asm("push {r0,r1,r2,r3,r4}");
	// Set special registers
	// R5 is pointer to array containing variable values
	asm("ldr r5,=kmbasic_variables");
	// R6 is pointer to argument array
	asm("ldr r6,=g_r6_array");
	// R7 is pointer to array containing various data
	asm("ldr r7,=kmbasic_data");
	// R8 is pointer to library function
	asm("ldr r0,=kmbasic_library");
	asm("mov r8,r0");
	// Store SP
	asm("mov r0,sp");
	asm("str r0,[r7,#0]");
/*
	See the comment in library.c
	
	// Reserve 1016 bytes stack area for some library functions (fprint etc)
	asm("sub sp,#508");
	asm("sub sp,#508");
*/
	// Store return address and call kmbasic_object
	asm("ldr r1,=kmbasic_object+1");
	asm("mov r0,pc");
	asm("add r0,#5");
	asm("str r0,[r7,#4]");
	asm("bx r1");
	// Pop r0-r12
	asm("pop {r0,r1,r2,r3,r4}");
	asm("mov r8,r0");
	asm("mov r9,r1");
	asm("mov r10,r2");
	asm("mov r11,r3");
	asm("mov r12,r4");
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("pop {pc}");
}

void run_code(void){
	// Initializing environment
	init_memory();
	// Initialize READ/DATA
	g_read_point=&kmbasic_object[0];
	g_read_mode=0;
	g_read_valid_len=0;
	// Random seed
	g_rnd_seed=0x92D68CA2; //2463534242
	// Run code
	asm("bl run_code_main");
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
	while(instruction[n]) n++;
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
	unsigned char* result=(unsigned char*)&kmbasic_variables[0];
	char instring=0;
	for(i=0;i<1024;i++){
		c=code[i];
		if (instring) {
			if ('"'==c) instring=0;
		} else {
			if ('a'<=c && c<='z') {
				c=c-0x20;
			} else if ('"'==c) {
				instring=1;
			} else if (0x09==c) {
				c=0x20;
			} else if (0x0d==c || 0x0a==c) {
				result[i]=0;
				break;
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

int handle_line_number(int id){
	int e;
	int* data;
	short* bl;
	// Check if this is the first time
	if (cmpdata_findfirst_with_id(CMPDATA_LINENUM,id)) return ERROR_LABEL_DUPLICATED;
	// Register new CMPDATA_LINENUM record
	g_scratch_int[0]=(int)object;
	e=cmpdata_insert(CMPDATA_LINENUM,id,(int*)g_scratch_int,1);
	if (e) return e;
	// Resolve all CMPDATA_GOTO_NUM_BL(s)
	// TODO: consider skipping following lines when not using line numbers in BASIC program
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
	e=get_positive_decimal_value();
	if (0<=e) {
		g_linenum=e;
		if (' '!=source[0]) return ERROR_SYNTAX;
		source++;
	}
	e=handle_line_number(g_linenum);
	if (e) return e;
	// Compile the statement(s)
	while(1){
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
	if (e) {
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
