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

unsigned short kmbasic_object[1024];
int kmbasic_data[32];
int kmbasic_variables[256];

// Depth of stack used for calculation
int g_sdepth;
int g_maxsdepth;

// Scratch variable
int g_scratch[4];
