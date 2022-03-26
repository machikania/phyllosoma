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
static struct repeating_timer g_drawcount_timer;
static unsigned short g_drawcount;

int drawcount_statement(void){
	return argn_function(LIB_TIMER,
		ARG_INTEGER<<ARG1 |
		TIMER_DRAWCOUNT<<LIBOPTION);
}

int drawcount_function(void){
	return argn_function(LIB_TIMER,
		ARG_NONE | 
		TIMER_DRAWCOUNTFUNC<<LIBOPTION);
}

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

bool repeating_drawcount_callback(struct repeating_timer *t) {
	g_drawcount++;
	return true;
}

void timer_init(void){
	cancel_repeating_timer(&g_timer);
	cancel_repeating_timer(&g_drawcount_timer);
	add_repeating_timer_us(16667, repeating_drawcount_callback, NULL, &g_drawcount_timer);
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
		case TIMER_DRAWCOUNT:
			g_drawcount=r0;
		case TIMER_DRAWCOUNTFUNC:
			return g_drawcount;
		default:
			stop_with_error(ERROR_UNKNOWN);
	}
	return r0;
}
