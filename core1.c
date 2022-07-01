/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "./core1.h"

/*
	Static variables.
	"1w0r" variables are written by core1 and read by core0
	"0w1r" variables are written by core0 and read by core1
*/

static void* g_core1_callback[CALLBACK_BUFFER_NUM];
static int g_core1_callback_at[CALLBACK_BUFFER_NUM];
static char g_1w0r_core1_busy;
static char g_1w0r_core1_started;
static char g_0w1r_core1_enabled;

void call_inner_requests(void){
	int i,j;
	// Call functions registered by request_core1_callback_at_us()
	for(i=0;i<CALLBACK_BUFFER_NUM;i++){
		if (!g_core1_callback[i]) continue;
		j=g_core1_callback_at[i]-time_us_32();
		if (1<j) continue;
		void(*f)(void)=g_core1_callback[i];
		g_core1_callback[i]=0;
		f();
		i=-1; // Start from beginning
	}
}

void call_outer_requests(void){
	// Call functions registered by request_core1_callback()
	while(multicore_fifo_rvalid()){
		void(*f)(void)=(void*)multicore_fifo_pop_blocking();
		f();
	}
}

void core1_entry(void) {
	int first;
	int now,i,j;
	g_1w0r_core1_busy=1;
	g_1w0r_core1_started=1;
	while(g_0w1r_core1_enabled){
		// Wait until next timing
		now=time_us_32();
		first=16667; // Slowest frequency to check requests is 60 Hz
		for(i=0;i<CALLBACK_BUFFER_NUM;i++){
			if (!g_core1_callback[i]) continue;
			j=g_core1_callback_at[i]-now;
			if (j<first) first=j;
		}
		if (1<first) {
			g_1w0r_core1_busy=0;
			sleep_until(now+first); // Must use "busy_wait_until()" ???
			g_1w0r_core1_busy=1;
		}
		// Call functions at requested timing
		call_inner_requests();
		// Call functions requested by core0
		call_outer_requests();
		// Call functions past due
		call_inner_requests();
	}
	g_1w0r_core1_started=0;
}

void start_core1(void){
	g_0w1r_core1_enabled=1;
	if (!g_1w0r_core1_started) multicore_launch_core1(core1_entry);
	while(!g_1w0r_core1_started) sleep_us(1);
}

void stop_core1(void){
	g_0w1r_core1_enabled=0;
	while(g_1w0r_core1_started) sleep_us(1);
}

void wait_core1_busy(void){
	while(g_1w0r_core1_busy) sleep_us(1);
}

void request_core1_callback(void* func){
	multicore_fifo_push_blocking((uint32_t)func);
}

void request_core1_callback_at(void* func, unsigned int at){
	int i;
	for(i=0;i<CALLBACK_BUFFER_NUM;i++){
		if (g_core1_callback[i]) continue;
		g_core1_callback[i]=func;
		g_core1_callback_at[i]=at;
		break;
	}
}
