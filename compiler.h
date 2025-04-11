/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Configration
*/

//#define MACHIKANIA_DEBUG_MODE
#define TEMPVAR_NUMBER 10
#define ALLOC_BLOCK_NUM 256

#ifdef KMBASIC_COMPILER_H
#error "compiler.h" included more than once
#else
#define KMBASIC_COMPILER_H
#endif

#include "./config.h"

/*
	Error codes
*/
#ifdef MACHIKANIA_DEBUG_MODE
	int throw_error(int e,int line, char* file);
	#define _throw_error(e) throw_error(e,__LINE__,__FILE__)
#else
	#define _throw_error(e) (e)
#endif
#define ERROR_SYNTAX _throw_error(-1)
#define ERROR_UNKNOWN _throw_error(-2)
#define ERROR_OTHERS (-3)
#define ERROR_VARNAME_USED _throw_error(-4)
#define ERROR_TOO_MANY_VARS _throw_error(-5)
#define ERROR_RESERVED_WORD _throw_error(-6)
#define ERROR_LABEL_DUPLICATED _throw_error(-7)
#define ERROR_NO_TEMP_VAR _throw_error(-8)
#define ERROR_OUT_OF_MEMORY _throw_error(-9)
#define ERROR_NOT_OBJECT _throw_error(-10)
#define ERROR_DATA_NOT_FOUND  _throw_error(-11)
#define ERROR_OBJ_TOO_MANY  _throw_error(-12)
#define ERROR_FILE  _throw_error(-13)
#define ERROR_COMPILE_CLASS (-14)
#define ERROR_NOT_FIELD  _throw_error(-15)
#define ERROR_NOT_PUBLIC  _throw_error(-16)
#define ERROR_STATEMENT_NOT_DETECTED (-17)
#define ERROR_NO_CLASS_FILE _throw_error(-18)
#define ERROR_OBJ_TOO_LARGE _throw_error(-19)
#define ERROR_INVALID _throw_error(-20)
#define ERROR_MUSIC (-21)
#define ERROR_PATH_TOO_LONG _throw_error(-22)
#define ERROR_OPTION_CLASSCODE (-23)
#define ERROR_EXCEPTION (-24)
#define ERROR_FUNCTION_CALL (-25)
#define ERROR_BAD_FREQUENCY (-26)

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
#define LIB_DISPLAY_FUNCTION 22
#define LIB_INKEY 23
#define LIB_INPUT 24
#define LIB_TIMER 25
#define LIB_KEYS 26
#define LIB_NEW 27
#define LIB_OBJ_FIELD 28
#define LIB_OBJ_METHOD 29
#define LIB_PRE_METHOD 30
#define LIB_POST_METHOD 31
#define LIB_READKEY 32

#define LIB_DEBUG 128
#define LIB_PRINT 129
#define LIB_LET_STR 130
#define LIB_END 131
#define LIB_LINE_NUM 132
#define LIB_DIM 133
#define LIB_RESTORE 134
#define LIB_VAR_PUSH 135
#define LIB_VAR_POP 136
#define LIB_DISPLAY 137
#define LIB_WAIT 138
#define LIB_SYSTEM 139
#define LIB_STR_TO_OBJECT 140
#define LIB_DELETE 141
#define LIB_FILE 142
#define LIB_FOPEN 143
#define LIB_FPRINT 144
#define LIB_INTERRUPT 145
#define LIB_PWM 146
#define LIB_ANALOG 147
#define LIB_SPI 148
#define LIB_I2C 149
#define LIB_SERIAL 150
#define LIB_GPIO 151
#define LIB_MUSIC 152
#define LIB_DELAYUS 153
#define LIB_DELAYMS 154
#define LIB_RTC 155
#define LIB_WIFI 156
#define LIB_AUXCODE 157

/*
	Gereral option used for intializing static variables
*/

#define RESET_STATIC_VARS 32767

