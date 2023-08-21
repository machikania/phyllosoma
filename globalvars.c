/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	Dedicate followings for easy writing of compiler codes
*/
unsigned char* source;
unsigned short* object;

/*
	KM-BASIC related global areas
*/

unsigned short __attribute__((section(".kmbasicobject"))) kmbasic_object[512*KMBASIC_OBJECT_KBYTES]; // 192K or defined bytes RAM area
int kmbasic_data[32];
int kmbasic_variables[ALLOC_BLOCK_NUM];
unsigned short kmbasic_var_size[ALLOC_BLOCK_NUM];

unsigned short* g_objmax;

// kmbasic_data[] area is also used for temporary region when compiling
int* g_default_args=(int*)&kmbasic_data[-1];      // Use 6 words; g_default_args[1] - g_default_args[6]
void** g_callback_args=(void**)&kmbasic_data[-1]; // Use 6 functions; g_callback_args[1] - g_callback_args[6]
// (26 words remaining)

// kmbasic_variables[] is also used for file buffer and compiling buffer
unsigned char* g_file_buffer=(unsigned char*)&kmbasic_variables[0];
unsigned char* g_compile_buffer=(unsigned char*)&kmbasic_variables[ALLOC_BLOCK_NUM/2];
const int g_file_buffer_size=ALLOC_BLOCK_NUM*2;

// Line number
int g_linenum;
int g_error_linenum;

// Function to call when multipe line statement
void* g_multiple_statement;

// Variable number used when compiling
int g_next_varnum;

// Depth of stack used for calculation
int g_sdepth;
int g_maxsdepth;

// Depth of if/endif and for/do/while
short g_ifdepth;
short g_fordepth;

// Scratch variable
volatile char g_scratch[32];
volatile int* g_scratch_int=(volatile int*)&g_scratch[0];
volatile short* g_scratch_short=(volatile short*)&g_scratch[0];
volatile char* g_scratch_char=(volatile char*)&g_scratch[0];
volatile float* g_scratch_float=(volatile float*)&g_scratch[0];

// Constant values
char g_constant_value_flag;
int g_constant_int;
float g_constant_float;

// Memory allocation
char g_garbage_collection;

// Random seed
int g_rnd_seed;

// Last var number used for memory allocation
int g_last_var_num;

// Reading point used for READ() and CREAD() and reading mode
unsigned short* g_read_point;
int g_read_valid_len;
unsigned short g_read_mode;

// Class related

char* g_class_file;
unsigned short g_class_id;
int g_class_mode;
unsigned short* g_class_id_list;
int* g_class_list;
int* g_empty_object_list;
char g_before_classcode;
char g_after_classcode;

// printf() ON/OFF, LCD out ON/OFF

char g_disable_printf=0;
char g_disable_lcd_out=0;

// debug wait ON/OFF

char g_disable_debugwait2500=0;

// Button rotation ON/OFF

char g_enable_button_rotation=1;

// Reset at the end of program

char g_reset_at_end=0;

// Waiting time at the beginning

int g_wait_at_begin=500;

// Flag of interrupt

char g_interrupt_code=0;

// How long time for waiting keyboard connection

unsigned int g_wait_for_keyboard=2000;

// Reserved words



