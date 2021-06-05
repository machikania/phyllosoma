/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

/*
	Error codes
*/
#define ERROR_SYNTAX -1

/*
	Libraries
*/
#define LIB_PRINT_STR 1


/*
	Variables
*/
extern unsigned short g_object[1024];
extern unsigned char* ccode;
extern unsigned short* object;

/*
	Prototypes
*/

void printstr(unsigned char *s);

void init_compiler(void);
void run_code(void);
int call_lib_code(int lib_number);
int compile_line(unsigned char* code);

int kmbasic_library(int r0, int r1, int r2, int r3);

int print_statement(void);
int return_statement(void);
int end_statement(void);

int get_string(void);

int kmbasic_library(int r0, int r1, int r2, int r3);

/*
	Macros
*/
// Skip blank and tab
#define skip_blank() \
	do {\
		while(ccode[0]==0x20 || ccode[0]==0x09) ccode++;\
	} while(0)

