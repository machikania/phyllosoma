/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"

static struct repeating_timer g_timer;
static int g_timer_counter;
static struct repeating_timer g_drawcount_timer;
static unsigned short g_drawcount;
static void* g_interrupt_vector[7];
static struct repeating_timer g_coretimer_timer;
static unsigned int g_interrupt_flags;

void raise_interrupt_flag(int i){
	g_interrupt_flags|=1<<i;
}

void drop_interrupt_flag(int i){
	g_interrupt_flags&=~(1<<i);
}

int interrupt_statement(void){
	unsigned short* bl;
	int e;
	int type;
	int stop=0;
	if (instruction_is("STOP")) {
		skip_blank();
		stop=1;
	}
	if (instruction_is("TIMER")) type=INTERRUPT_TIMER;
	else if (instruction_is("DRAWCOUNT")) type=INTERRUPT_DRAWCOUNT;
	else if (instruction_is("KEYS")) type=INTERRUPT_KEYS;
	else if (instruction_is("INKEY")) type=INTERRUPT_INKEY;
	else if (instruction_is("MUSIC")) type=INTERRUPT_MUSIC;
	else if (instruction_is("WAVE")) type=INTERRUPT_WAVE;
	else if (instruction_is("CORETIMER")) type=INTERRUPT_CORETIMER;
	else return ERROR_SYNTAX;
	if (stop) {
		// Set type in r0
		e=set_value_in_register(0,type);
		if (e) return e;
		// Set zero in r1
		e=set_value_in_register(1,0);
		if (e) return e;
	} else {
		if (','!=source[0]) return ERROR_SYNTAX;
		source++;
		skip_blank();
		// BL instruction
		check_object(2);
		bl=object;
		object+=2;
		// Insert a gosub statement
		check_object(1);
		(object++)[0]=0xb500; // push	{lr}
		e=gosub_statement();
		if (e) return e;
		check_object(1);
		(object++)[0]=0xbd00; // pop	{pc}
		// BL jump destination is here
		update_bl(bl,object);
		// Set type in r0
		e=set_value_in_register(0,type);
		if (e) return e;
		// Set interrupt vector in r1
		e=set_value_in_register(1,1+(int)&bl[2]);
		if (e) return e;
	}
	return call_lib_code(LIB_INTERRUPT);
}

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

int coretimer_statement(void){
	return argn_function(LIB_TIMER,
		ARG_INTEGER<<ARG1 |
		TIMER_CORETIMER<<LIBOPTION);
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
	if (g_interrupt_vector[INTERRUPT_TIMER]) call_interrupt_function(g_interrupt_vector[INTERRUPT_TIMER]);
	return true;
}

bool repeating_drawcount_callback(struct repeating_timer *t) {
	static char s_keys=-1;
	char keys;
	g_drawcount++;
	if (g_interrupt_vector[INTERRUPT_DRAWCOUNT]) call_interrupt_function(g_interrupt_vector[INTERRUPT_DRAWCOUNT]);
	if (g_interrupt_vector[INTERRUPT_KEYS]) {
		keys=lib_keys(63,0,0);
		if (0<=s_keys && s_keys!=keys) call_interrupt_function(g_interrupt_vector[INTERRUPT_KEYS]);
		s_keys=keys;
	} else {
		s_keys=-1;
	}
	musicint();
	if (g_interrupt_vector[INTERRUPT_MUSIC]) {
		if (g_interrupt_flags&(1<<INTERRUPT_MUSIC)) call_interrupt_function(g_interrupt_vector[INTERRUPT_MUSIC]);
		drop_interrupt_flag(INTERRUPT_MUSIC);
	}
	if (g_interrupt_vector[INTERRUPT_WAVE]) {
		if (g_interrupt_flags&(1<<INTERRUPT_WAVE)) {
			call_interrupt_function(g_interrupt_vector[INTERRUPT_WAVE]);
		}
		drop_interrupt_flag(INTERRUPT_WAVE);
	}
	if (g_interrupt_vector[INTERRUPT_INKEY]) {
		if (check_keypress()) call_interrupt_function(g_interrupt_vector[INTERRUPT_INKEY]);
		drop_interrupt_flag(INTERRUPT_INKEY);
	}
	return true;
}

int64_t alarm_coretimer_callback(alarm_id_t id, void *user_data) {
	if (g_interrupt_vector[INTERRUPT_CORETIMER]) call_interrupt_function(g_interrupt_vector[INTERRUPT_CORETIMER]);
	return 0;
}

void cancel_all_interrupts(void){
	int i;
	// Cancel timers
	cancel_repeating_timer(&g_timer);
	cancel_repeating_timer(&g_coretimer_timer);
	// Cancel all interrupts
	for(i=0;i<(sizeof g_interrupt_vector)/(sizeof g_interrupt_vector[0]);i++) g_interrupt_vector[i]=0;
	g_interrupt_flags=0;
}

void timer_init(void){
	int i;
	// Cancel all timers, first
	cancel_repeating_timer(&g_timer);
	cancel_repeating_timer(&g_drawcount_timer);
	cancel_repeating_timer(&g_coretimer_timer);
	// Cancel all interrupts
	for(i=0;i<(sizeof g_interrupt_vector)/(sizeof g_interrupt_vector[0]);i++) g_interrupt_vector[i]=0;
	g_interrupt_flags=0;
	// Start drawcount interrupt (every 1/60 sec)
	add_repeating_timer_us(-16667, repeating_drawcount_callback, NULL, &g_drawcount_timer);
}

int lib_timer(int r0, int r1, int r2){
	uint64_t ui64;
	switch(r2){
		case TIMER_CORETIMER:
			cancel_repeating_timer(&g_coretimer_timer);
			ui64=time_us_64()&0xffffffff00000000;
			ui64|=(unsigned int)r0;
			while(ui64<time_us_64()) ui64+=0x100000000;
			add_alarm_at(ui64,alarm_coretimer_callback,NULL,&g_coretimer_timer);
			break;
		case TIMER_USETIMER:
			cancel_repeating_timer(&g_timer);
			g_timer_counter=0;
			add_repeating_timer_us(0-r0, repeating_timer_callback, NULL, &g_timer);
			break;
		case TIMER_TIMER:
			g_timer_counter=r0;
			break;
		case TIMER_CORETIMERFUNC:
			return time_us_32();
		case TIMER_TIMERFUNC:
			return g_timer_counter;
		case TIMER_DRAWCOUNT:
			if (PUERULUS) {
				drawcount=r0;
			} else {
				g_drawcount=r0;
			}
		case TIMER_DRAWCOUNTFUNC:
			if (PUERULUS) {
				return drawcount;
			} else {
				return g_drawcount;
			}
		default:
			stop_with_error(ERROR_UNKNOWN);
	}
	return r0;
}

int lib_interrupt(int r0, int r1, int r2){
	switch(r0) {
		case INTERRUPT_TIMER:
		case INTERRUPT_DRAWCOUNT:
		case INTERRUPT_KEYS:
		case INTERRUPT_INKEY:
		case INTERRUPT_MUSIC:
		case INTERRUPT_WAVE:
		case INTERRUPT_CORETIMER:
			g_interrupt_vector[r0]=(void*)r1;
			drop_interrupt_flag(r0);
			break;
		default:
			break;
	}
	return r0;
}