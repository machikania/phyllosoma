/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "./compiler.h"
#include "./interface/ff.h"

// Flag to use RTC for timestamp when saving file
static char g_rtc4file=0;

// Timezone value * 4
static char g_timezome=0;

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
		g_timezome=(char)(f*4);
	} else if (!strncmp(line,"DAYLIGHTSAVING=",15)) {
		// Support daylight saving time (not supported yet)
		line+=15;
	} else {
		return 0;
	}
	return 1;
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
DWORD get_fattime (void){
	datetime_t now;
	if (g_rtc4file) {
		rtc_get_datetime(&now);
		return
			((now.year-1980)<<25) |
			(now.month<<21) |
			(now.day<<16) |
			(now.hour<<11) |
			(now.min<<5) |
			(now.sec>>1);
	} else {
		return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);
	}
}

void init_machikania_rtc(void){
    datetime_t t = {
            .year  = FF_NORTC_YEAR,
            .month = FF_NORTC_MON,
            .day   = FF_NORTC_MDAY,
            .dotw  = 0, // Week setting will be ignored
            .hour  = 0,
            .min   = 0,
            .sec   = 0
    };
	rtc_init();
	rtc_set_datetime(&t);
}

struct tm* datetime2tm(datetime_t* dt){
	static struct tm t;
	t.tm_sec = dt->sec;
	t.tm_min = dt->min;
	t.tm_hour = dt->hour;
	t.tm_mday = dt->day;
	t.tm_mon = dt->month - 1; // 0-11
	t.tm_year = dt->year - 1900; // year since 1900
	t.tm_wday = 0; // not used
	t.tm_yday = 0; // not used
	t.tm_isdst = -1; // not used
	return &t;
}

datetime_t* tm2datetime(struct tm* t){
	static datetime_t dt;
	dt.sec = t->tm_sec;
	dt.min = t->tm_min;
	dt.hour = t->tm_hour;
	dt.dotw = 0;  // Week setting will be ignored
	dt.day = t->tm_mday;
	dt.month = t->tm_mon + 1; // 0-11 to 1-12
	dt.year = t->tm_year + 1900; // year-1900 to year
	return &dt;
}

char* get_time_now(void){
	static char str[]="YYYY-MM-DDThh:mm:ss";
	datetime_t now;
	rtc_get_datetime(&now);
	strftime((char*)&str, sizeof str,"%Y-%m-%dT%H:%M:%S",datetime2tm(&now));
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

int lib_rtc(int r0, int r1, int r2){
	struct tm* tm;
	time_t t;
	datetime_t now;
	char* str;
	int i;
	switch(r2){
		case LIB_RTC_GETTIME:
			// r1 (1st argument): ISO-8601 string (optional)
			// r0 (2nd argument): adjustment in seconds
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
			rtc_set_datetime(tm2datetime(tm));
			break;
		case LIB_RTC_STRFTIME:
			// r1 (1st argument): format for strftime
			// r0 (2nd argument): ISO-8601 string (optional)
			if (r0) {
				tm=iso8601str2tm((char*)r0);
				garbage_collection((char*)r0);
			} else {
				rtc_get_datetime(&now);
				tm=datetime2tm(&now);
			}
			i=strlen((char*)r1)+1;
			str=alloc_memory(i,-1); // Allocate (bytes of string+1)*4 bytes memory
			strftime(str,i*4,(char*)r1,tm);
			return (int)str;
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
