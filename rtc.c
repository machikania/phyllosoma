/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "pico/aon_timer.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "./compiler.h"
#include "./interface/ff.h"

// Flag to use RTC for timestamp when saving file
static char g_rtc4file=0;

// Timezone value * 4
static int g_timezome=0;

int ini_file_rtc(char* line){
	float f;
	if (!strncmp(line,"RTCFILE",7)) {
		g_rtc4file=1;
	} else if (!strncmp(line,"TIMEZONE=",9)) {
		// TIMEZONE value is between -12.0 to +14.0
		line+=9;
		if ('+'==line[0]) line++;
		f=strtof(line,NULL);
		// g_timezone is 4xTIMEZONE, which is between -48 to 56
		g_timezome=(int)(f*4);
	} else if (!strncmp(line,"DAYLIGHTSAVING=",15)) {
		// Support daylight saving time (not supported yet)
		line+=15;
	} else {
		return 0;
	}
	return 1;
}

void init_machikania_rtc(void){
	struct tm ttm={
		.tm_sec=0,                   // seconds after the minute (0-59)
		.tm_min=0,                   // minutes after the hour (0-59)
		.tm_hour=0,                  // hours since midnight (0-23)
		.tm_mday=FF_NORTC_MDAY,      // day of the month (1-31)
		.tm_mon=FF_NORTC_MON-1,      // months since January (0-11)
		.tm_year=FF_NORTC_YEAR-1900, // years since 1900
		.tm_wday=0,                  // days since Sunday (0-6)
		.tm_yday=0,                  // days since January 1 (0-365)
		.tm_isdst=0                  // Daylight Saving Time flag
	};
	struct timespec tts={
		.tv_sec=0,
		.tv_nsec=0
	};
	tts.tv_sec=mktime(&ttm);
	aon_timer_start(&tts);
}

struct tm* get_tm_now(void){
	struct timespec tts;
	time_t t;
	aon_timer_get_time(&tts);
	t=tts.tv_sec;
	return localtime(&t);
}

/*
	DWORD get_fattime (void);
		bit31:25 (7 bits) Year origin from the 1980 (0..127, e.g. 37 for 2017)
		bit24:21 (4 bits) Month (1..12)
		bit20:16 (5 bits) Day of the month (1..31)
		bit15:11 (5 bits) Hour (0..23)
		bit10:5  (6 bits) Minute (0..59)
		bit4:0   (5 bits) Second / 2 (0..29, e.g. 25 for 50) 
*/
DWORD get_fattime(void){
	struct tm* ttm;
	if (g_rtc4file) {
		ttm=get_tm_now();
		return
			((ttm->tm_year-80)<<25) |
			((ttm->tm_mon+1)<<21) |
			(ttm->tm_mday<<16) |
			(ttm->tm_hour<<11) |
			(ttm->tm_min<<5) |
			(ttm->tm_sec>>1);
	} else {
		return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);
	}
}

char* get_time_now(void){
	static char str[]="YYYY-MM-DDThh:mm:ss";
	strftime((char*)&str, sizeof str,"%Y-%m-%dT%H:%M:%S",get_tm_now());
	return str;
}

struct tm* iso8601str2tm(char* iso8601){
	// 0    5  8 11 14 17
	// |    |  |  |  |  |
	// YYYY-MM-DDThh:mm:ss
	static struct tm t;
	t.tm_sec = strtol(iso8601+17,NULL,10);
	t.tm_min = strtol(iso8601+14,NULL,10);
	t.tm_hour = strtol(iso8601+11,NULL,10);
	t.tm_mday = strtol(iso8601+8,NULL,10);
	t.tm_mon = strtol(iso8601+5,NULL,10) - 1; // 0-11
	t.tm_year = strtol(iso8601+0,NULL,10) - 1900; // year since 1900
	t.tm_wday = 0; // not used
	t.tm_yday = 0; // not used
	t.tm_isdst = -1; // not used
	return &t;
}

void set_time_from_utc(time_t t){
	struct timespec tts={
		.tv_sec=0,
		.tv_nsec=0
	};
	if (0<g_timezome) {
		t+=(3600/4)*g_timezome;
	} else if (g_timezome<0) {
		t-=(3600/4)*(0-g_timezome);
	}
	tts.tv_sec=t;
	aon_timer_set_time(&tts);
}

int lib_rtc(int r0, int r1, int r2){
	struct tm* tm;
	time_t t;
	char* str;
	int i;
	struct timespec tts={
		.tv_sec=0,
		.tv_nsec=0
	};
	switch(r2){
		case LIB_RTC_GETTIME:
			// r1 (1st argument): ISO-8601 string (optional)
			// r0 (2nd argument): adjustment in seconds
			if (!r1) return (int)get_time_now();
			if (!r1) return (int)get_time_now();
			t=mktime(iso8601str2tm((char*)r1))+r0;
			garbage_collection((char*)r1);
			tm=localtime(&t);
			str=alloc_memory(5,-1);
			strftime(str,20,"%Y-%m-%dT%H:%M:%S",tm);
			return (int)str;
		case LIB_RTC_SETTIME:
			// r0 (1st argument): ISO-8601 string
			tm=iso8601str2tm((char*)r0);
			garbage_collection((char*)r0);
			tts.tv_sec=mktime(tm);
			aon_timer_set_time(&tts);
			break;
		case LIB_RTC_STRFTIME:
			// r1 (1st argument): format for strftime
			// r0 (2nd argument): ISO-8601 string (optional)
			if (r0) {
				t=mktime(iso8601str2tm((char*)r0));
				garbage_collection((char*)r0);
				tm=localtime(&t);
			} else {
				tm=get_tm_now();
			}
			i=strlen((char*)r1)+1;
			str=alloc_memory(i,-1); // Allocate (bytes of string+1)*4 bytes memory
			strftime(str,i*4,(char*)r1,tm);
			return (int)str;
			return 0;
		default:
			stop_with_error(ERROR_UNKNOWN);
	}
	return r0;
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