/*
	LIB WIFI options
*/

#define LIB_WIFI_IFCONFIG   1
#define LIB_WIFI_DNS        2
#define LIB_WIFI_NTP        3
#define LIB_WIFI_ERR_INT    4
#define LIB_WIFI_ERR_STR    5
#define LIB_WIFI_TCPCLIENT  6
#define LIB_WIFI_TLSCLIENT  7
#define LIB_WIFI_TCPSERVER  8
#define LIB_WIFI_TCPCLOSE   9
#define LIB_WIFI_TCPSTATUS  10
#define LIB_WIFI_TCPSEND    11
#define LIB_WIFI_TCPRECEIVE 12
#define LIB_WIFI_TCPACCEPT  13

/*
	LIB RTC options
*/

#define LIB_RTC_GETTIME  1
#define LIB_RTC_SETTIME  2
#define LIB_RTC_STRFTIME 3

/*
	LIB MUSIC options
*/

#define LIB_MUSIC_MUSIC 1
#define LIB_MUSIC_SOUND 2
#define LIB_MUSIC_MUSICFUNC 3
#define LIB_MUSIC_PLAYWAVE 4
#define LIB_MUSIC_PLAYWAVEFUNC 5

/*
	LIB IO options
*/

#define LIB_SPI_SPI 1
#define LIB_SPI_SPIREAD 2
#define LIB_SPI_SPIWRITE 3
#define LIB_SPI_SPIREADDATA 4
#define LIB_SPI_SPIWRITEDATA 5
#define LIB_SPI_SPISWAPDATA 6
#define LIB_I2C_I2C 1
#define LIB_I2C_I2CREAD 2
#define LIB_I2C_I2CWRITE 3
#define LIB_I2C_I2CERROR 4
#define LIB_I2C_I2CREADDATA 5
#define LIB_I2C_I2CWRITEDATA 6
#define LIB_SERIAL_SERIAL 1
#define LIB_SERIAL_SERIALIN 2
#define LIB_SERIAL_SERIALOUT 3
#define LIB_GPIO_IN 1
#define LIB_GPIO_IN8H 2
#define LIB_GPIO_IN8L 3
#define LIB_GPIO_IN16 4
#define LIB_GPIO_OUT 5
#define LIB_GPIO_OUT8H 6
#define LIB_GPIO_OUT8L 7
#define LIB_GPIO_OUT16 8

/*
	LIB FILE options
*/
#define FILE_FCLOSE 1
#define FILE_FGET 2
#define FILE_FILE 3
#define FILE_FPUT 4
#define FILE_FPUTC 5
#define FILE_FREMOVE 6
#define FILE_FSEEK 7
#define FILE_SETDIR 8
#define FILE_FEOF 9
#define FILE_FGETC 10
#define FILE_FLEN 11
#define FILE_FSEEKFUNC 12
#define FILE_FINPUT 13
#define FILE_GETDIR 14
#define FILE_FFIND 15
#define FILE_FINFO 16
#define FILE_FINFOSTR 17
#define FILE_FRENAME 18
#define FILE_MKDIR 19

/*
	LIB TIMER options
*/
#define TIMER_CORETIMER 1
#define TIMER_USETIMER 2
#define TIMER_TIMER 3
#define TIMER_CORETIMERFUNC 4
#define TIMER_TIMERFUNC 5
#define TIMER_DRAWCOUNT 6
#define TIMER_DRAWCOUNTFUNC 7

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
#define CMPDATA_TEMP          0x00
#define CMPDATA_VARNAME       0x01
#define CMPDATA_LINENUM       0x02
#define CMPDATA_LABEL         0x03
#define CMPDATA_LABELNAME     0x04
#define CMPDATA_GOTO_NUM_BL   0x05
#define CMPDATA_GOTO_LABEL_BL 0x06
#define CMPDATA_CONTINUE      0x07
#define CMPDATA_BREAK_BL      0x08
#define CMPDATA_IF_BL         0x09
#define CMPDATA_ENDIF_BL      0x0A
#define CMPDATA_CLASSNAME     0x0B
#define CMPDATA_FIELDNAME     0x0C
#define CMPDATA_CLASS         0x0D
#define CMPDATA_METHOD        0x0E
#define CMPDATA_STRSTACK      0x0F
#define CMPDATA_CLASS_ADDRESS 0x10
#define CMPDATA_STATIC        0x11
#define CMPDATA_DATA_LABEL_BL 0x12
#define CMPDATA_ALL           0xFF

