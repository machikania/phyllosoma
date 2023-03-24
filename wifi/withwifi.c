/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu

   Some part of this file came from codes with following license,
   and the license has been changed to LGPL

/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "../compiler.h"
#include "../api.h"


#ifdef WIFI_SSID
#define MACHIKANIA_DEFAULT_WIFI_SSID WIFI_SSID
#else
#define MACHIKANIA_DEFAULT_WIFI_SSID "MACHIKANIA_DEFAULT_WIFI_SSID\0\0\0"
#endif

#ifdef WIFI_PASSWORD
#define MACHIKANIA_DEFAULT_WIFI_PASSWORD WIFI_PASSWORD
#else
#define MACHIKANIA_DEFAULT_WIFI_PASSWORD "MACHIKANIA_DEFAULT_WIFI_PASSWD\0"
#endif


static char g_wifi_id[128]=MACHIKANIA_DEFAULT_WIFI_SSID;
static char g_wifi_passwd[128]=MACHIKANIA_DEFAULT_WIFI_PASSWORD;
static char g_cyw43_country_char1='U';
static char g_cyw43_country_char2='S';

#define printf wifi_set_error
static char* g_err_str_wifi;
void wifi_set_error(char* err_str,...){
	g_err_str_wifi=err_str;
}
char* wifi_error_str(void){
	return g_err_str_wifi;
}

static int g_err_wifi;
int wifi_error(void){
	return g_err_wifi;
}

void set_wifi_id(char* wifi_id){
	int i;
	for(i=0;i<(sizeof g_wifi_id)-1;i++){
		if ((g_wifi_id[i]=wifi_id[i])<=0x20) break;
	}
	g_wifi_id[i]=0;
}

void set_wifi_passwd(char* wifi_passwd){
	int i;
	for(i=0;i<(sizeof g_wifi_passwd)-1;i++){
		if ((g_wifi_passwd[i]=wifi_passwd[i])<=0x20) break;
	}
	g_wifi_passwd[i]=0;
}

void set_wifi_country(char* country){
	g_cyw43_country_char1=country[0];
	g_cyw43_country_char2=country[1];
}

static ip_addr_t g_server_address;
static void dns_lookup_callback(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
	int *err=(int*)arg;
	if (ipaddr) {
		g_server_address = *ipaddr;
		*err=ERR_OK;
	} else {
		*err=ERR_CONN;
	}
}

ip_addr_t* dns_lookup(char* server_name){
	int i;
	volatile int err;
	cyw43_arch_lwip_begin();
	int err2=dns_gethostbyname(server_name, &g_server_address, dns_lookup_callback, (void*)&err);
	cyw43_arch_lwip_end();
	switch(err2){
		case ERR_INPROGRESS:
			// DNS look-up is in progress
			// The call back function will be called when look-up will be done
			// Waiting time for result: 15 seconds
			err=ERR_INPROGRESS;
			for(i=0;i<1500;i++){
				if (ERR_INPROGRESS!=err) break;
				sleep_ms(10);
			}
			g_err_wifi=err;
			break;
		case ERR_OK:
			// DNS look-up isn't needed. The server_name shows the IP address
		default:
			g_err_wifi=err2;
			break;
	}
	if (ERR_OK==g_err_wifi) return &g_server_address;
	return 0;
}

void wifi_test(void);
int connect_wifi(char show_progress){
	int i;
	if (show_progress) printstr("\nInitialising wifi ... ");
	if (cyw43_arch_init_with_country(CYW43_COUNTRY(g_cyw43_country_char1, g_cyw43_country_char2, 0))) {
		if (show_progress) printstr("failed to initialise\n");
		return 1;
	}
	if (show_progress) printstr("OK\n");
	cyw43_arch_enable_sta_mode();
	for(i=0;i<5;i++){
		if (cyw43_arch_wifi_connect_timeout_ms(g_wifi_id, g_wifi_passwd, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
			if (show_progress) printstr("failed to connect\n");
			if (4==i) return 1;
			if (show_progress) printstr("try it again... ");
		} else {
			break;
		}
	}	
	if (show_progress) {
		printstr("connected as ");
		printstr(ip4addr_ntoa(&cyw43_state.netif[0].ip_addr));
		printstr("\n");
	}
	wifi_test();
	return 0;
}

void wifi_test(void){
	// DNS test
	ip_addr_t* ipaddr=dns_lookup("abehiroshi.la.coocan.jp");
	if (ipaddr) {
		printstr("abehiroshi.la.coocan.jp: ");
		printstr(ip4addr_ntoa(ipaddr));
		printstr("\n");
	}
	sleep_ms(10000);
}