const char* const g_reserved_words[187]={
	"ABS",
	"ACOS",
	"ALIGN4",
	"ANALOG",
	"ARGS",
	"ASC",
	"ASIN",
	"ATAN",
	"ATAN2",
	"BGCOLOR",
	"BOXFILL",
	"BREAK",
	"CALL",
	"CDATA",
	"CEIL",
	"CHR",
	"CIRCLE",
	"CIRCLEFILL",
	"CLASSCODE",
	"CLEAR",
	"CLS",
	"COLOR",
	"CONTINUE",
	"CORETIMER",
	"COS",
	"COSH",
	"CREAD",
	"CURSOR",
	"DATA",
	"DATAADDRESS",
	"DEBUG",
	"DEC",
	"DELAYMS",
	"DELAYUS",
	"DELETE",
	"DIM",
	"DNS",
	"DO",
	"DRAWCOUNT",
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
	"FREMOVE",
	"FSEEK",
	"FUNCADDRESS",
	"GCLS",
	"GCOLOR",
	"GETDIR",
	"GETTIME",
	"GOSUB",
	"GOTO",
	"GPALETTE",
	"GPRINT",
	"HEX",
	"I2C",
	"I2CERROR",
	"I2CREAD",
	"I2CREADDATA",
	"I2CWRITE",
	"I2CWRITEDATA",
	"IDLE",
	"IF",
	"IFCONFIG",
	"IN",
	"IN16",
	"IN8H",
	"IN8L",
	"INKEY",
	"INPUT",
	"INT",
	"INTERRUPT",
	"KEYS",
	"LABEL",
	"LEN",
	"LET",
	"LINE",
	"LOG",
	"LOG10",
	"LOOP",
	"METHOD",
	"MODF",
	"MUSIC",
	"NEW",
	"NEXT",
	"NOT",
	"NTP",
	"OPTION",
	"OUT",
	"OUT16",
	"OUT8H",
	"OUT8L",
	"PALETTE",
	"PCG",
	"PEEK",
	"PEEK16",
	"PEEK32",
	"PI",
	"PLAYWAVE",
	"POINT",
	"POKE",
	"POKE16",
	"POKE32",
	"POW",
	"PRINT",
	"PRIVATE",
	"PSET",
	"PUBLIC",
	"PUTBMP",
	"PWM",
	"READ",
	"READKEY",
	"REM",
	"RESTORE",
	"RETURN",
	"RND",
	"SCROLL",
	"SERIAL",
	"SERIALIN",
	"SERIALOUT",
	"SETDIR",
	"SETTIME",
	"SGN",
	"SIN",
	"SINH",
	"SOUND",
	"SPI",
	"SPIREAD",
	"SPIREADDATA",
	"SPISWAPDATA",
	"SPIWRITE",
	"SPIWRITEDATA",
	"SPRINTF",
	"SQRT",
	"STATIC",
	"STEP",
	"STOP",
	"STRFTIME",
	"STRNCMP",
	"SYSTEM",
	"TAN",
	"TANH",
	"TCPACCEPT",
	"TCPCLIENT",
	"TCPCLOSE",
	"TCPRECEIVE",
	"TCPSEND",
	"TCPSERVER",
	"TCPSTATUS",
	"THEN",
	"TIMER",
	"TLSCLIENT",
	"TO",
	"TVRAM",
	"UNTIL",
	"USECLASS",
	"USEGRAPHIC",
	"USEPCG",
	"USETIMER",
	"USEVAR",
	"VAL",
	"VAR",
	"WAIT",
	"WAVE",
	"WEND",
	"WHILE",
	"WIDTH",
	"WIFIERR",
};
const int const g_hash_resereved_words[187]={
	0x000400d3, //ABS
	0x01002393, //ACOS
	0x0d2063a4, //ALIGN4
	0x0f00d397, //ANALOG
	0x01013193, //ARGS
	0x00040483, //ASC
	0x0101220e, //ASIN
	0x0101500e, //ATAN
	0x405403b2, //ATAN2
	0x7dc72863, //BGCOLOR
	0x66e378bf, //BOXFILL
	0x434c400b, //BREAK
	0x0108034c, //CALL
	0x42140541, //CDATA
	0x0108420c, //CEIL
	0x00042252, //CHR
	0x884c2355, //CIRCLE
	0xab94cf50, //CIRCLEFILL
	0x92492a81, //CLASSCODE
	0x42344012, //CLEAR
	0x00042353, //CLS
	0x4238d392, //COLOR
	0x2ade0dd9, //CONTINUE
	0xea9ef7dc, //CORETIMER
	0x00042393, //COS
	0x0108e488, //COSH
	0x424c4004, //CREAD
	0x944d2382, //CURSOR
	0x01140541, //DATA
	0x15594496, //DATAADDRESS
	0x45103507, //DEBUG
	0x00045103, //DEC
	0x0d018742, //DELAYMS
	0x0d018142, //DELAYUS
	0x44344554, //DELETE
	0x0004520d, //DIM
	0x000453d3, //DNS
	0x0000114f, //DO
	0x827c002e, //DRAWCOUNT
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
	0x3bcf1e71, //FREMOVE
	0x4748410b, //FSEEK
	0x0fbcbca9, //FUNCADDRESS
	0x01182353, //GCLS
	0x8238d383, //GCOLOR
	0x84545203, //GETDIR
	0xeaab78a4, //GETTIME
	0x46392502, //GOSUB
	0x0118e54f, //GOTO
	0x0d1043aa, //GPALETTE
	0x914c83c5, //GPRINT
	0x00049118, //HEX
	0x00049cc3, //I2C
	0x7bb3eb62, //I2CERROR
	0xc24c4498, //I2CREAD
	0x992a493a, //I2CREADDATA
	0x69b25db5, //I2CWRITE
	0xb47db71c, //I2CWRITEDATA
	0x01205345, //IDLE
	0x00001206, //IF
	0x71c2adb7, //IFCONFIG
	0x0000120e, //IN
	0x0120ec76, //IN16
	0x0120ee48, //IN8H
	0x0120ee4c, //IN8L
	0x483ca119, //INKEY
	0x483d1514, //INPUT
	0x000483d4, //INT
	0x130576fa, //INTERRUPT
	0x01284613, //KEYS
	0x4d00310c, //LABEL
	0x0004d10e, //LEN
	0x0004d114, //LET
	0x013483c5, //LINE
	0x0004d387, //LOG
	0x4d387c70, //LOG10
	0x0134e390, //LOOP
	0x04549397, //METHOD
	0x0130e146, //MODF
	0x4c512203, //MUSIC
	0x0004f117, //NEW
	0x013c4654, //NEXT
	0x0004f394, //NOT
	0x0004f550, //NTP
	0x9154839d, //OPTION
	0x0004e514, //OUT
	0x4e514c76, //OUT16
	0x4e514e48, //OUT8H
	0x4e514e4c, //OUT8L
	0x0d115055, //PALETTE
	0x00051087, //PCG
	0x0144410b, //PEEK
	0x4410bc62, //PEEK16
	0x4410bce6, //PEEK32
	0x00001449, //PI
	0x18594115, //PLAYWAVE
	0x513883d4, //POINT
	0x0144e285, //POKE
	0x4e285c62, //POKE16
	0x4e285ce6, //POKE32
	0x00051397, //POW
	0x514c83d4, //PRINT
	0xc85c0051, //PRIVATE
	0x01452114, //PSET
	0x540cd217, //PUBLIC
	0x54543304, //PUTBMP
	0x0005158d, //PWM
	0x014c4004, //READ
	0xffeb5be8, //READKEY
	0x0005310d, //REM
	0xedab1e34, //RESTORE
	0xc45544da, //RETURN
	0x000533c4, //RND
	0x824ce358, //SCROLL
	0x844c8018, //SERIAL
	0xc80195b5, //SERIALIN
	0xff9af426, //SERIALOUT
	0x84545206, //SETDIR
	0xeaab79e4, //SETTIME
	0x0005218e, //SGN
	0x0005220e, //SIN
	0x014883c8, //SINH
	0x523943c4, //SOUND
	0x00052449, //SPI
	0xb7b3bae0, //SPIREAD
	0x1ea046fb, //SPIREADDATA
	0xf55fbc1b, //SPISWAPDATA
	0x164dc3a8, //SPIWRITE
	0xa901b8bd, //SPIWRITEDATA
	0xacdf0fa2, //SPRINTF
	0x014904d4, //SQRT
	0x95015217, //STATIC
	0x01495110, //STEP
	0x01495390, //STOP
	0xc755c5ae, //STRFTIME
	0xacc3d9f5, //STRNCMP
	0x98495119, //SYSTEM
	0x0005500e, //TAN
	0x015403c8, //TANH
	0x025d42bb, //TCPACCEPT
	0x8d75453b, //TCPCLIENT
	0xbdca4f61, //TCPCLOSE
	0xe8aec0f4, //TCPRECEIVE
	0x91484694, //TCPSEND
	0x841877f9, //TCPSERVER
	0x955453f8, //TCPSTATUS
	0x0154910e, //THEN
	0x5520c112, //TIMER
	0x728a8934, //TLSCLIENT
	0x0000154f, //TO
	0x555d300d, //TVRAM
	0x543d520c, //UNTIL
	0xfdcaaa72, //USECLASS
	0x2aaa8267, //USEGRAPHIC
	0x12111092, //USEPCG
	0xeade6ff3, //USETIMER
	0x12117007, //USEVAR
	0x0005700c, //VAL
	0x00057012, //VAR
	0x01580214, //WAIT
	0x015805c5, //WAVE
	0x015843c4, //WEND
	0x56248345, //WHILE
	0x56205548, //WIDTH
	0xf8dfbe70, //WIFIERR
};
