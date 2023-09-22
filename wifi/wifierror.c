/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "pico/cyw43_arch.h"
#include "./wifi.h"

static char g_err_str_wifi[32]="No error";
void wifi_set_error_str(char* err_str,...){
	int i;
	for(i=0;i<31;i++) {
		if (0==(g_err_str_wifi[i]=err_str[i])) break;
	}
}
char* wifi_error_str(void){
	return &g_err_str_wifi[0];
}

static int g_err_wifi;
void wifi_set_error(int err){
	if (err!=g_err_wifi) {
		switch(err){
			case WIFI_ERROR_NO_ERROR:
				wifi_set_error_str("No error");
				break;
			case WIFI_ERROR_CONNECTION_CLOSED:
				wifi_set_error_str("Connection closed");
				break;
			case WIFI_ERROR_DNS_ERROR:
				wifi_set_error_str("DNS error");
				break;
			case WIFI_ERROR_CONNECTION_ERROR:
				wifi_set_error_str("Connection error");
				break;
			case WIFI_ERROR_WIFI_ERROR:
				wifi_set_error_str("WiFi not connected");
				break;
			default:
				break;
		}
	}
	g_err_wifi=err;
}

int wifi_error(void){
	return g_err_wifi;
}
