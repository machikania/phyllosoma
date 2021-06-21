/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "./api.h"
#include "./compiler.h"

const char* const g_reserved_words[114]={
	"ABS",
	"ACOS",
	"ARGS",
	"ASC",
	"ASIN",
	"ATAN",
	"ATAN2",
	"BREAK",
	"CALL",
	"CDATA",
	"CEIL",
	"CHR",
	"CIRCLE",
	"CLEAR",
	"CLS",
	"COLOR",
	"COS",
	"COSH",
	"CREAD",
	"CURSOR",
	"DATA",
	"DEC",
	"DELETE",
	"DIM",
	"DO",
	"ELSE",
	"ELSEIF",
	"END",
	"ENDIF",
	"EXEC",
	"EXP",
	"FABS",
	"FCLOSE",
	"FEOF",
	"FGET",
	"FGETC",
	"FIELD",
	"FILE",
	"FINPUT",
	"FLEN",
	"FLOAT",
	"FLOOR",
	"FMOD",
	"FOPEN",
	"FOR",
	"FPRINT",
	"FPUT",
	"FPUTC",
	"FSEEK",
	"GCLS",
	"GCOLOR",
	"GETDIR",
	"GOSUB",
	"GOTO",
	"GPRINT",
	"HEX",
	"IDLE",
	"IF",
	"INKEY",
	"INPUT",
	"INT",
	"KEYS",
	"LABEL",
	"LEN",
	"LET",
	"LINE",
	"LOG",
	"LOG10",
	"LOOP",
	"MODF",
	"MUSIC",
	"NEXT",
	"NEW",
	"NOT",
	"OPTION",
	"PCG",
	"PEEK",
	"PEEK16",
	"PEEK32",
	"PI",
	"POINT",
	"POKE",
	"POKE16",
	"POKE32",
	"POW",
	"PRINT",
	"PSET",
	"PUBLIC",
	"PUTBMP",
	"READ",
	"REM",
	"RETURN",
	"RND",
	"SCROLL",
	"SETDIR",
	"SGN",
	"SIN",
	"SINH",
	"SOUND",
	"SQRT",
	"SYSTEM",
	"TAN",
	"TANH",
	"TIMER",
	"TVRAM",
	"UNTIL",
	"USEPCG",
	"USEVAR",
	"VAL",
	"VAR",
	"WAIT",
	"WEND",
	"WHILE",
	"WIDTH",
};
const int const g_hash_resereved_words[114]={
	0x000400d3, //ABS
	0x01002393, //ACOS
	0x01013193, //ARGS
	0x00040483, //ASC
	0x0101220e, //ASIN
	0x0101500e, //ATAN
	0x405403b2, //ATAN2
	0x434c400b, //BREAK
	0x0108034c, //CALL
	0x42140541, //CDATA
	0x0108420c, //CEIL
	0x00042252, //CHR
	0x884c2355, //CIRCLE
	0x42344012, //CLEAR
	0x00042353, //CLS
	0x4238d392, //COLOR
	0x00042393, //COS
	0x0108e488, //COSH
	0x424c4004, //CREAD
	0x944d2382, //CURSOR
	0x01140541, //DATA
	0x00045103, //DEC
	0x44344554, //DELETE
	0x0004520d, //DIM
	0x0000114f, //DO
	0x0110d485, //ELSE
	0x0d484217, //ELSEIF
	0x000443c4, //END
	0x443c5206, //ENDIF
	0x01119103, //EXEC
	0x00044650, //EXP
	0x011c00d3, //FABS
	0xc234e494, //FCLOSE
	0x011c4386, //FEOF
	0x011c6114, //FGET
	0x47184543, //FGETC
	0x47204344, //FIELD
	0x011c8345, //FILE
	0xc83d1505, //FINPUT
	0x011cd10e, //FLEN
	0x4734e014, //FLOAT
	0x4734e392, //FLOOR
	0x011cc384, //FMOD
	0x4739110e, //FOPEN
	0x00047392, //FOR
	0xd14c83c5, //FPRINT
	0x011d1514, //FPUT
	0x47454543, //FPUTC
	0x4748410b, //FSEEK
	0x01182353, //GCLS
	0x8238d383, //GCOLOR
	0x84545203, //GETDIR
	0x46392502, //GOSUB
	0x0118e54f, //GOTO
	0x914c83c5, //GPRINT
	0x00049118, //HEX
	0x01205345, //IDLE
	0x00001206, //IF
	0x483ca119, //INKEY
	0x483d1514, //INPUT
	0x000483d4, //INT
	0x01284613, //KEYS
	0x4d00310c, //LABEL
	0x0004d10e, //LEN
	0x0004d114, //LET
	0x013483c5, //LINE
	0x0004d387, //LOG
	0x4d387c70, //LOG10
	0x0134e390, //LOOP
	0x0130e146, //MODF
	0x4c512203, //MUSIC
	0x013c4654, //NEXT
	0x0004f117, //NEW
	0x0004f394, //NOT
	0x9154839d, //OPTION
	0x00051087, //PCG
	0x0144410b, //PEEK
	0x4410bc62, //PEEK16
	0x4410bce6, //PEEK32
	0x00001449, //PI
	0x513883d4, //POINT
	0x0144e285, //POKE
	0x4e285c62, //POKE16
	0x4e285ce6, //POKE32
	0x00051397, //POW
	0x514c83d4, //PRINT
	0x01452114, //PSET
	0x540cd217, //PUBLIC
	0x54543304, //PUTBMP
	0x014c4004, //READ
	0x0005310d, //REM
	0xc45544da, //RETURN
	0x000533c4, //RND
	0x824ce358, //SCROLL
	0x84545206, //SETDIR
	0x0005218e, //SGN
	0x0005220e, //SIN
	0x014883c8, //SINH
	0x523943c4, //SOUND
	0x014904d4, //SQRT
	0x98495119, //SYSTEM
	0x0005500e, //TAN
	0x015403c8, //TANH
	0x5520c112, //TIMER
	0x555d300d, //TVRAM
	0x543d520c, //UNTIL
	0x12111092, //USEPCG
	0x12117007, //USEVAR
	0x0005700c, //VAL
	0x00057012, //VAR
	0x01580214, //WAIT
	0x015843c4, //WEND
	0x56248345, //WHILE
	0x56205548, //WIDTH
};

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

