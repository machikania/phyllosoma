/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Configration
*/

#define DEBUG_MODE
#define TEMPVAR_NUMBER 10
#define ALLOC_BLOCK_NUM 256

/*
	Error codes
*/
#ifdef DEBUG_MODE
	int throw_error(int e,int line, char* file);
	#define _throw_error(e) throw_error(e,__LINE__,__FILE__)
#else
	#define _throw_error(e) (e)
#endif
#define ERROR_SYNTAX _throw_error(-1)
#define ERROR_UNKNOWN _throw_error(-2)
#define ERROR_OBJ_TOO_LARGE _throw_error(-3)
#define ERROR_VARNAME_USED _throw_error(-4)
#define ERROR_TOO_MANY_VARS _throw_error(-5)
#define ERROR_RESERVED_WORD _throw_error(-6)
#define ERROR_LABEL_DUPLICATED _throw_error(-7)
#define ERROR_NO_TEMP_VAR _throw_error(-8)
#define ERROR_OUT_OF_MEMORY _throw_error(-9)
#define ERROR_NOT_OBJECT _throw_error(-10)
#define ERROR_DATA_NOT_FOUND  _throw_error(-11)
#define ERROR_OBJ_TOO_MANY  _throw_error(-12)

/*
	Libraries
		0-127:
			Quick libraries.
			Used for calculations, functions, etc.
		128-255:
			Statement libraries.
			Garbage collection flag will raised after executing the library function.
			Break key will be also checked.
*/
#define LIB_CALC 0
#define LIB_CALC_FLOAT 1
#define LIB_HEX 2
#define LIB_ADD_STRING 3
#define LIB_STRNCMP 4
#define LIB_VAL 5
#define LIB_LEN 6
#define LIB_INT 7
#define LIB_RND 8
#define LIB_FLOAT 9
#define LIB_VAL_FLOAT 10
#define LIB_MATH 11
#define LIB_MID 12
#define LIB_CHR 13
#define LIB_DEC 14
#define LIB_FLOAT_STRING 15
#define LIB_SPRINTF 16
#define LIB_READ 17
#define LIB_CREAD 18
#define LIB_READ_STR 19
#define LIB_ASC 20
#define LIB_POST_GOSUB 21
#define LIB_DISPLAY 22

#define LIB_DEBUG 128
#define LIB_PRINT 129
#define LIB_LET_STR 130
#define LIB_END 131
#define LIB_LINE_NUM 132
#define LIB_DIM 133
#define LIB_RESTORE 134
#define LIB_VAR_PUSH 135
#define LIB_VAR_POP 136

/*
	LIB MATH options
*/
#define MATH_ACOS 1
#define MATH_ASIN 2
#define MATH_ATAN 3
#define MATH_ATAN2 4
#define MATH_CEIL 5
#define MATH_COS 6
#define MATH_COSH 7
#define MATH_EXP 8
#define MATH_FABS 9
#define MATH_FLOOR 10
#define MATH_FMOD 11
#define MATH_LOG 12
#define MATH_LOG10 13
#define MATH_MODF 14
#define MATH_POW 15
#define MATH_SIN 16
#define MATH_SINH 17
#define MATH_SQRT 18
#define MATH_TAN 19
#define MATH_TANH 20

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
#define CMPDATA_BREAK_BL 8
#define CMPDATA_IF_BL 9
#define CMPDATA_ENDIF_BL 10
#define CMPDATA_ALL 255
/*
	Misc
*/

#define VAR_MODE_INTEGER 0
#define VAR_MODE_STRING  1
#define VAR_MODE_FLOAT   2

#define ARG_NONE    0
#define ARG_INTEGER 1
#define ARG_FLOAT   2
#define ARG_STRING  3
#define ARG_OPTIONAL         4
#define ARG_INTEGER_OPTIONAL 5
#define ARG_FLOAT_OPTIONAL   6
#define ARG_STRING_OPTIONAL  7
#define ARG1      0
#define ARG2      3
#define ARG3      6
#define ARG4      9
#define ARG5      12
#define ARG6      15
#define LIBOPTION 24

