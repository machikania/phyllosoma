/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"

int coretimer_function(){
	return argn_function(LIB_TIMER,
		ARG_NONE | 
		TIMER_CORETIMERFUNC<<LIBOPTION);
}

int lib_timer(int r0, int r1, int r2){
	switch(r2){
		case TIMER_CORETIMER:
			break;
		case TIMER_USETIMER:
			break;
		case TIMER_TIMER:
			break;
		case TIMER_CORETIMERFUNC:
			return time_us_32();
		case TIMER_TIMERFUNC:
			break;
		default:
			stop_with_error(ERROR_UNKNOWN);
	}
	return r0;
}
