/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Settings
*/

#define TEMPVAR_NUMBER 10

/*
	Error codes
*/
#define ERROR_SYNTAX -1
#define ERROR_UNKNOWN -2
#define ERROR_OBJ_TOO_LARGE -3
#define ERROR_VARNAME_USED -4
#define ERROR_TOO_MANY_VARS -5

/*
	Libraries
*/
#define LIB_DEBUG 0
#define LIB_PRINT 1
#define LIB_LET_STR 2
#define LIB_CALC 3
#define LIB_CALC_FLOAT 4
#define LIB_END 5

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
	CMPDATA
*/
#define CMPDATA_TEMP 0
#define CMPDATA_VARNAME 1
#define CMPDATA_LINENUM 2
#define CMPDATA_LABEL 3
#define CMPDATA_LABELNAME 4
#define CMPDATA_GOTO_NUM_BL 5
#define CMPDATA_GOTO_LABEL_BL 6
#define CMPDATA_CONTINUE 7
#define CMPDATA_BREAK 8
#define CMPDATA_CONTINUE_BL 9
#define CMPDATA_BREAK_BL 10
#define CMPDATA_IF_BL 11
#define CMPDATA_ENDIF_BL 12

/*
	Misc
*/

#define VAR_MODE_INTEGER 0
#define VAR_MODE_STRING  1
#define VAR_MODE_FLOAT   2

/*
	Variables
*/
extern unsigned short kmbasic_object[512*192];
extern int kmbasic_data[32];
extern int kmbasic_variables[256];

extern unsigned char* source;
extern unsigned short* object;
extern unsigned short* g_objmax;

extern int g_sdepth;
extern int g_maxsdepth;

extern volatile int g_scratch[];
extern volatile int* g_scratch_int;
extern volatile float* g_scratch_float;
extern volatile char* g_scratch_char;

/*
	Prototypes
*/

void variable_init(void);
short get_new_varnum(void);
int get_var_number(void);
int var_num_to_r1(int vn);
int r0_to_variable(int vn);
int variable_to_r0(int vn);

void printstr(unsigned char *s);

void init_compiler(void);
void run_code(void);
int call_lib_code(int lib_number);
int set_value_in_register(unsigned char r,int val);
int compile_line(unsigned char* code);
int instruction_is(unsigned char* instruction);

int kmbasic_library(int r0, int r1, int r2, int r3);

int usevar_statement(void);
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

int integer_functions(void);
int float_functions(void);

void cmpdata_init(void);
unsigned short cmpdata_get_id(void);
void cmpdata_reset(void);
int cmpdata_insert(unsigned char type, short data16, int* data, unsigned char num);
int cmpdata_insert_string(unsigned char type, short data16, unsigned char* str, int num);
int* cmpdata_find(unsigned char type);
int* cmpdata_findfirst(unsigned char type);
void cmpdata_delete(int* record);
int* cmpdata_nsearch_string(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string(unsigned int type,unsigned char* str);
int* cmpdata_nsearch_string_first(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string_first(unsigned int type,unsigned char* str);
int cmpdata_nhash(unsigned char* str, int num);
int cmpdata_hash(unsigned char* str);

/*
	Macros
*/
// Skip blank
#define skip_blank() \
	do {\
		while(0x20==source[0]) source++;\
	} while(0)

// Check object area remaining
#define check_object(size) \
	do {\
		if (g_objmax<=object+size) return ERROR_OBJ_TOO_LARGE;\
	} while(0)

// Operator priority
extern const unsigned char g_priority[];
#define priority(x) (int)g_priority[(int)(x)]