/*
	Variables
*/
extern unsigned short kmbasic_object[512*192];
extern int kmbasic_data[32];
extern int kmbasic_variables[ALLOC_BLOCK_NUM];
extern unsigned short kmbasic_var_size[ALLOC_BLOCK_NUM];

extern unsigned char* source;
extern unsigned short* object;
extern unsigned short* g_objmax;

extern int* g_default_args;

extern int g_linenum;
extern int g_next_varnum;
extern int g_sdepth;
extern int g_maxsdepth;
extern short g_ifdepth;
extern short g_fordepth;

extern volatile char g_scratch[32];
extern volatile int* g_scratch_int;
extern volatile float* g_scratch_float;
extern volatile char* g_scratch_char;

extern const char* const g_reserved_words[116];
extern const int const g_hash_resereved_words[116];

extern char g_constant_value_flag;
extern int g_constant_int;
extern float g_constant_float;

extern char g_garbage_collection;

extern int g_rnd_seed;

extern int g_last_var_num;

extern unsigned short* g_read_point;
extern int g_read_valid_len;
extern unsigned short g_read_mode;

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
void printchar(unsigned char c);
void printint(int i);
void printhex4(unsigned char c);
void printhex8(unsigned char c);
void printhex16(unsigned short s);
void printhex32(unsigned int i);

void init_compiler(void);
void run_code(void);
void rewind_object(unsigned short* objpos);
int check_if_reserved(char* str, int num);
void update_bl(short* bl,short* destination);
int call_lib_code(int lib_number);
int set_value_in_register(unsigned char r,int val);
int compile_line(unsigned char* code);
int instruction_is(unsigned char* instruction);

int lib_end(int r0, int r1, int r2);
int kmbasic_library(int r0, int r1, int r2, int r3);

int gosub_statement(void);
int compile_statement(void);

int string_char(void);
int get_string(void);

int get_positive_decimal_value(void);
int get_simple_integer(void);
int get_integer(void);

int get_simple_float(void);
int get_float(void);

int read_function(void);
int cread_function(void);
int get_dim_pointer(void);
int get_dim_value(void);
int get_int_or_float(void);
int get_string_int_or_float(void);
int get_value(int vmode);

int get_operator(int vmode);
int calculation(int op,int vmode);

int kmbasic_library(int r0, int r1, int r2, int r3);

int argn_function(int lib,int mode);
int args_function(void);
int gosub_function(void);
int debug_function(void);

void cmpdata_init(void);
unsigned short cmpdata_get_id(void);
void cmpdata_reset(void);
int cmpdata_insert(unsigned char type, short data16, int* data, unsigned char num);
int cmpdata_insert_string(unsigned char type, short data16, unsigned char* str, int num);
int* cmpdata_find(unsigned char type);
int* cmpdata_findfirst(unsigned char type);
int* cmpdata_findfirst_with_id(unsigned char type, unsigned short id);
void cmpdata_delete(int* record);
void cmpdata_delete_invalid(void);
int* cmpdata_nsearch_string(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string(unsigned int type,unsigned char* str);
int* cmpdata_nsearch_string_first(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string_first(unsigned int type,unsigned char* str);
int cmpdata_nhash(unsigned char* str, int num);
int cmpdata_hash(unsigned char* str);

void show_error(int e, int pos);
int line_number_from_address(int addr);
void stop_with_error(int e);

void init_memory(void);
void* alloc_memory(int size, int var_num);
void* calloc_memory(int size, int var_num);
void delete_memory(void* data);
int move_from_temp(int vn, int pdata);
void garbage_collection(void* data);
int get_permanent_block_number(void);

int get_class_number(void);
int static_method_or_property(int cn, char stringorfloat);
int method_or_property(char stringorfloat);

// For debugging
void dump_cmpdata(void);
void dump(void);

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
		if (g_objmax<=&object[size]) return ERROR_OBJ_TOO_LARGE;\
	} while(0)

// Operator priority
extern const unsigned char g_priority[];
#define priority(x) (int)g_priority[(int)(x)]
