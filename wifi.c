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
#include "./compiler.h"
#include "./api.h"

char g_ssid[] = WIFI_SSID;
char g_pass[] = WIFI_PASSWORD;

char g_cyw43_country_char1='U';
char g_cyw43_country_char2='S';

static int g_err_wifi;

int wifi_error(void){
	return g_err_wifi;
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

int connect_wifi(char show_progress){
	int i;
	if (show_progress) printstr("\nInitialising wifi ... ");
	if (cyw43_arch_init_with_country(CYW43_COUNTRY(g_cyw43_country_char1, g_cyw43_country_char2, 0))) {
		if (show_progress) printf("failed to initialise\n");
		return 1;
	}
	if (show_progress) printstr("OK\n");
	cyw43_arch_enable_sta_mode();
	for(i=0;i<5;i++){
		if (cyw43_arch_wifi_connect_timeout_ms(g_ssid, g_pass, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
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
	// DNS test
	ip_addr_t* ipaddr=dns_lookup("abehiroshi.la.coocan.jp");
	if (ipaddr) {
		printstr("abehiroshi.la.coocan.jp: ");
		printstr(ip4addr_ntoa(ipaddr));
		printstr("\n");
	}
	sleep_ms(2000);
	return 0;
}