/*
	Class
*/
#define CLASS_METHOD   0x00010000
#define CLASS_FIELD    0x00020000
#define CLASS_PUBLIC   0x00100000
#define CLASS_STATIC   0x00200000

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
#define ARG_CALLBACK         4
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

#define INTERRUPT_TIMER     0
#define INTERRUPT_DRAWCOUNT 1
#define INTERRUPT_KEYS      2
#define INTERRUPT_INKEY     3
#define INTERRUPT_MUSIC     4
#define INTERRUPT_WAVE      5
#define INTERRUPT_CORETIMER 6

#define VALIDCLOCK4NTSC (g_clock_hz==157500000 || g_clock_hz==315000000)

/*
	Variables
*/

extern unsigned short kmbasic_object[512*KMBASIC_OBJECT_KBYTES_EXTENDED];
extern int kmbasic_data[32];
extern int kmbasic_variables[ALLOC_BLOCK_NUM];
extern unsigned short kmbasic_var_size[ALLOC_BLOCK_NUM];

extern unsigned char* source;
extern unsigned short* object;
extern unsigned short* g_objmax;

extern int* g_default_args;
extern void** g_callback_args;

extern unsigned char* g_file_buffer;
extern unsigned char* g_compile_buffer;
extern const int g_file_buffer_size;

extern void* g_multiple_statement;
extern int g_linenum;
extern int g_error_linenum;
extern int g_next_varnum;
extern int g_sdepth;
extern int g_maxsdepth;
extern short g_ifdepth;
extern short g_fordepth;

extern volatile char g_scratch[32];
extern volatile int* g_scratch_int;
extern volatile short* g_scratch_short;
extern volatile float* g_scratch_float;
extern volatile char* g_scratch_char;

extern const char* const g_reserved_words[189];
extern const int const g_hash_resereved_words[189];

extern char g_constant_value_flag;
extern int g_constant_int;
extern float g_constant_float;

extern char g_garbage_collection;

extern int g_rnd_seed;

extern int g_last_var_num;

extern unsigned short* g_read_point;
extern int g_read_valid_len;
extern unsigned short g_read_mode;

extern char* g_class_file;
extern unsigned short g_class_id;
extern int g_class_mode;
extern unsigned short* g_class_id_list;
extern int* g_class_list;
extern int* g_empty_object_list;
extern char g_before_classcode;
extern char g_after_classcode;

extern char g_disable_printf;
extern char g_disable_lcd_out;
extern char g_disable_debugwait2500;
extern char g_enable_button_rotation;
extern char g_reset_at_end;
extern int g_wait_at_begin;
extern unsigned int g_wait_for_keyboard;

extern char g_interrupt_code;

extern const char g_active_usb_keyboard;
extern unsigned char show_timestamp;
extern unsigned char filesortby;

extern unsigned int g_clock_hz;
extern unsigned int g_clock_hz_default;
extern char g_cpu_voltage_default;

/*
	Prototypes
*/

// main.c
void software_reset(void);

// variable.c
void variable_init(void);
short get_new_varnum(void);
int get_var_number(void);
int r0_to_variable(int vn);
int variable_to_r0(int vn);