void run_code(void){
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
	// R7 is pointer to array containing various data
	asm("ldr r7,=kmbasic_data");
	// R8 is pointer to library function
	asm("ldr r0,=kmbasic_library");
	asm("mov r8,r0");
	// Store SP
	asm("mov r0,sp");
	asm("str r0,[r7,#0]");
	// Reserve 1016 bytes stack area for some library functions (fprint etc)
	asm("sub sp,#508");
	asm("sub sp,#508");
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

int set_value_in_register(unsigned char r,int val){
	// TODO: revise here for r1,r2,r3 etc (not r0)
	if (7<r) return ERROR_UNKNOWN;
	if (0<=val && val<=255) {
		val+=(r<<8);
		check_object(1);
		(object++)[0]=0x2000 | val | (r<<8);      // movs	r1, #xx
		return 0;
	} else if ((int)object&0x03) {
		// Lower 2 bit of object is 0b10
		check_object(5);
		(object++)[0]=0x4801;     // ldr    r0, [pc, #4]
		(object++)[0]=0xe002;     // b.n    <skip>
		(object++)[0]=0x46c0;     // nop            ; (mov r8, r8)
		(object++)[0]=val&0xffff; // lower 16 bits
		(object++)[0]=val>>16;    // upper 16 bits
		                          // <skip>:	
		return 0;
	} else {
		// Lower 2 bit of object is 0b00
		check_object(4);
		(object++)[0]=0x4800;     // ldr    r0, [pc, #0]
		(object++)[0]=0xe001;     // b.n    <skip>
		(object++)[0]=val&0xffff; // lower 16 bits
		(object++)[0]=val>>16;    // upper 16 bits
		                          // <skip>:	
		return 0;
	}
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

// This function returns number of bytes being compiled.
// If an error occured. this returns negative value as error code.
int compile_line(unsigned char* code){
	int e;
	unsigned char* before;
	// Initialize
	g_linenum++;
	before=source=code2upper(code);
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
