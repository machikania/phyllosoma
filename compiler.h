/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
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
#define LIB_DEBUG 0
#define LIB_PRINT 1
#define LIB_LET_STR 2
#define LIB_CALC 3
#define LIB_CALC_FLOAT 4

/*
	Operators
*/
#define OP_VOID 0
#define OP_OR 1
#define OP_AND 2
#define OP_XOR 3
#define OP_EQ 4
#define OP_NEQ 5
#define OP_LT 6
#define OP_LTE 7
#define OP_MT 8
#define OP_MTE 9
#define OP_SHL 10
#define OP_SHR 11
#define OP_ADD 12
#define OP_SUB 13
#define OP_MUL 14
#define OP_DIV 15
#define OP_REM 16

/*
	Misc
*/

#define VAR_MODE_INTEGER 0
#define VAR_MODE_STRING  1
#define VAR_MODE_FLOAT   2

/*
	Variables
*/
extern unsigned short kmbasic_object[1024];
extern int kmbasic_data[32];
extern int kmbasic_variables[256];

extern unsigned char* source;
extern unsigned short* object;

extern int g_sdepth;
extern int g_maxsdepth;

extern char g_allow_shift_obj;

extern int g_scratch[4];

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
int debug_statement(void);

int get_string(void);
int get_simple_integer(void);
int get_integer(void);
int get_simple_float(void);
int get_float(void);

int get_value(int vmode);

int get_operator(int vmode);
int calculation(int op,int vmode);

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

// Operator priority
extern const unsigned char g_priority[];
#define priority(x) (int)g_priority[(int)(x)]