// compiler.c
void init_compiler(void);
int post_compile(void);
void begin_file_compiler(void);
int end_file_compiler(void);
void rewind_object(unsigned short* objpos);
int check_if_reserved(char* str, int num);
void update_bl(short* bl,short* destination);
int call_lib_code(int lib_number);
int set_value_in_register(unsigned char r,int val);
int compile_line(unsigned char* code);
int instruction_is(unsigned char* instruction);
int filename_strcmpi(const char *string1, const char *string2);

// run.c
void pre_run(void);
void run_code(void);
void post_run(void);
int machikania_snprintf(char *buffer, int n, const char *format_string, float float_value);

// library.c
int lib_end(int r0, int r1, int r2);
unsigned short* seek_data(int mode);
int lib_restore(int r0, int r1, int r2);
int lib_read(int r0, int r1, int r2);
int kmbasic_library(int r0, int r1, int r2, int r3);

// statement.c
int get_label_id(void);
int goto_label(void);
int gosub_arguments(void);
int gosub_statement(void);
int post_gosub_statement(int i);
int compile_statement(void);
int end_of_statement(void);
int restore_statement(void);

// withoutkeyboard.c
// withkeyboard.c
void post_inifile(void);
void pre_fileselect(void);
int lib_inkey(int r0, int r1, int r2);
int lib_input(int r0, int r1, int r2);
int lib_readkey(int r0, int r1, int r2);
int check_break(void);
int check_keypress(void);

// editor.c
void texteditor(void);

// string.c
int string_char(void);
int get_string(void);

// integer.c
int system_function(void);
int get_positive_decimal_value(void);
int get_simple_integer(void);
int get_integer(void);

// float.c
int get_simple_float(void);
int get_float(void);

// value.c
int read_function(void);
int cread_function(void);
int get_dim_pointer(void);
int get_dim_value(void);
int get_int_or_float(void);
int get_string_int_or_float(void);
int get_value(int vmode);
int get_simple_value(int vmode);
int end_of_value(void);

// operators.c
int get_operator(int vmode);
int calculation(int op,int vmode);

// function.c
int argn_function(int lib,int mode);
int args_function(void);
int gosub_function(void);
int debug_function(void);

