/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "pico/stdlib.h"
#include "../compiler.h"

void board_led(int led_on){
	static char init=0;
	if (!init) {
		init=1;
	    gpio_init(PICO_DEFAULT_LED_PIN);
		gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
		
	}
	gpio_put(PICO_DEFAULT_LED_PIN, led_on ? 1:0);
}

int ini_file_wifi(char* line){
	// Do nothing	
	return 0;
}
void pre_run_wifi(void){ }
void post_run_wifi(void){ }
int connect_wifi(char show_progress){
	// Return error
	return 1;
}
int wifi_statements(void){
	return ERROR_STATEMENT_NOT_DETECTED;
}
int wifi_int_functions(void){
	return ERROR_STATEMENT_NOT_DETECTED;
}
int wifi_str_functions(void){
	return ERROR_STATEMENT_NOT_DETECTED;
}
int lib_wifi(int r0, int r1, int r2){
	return r0;
}
