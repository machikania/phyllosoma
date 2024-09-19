/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./interface/ff.h"

void init_machikania_rtc(void){}
int ini_file_rtc(char* line){ return 0; }
int lib_rtc(int r0, int r1, int r2){ return 0; }
DWORD get_fattime (void){
	return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);
}

int gettime_function(void){
	g_default_args[1]=0;
	g_default_args[2]=0;
	return argn_function(LIB_RTC,
		ARG_STRING_OPTIONAL<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2|
		LIB_RTC_GETTIME<<LIBOPTION);
}

int settime_statement(void){
	return argn_function(LIB_RTC,
		ARG_STRING<<ARG1 |
		LIB_RTC_SETTIME<<LIBOPTION);
}

int strftime_function(void){
	g_default_args[2]=0;
	return argn_function(LIB_RTC,
		ARG_STRING<<ARG1 |
		ARG_STRING_OPTIONAL<<ARG2|
		LIB_RTC_STRFTIME<<LIBOPTION);
}