// cmpdata.c
void cmpdata_init(void);
int* cmpdata_current_record(void);
unsigned short cmpdata_get_id(void);
void cmpdata_reset(void);
int cmpdata_insert(unsigned char type, short data16, int* data, unsigned char num);
int cmpdata_insert_string(unsigned char type, short data16, unsigned char* str, int num);
int* cmpdata_find(unsigned char type);
int* cmpdata_findfirst(unsigned char type);
int* cmpdata_findfirst_with_id(unsigned char type, unsigned short id);
void cmpdata_delete(int* record);
void cmpdata_delete_all(unsigned char type);
void cmpdata_delete_invalid(void);
int* cmpdata_nsearch_string(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string(unsigned int type,unsigned char* str);
int* cmpdata_nsearch_string_first(unsigned int type,unsigned char* str,int num);
int* cmpdata_search_string_first(unsigned int type,unsigned char* str);
int cmpdata_nhash(unsigned char* str, int num);
int cmpdata_hash(unsigned char* str);
unsigned char* cmpdata_insert_string_stack(int num);
void cmpdata_delete_string_stack(unsigned char* str);

// error.c
int show_error(int e, int pos);
int line_number_from_address(int addr);
void stop_with_error(int e);

// memory.c
void init_memory(void);
void reset_memory(void);
void* alloc_memory(int size, int var_num);
void* calloc_memory(int size, int var_num);
void delete_memory(void* data);
int move_from_temp(int vn, int pdata);
void garbage_collection(void* data);
int get_permanent_block_number(void);
int get_number_of_remaining_blocks(void);
void var2permanent(int var_num);
void* machikania_malloc(int size);
void machikania_free(void *ptr);
void* machikania_calloc(int nmemb, int size);

// class.c
int init_class_compiling(void);
int post_compilling_a_class(void);
int post_compilling_classes(void);
int length_of_field(void);
int get_class_number(void);
int static_method_or_property(int cn, char stringorfloat);
int static_property_var_num(int cn);
int method_or_property(char stringorfloat);
int register_class_field(int var_number, int fieldinfo);
int register_class_static_field(int var_number);
int new_function(void);
int lib_new(int r0, int r1, int r2);
int let_object(int vn);
int lib_delete(int r0, int r1, int r2);
int delete_statement(void);
int method_statement_main(void);
int lib_resolve_field_address(int r0, int r1, int r2);
int lib_resolve_method_address(int r0, int r1, int r2);
int lib_pre_method(int r0, int r1, int r2);
int lib_post_method(int r0, int r1, int r2);

// file.c
void init_file_system(void);
int mmc_file_exists(unsigned char* fname);
#define file_exists mmc_file_exists
int compile_file(unsigned char* fname, char isclass);
void close_all_files(void);
int lib_file(int r0, int r1, int r2);
int lib_fopen(int r0, int r1, int r2);
int lib_fprint_main(int r0, int r1, int r2);
int fclose_statement(void);
int fget_function(void);
int file_statement(void);
int fopen_function(void);
int fput_function(void);
int fputc_function(void);
int fremove_function(void);
int fseek_statement(void);
int setdir_function(void);
int feof_function(void);
int fgetc_function(void);
int flen_function(void);
int fseek_function(void);
int finput_function(void);
int getdir_function(void);
int ffind_function(void);
int finfo_function(void);
int finfostr_function(void);
int frename_function(void);
int mkdir_function(void);

// display.c
int lib_display(int r0, int r1, int r2);

// timer.c
int interrupt_statement(void);
int drawcount_statement(void);
int drawcount_function(void);
int coretimer_function(void);
int usetimer_statement(void);
int coretimer_statement(void);
int timer_statement(void);
int timer_function(void);
void cancel_all_interrupts(void);
void timer_init(void);
int lib_timer(int r0, int r1, int r2);
int lib_interrupt(int r0, int r1, int r2);
void call_interrupt_function(void* r0);
void raise_interrupt_flag(int i);

// io.c
int ini_file_io(char* line);
void io_init(void);
int lib_keys(int r0, int r1, int r2);
int lib_pwm(int r0, int r1, int r2);
int lib_analog(int r0, int r1, int r2);
int lib_spi(int r0, int r1, int r2);
int lib_i2c(int r0, int r1, int r2);
int lib_serial(int r0, int r1, int r2);
int lib_gpio(int r0, int r1, int r2);
int io_statements(void);
int io_functions(void);

// music.c
void musicint(void);
void init_music(void);
void stop_music(void);
int lib_music(int r0, int r1, int r2);
int music_statement(void);
int music_function(void);
int sound_statement(void);
int playwave_statement(void);
int playwave_function(void);

// pcconnect.c
void connect2pc(void);

// hexfile.c
void runHex(char* filename);

// exception.c
int ini_file_exception(char* line);
void handle_exception(int set);

// wifi
void board_led(int led_on);
int ini_file_wifi(char* line);
int connect_wifi(char show_progress);
int wifi_statements(void);
int wifi_int_functions(void);
int wifi_str_functions(void);
int lib_wifi(int r0, int r1, int r2);
void pre_run_wifi(void);
void post_run_wifi(void);

// rtc.c
void init_machikania_rtc(void);
int ini_file_rtc(char* line);
int lib_rtc(int r0, int r1, int r2);
int gettime_function(void);
int settime_statement(void);
int strftime_function(void);

// auxcodes.c
int aux_statements(void);
int aux_int_functions(void);
int aux_str_functions(void);
int aux_float_functions(void);
int lib_aux(int r0, int r1, int r2);

// For debugging
void dump_cmpdata(void);
void dump(void);
void blink_led(int num);

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
#define priority(x) ((int)g_priority[(int)(x)])
