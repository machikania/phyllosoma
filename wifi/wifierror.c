/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./wifi.h"

static char* g_err_str_wifi;
void wifi_set_error_str(char* err_str,...){
	g_err_str_wifi=err_str;
}
char* wifi_error_str(void){
	return g_err_str_wifi;
}

static int g_err_wifi;
void wifi_set_error(int err){
	g_err_wifi=err;
}
int wifi_error(void){
	return g_err_wifi;
}
