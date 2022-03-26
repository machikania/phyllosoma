/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"

static struct repeating_timer g_timer;
static int g_timer_counter;

int coretimer_function(void){
	return argn_function(LIB_TIMER,
		ARG_NONE | 
		TIMER_CORETIMERFUNC<<LIBOPTION);
}

int usetimer_statement(void){
	return argn_function(LIB_TIMER,
		ARG_INTEGER<<ARG1 |
		TIMER_USETIMER<<LIBOPTION);
}

int timer_statement(void){
	return argn_function(LIB_TIMER,
		ARG_INTEGER<<ARG1 |
		TIMER_TIMER<<LIBOPTION);
}

int timer_function(void){
	return argn_function(LIB_TIMER,
		ARG_NONE | 
		TIMER_TIMERFUNC<<LIBOPTION);
}

bool repeating_timer_callback(struct repeating_timer *t) {
	g_timer_counter++;
	return true;
}

int lib_timer(int r0, int r1, int r2){
	switch(r2){
		case TIMER_CORETIMER:
			break;
		case TIMER_USETIMER:
			cancel_repeating_timer(&g_timer);
			g_timer_counter=0;
			add_repeating_timer_us(r0, repeating_timer_callback, NULL, &g_timer);
			break;
		case TIMER_TIMER:
			g_timer_counter=r0;
			break;
		case TIMER_CORETIMERFUNC:
			return time_us_32();
		case TIMER_TIMERFUNC:
			return g_timer_counter;
		default:
			stop_with_error(ERROR_UNKNOWN);
	}
	return r0;
}
