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
#define ERROR_UNKNOWN -2
#define ERROR_OBJ_TOO_LARGE -3

/*
	Libraries
*/
#define LIB_PRINT 0
#define LIB_LET_STR 1


/*
	Variables
*/
extern unsigned short kmbasic_object[1024];
extern int kmbasic_data[32];
extern int kmbasic_variables[256];

extern unsigned char* source;
extern unsigned short* object;

/*
	Prototypes
*/

int get_var_number(void);

void printstr(unsigned char *s);

void init_compiler(void);
void run_code(void);
int call_lib_code(int lib_number);
int set_value_in_register(unsigned char r,int val);
int compile_line(unsigned char* code);

int kmbasic_library(int r0, int r1, int r2, int r3);

int let_statement(void);
int print_statement(void);
int return_statement(void);
int end_statement(void);

int get_string(void);
int get_integer(void);
int get_float(void);

int kmbasic_library(int r0, int r1, int r2, int r3);

/*
	Macros
*/
// Skip blank and tab
#define skip_blank() \
	do {\
		while(source[0]==0x20 || source[0]==0x09) source++;\
	} while(0)

// Check object area remaining
#define check_object(size) \
	do {\
		if (&kmbasic_object[(sizeof kmbasic_object)/(sizeof kmbasic_object[0])]<=object+size) return ERROR_OBJ_TOO_LARGE;\
	} while(0)
