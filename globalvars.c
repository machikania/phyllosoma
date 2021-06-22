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

unsigned short kmbasic_object[512*192]; // 192K bytes RAM area
int kmbasic_data[32];
int kmbasic_variables[256];

unsigned short* g_objmax;

// Line number
int g_linenum;

// Depth of stack used for calculation
int g_sdepth;
int g_maxsdepth;

// Depth of if/endif and for/do/while
short g_ifdepth;
short g_fordepth;

// Scratch variable
volatile char g_scratch[32];
volatile int* g_scratch_int=(volatile int*)&g_scratch[0];
volatile float* g_scratch_float=(volatile float*)&g_scratch[0];