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
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "../compiler.h"
#include "../api.h"
#include "./wifi.h"

static char g_wifi_id[128]="MACHIKANIA_DEFAULT_WIFI_SSID\0\0\0";
static char g_wifi_passwd[128]="MACHIKANIA_DEFAULT_WIFI_PASSWD\0";
static char g_cyw43_country_char1='U';
static char g_cyw43_country_char2='S';
static char g_ntp_server[64]="pool.ntp.org";
static char g_usewifi=0;

int ini_file_wifi(char* line){
	int i;
	if (!strncmp(line,"USEWIFI",7)) {
		g_usewifi=1;
	} else if (!strncmp(line,"WIFISSID=",9)) {
		line+=9;
		for(i=0;i<(sizeof g_wifi_id)-1;i++){
			if ((g_wifi_id[i]=line[i])<=0x20) break;
		}
		g_wifi_id[i]=0;
	} else if (!strncmp(line,"WIFIPASSWD=",11)) {
		line+=11;
		for(i=0;i<(sizeof g_wifi_passwd)-1;i++){
			if ((g_wifi_passwd[i]=line[i])<=0x20) break;
		}
		g_wifi_passwd[i]=0;
	} else if (!strncmp(line,"WIFICOUNTRY=",12)) {
		line+=12;
		g_cyw43_country_char1=line[0];
		g_cyw43_country_char2=line[1];
	} else if (!strncmp(line,"NTPSERVER=",10)) {
		line+=10;
		for(i=0;i<(sizeof g_ntp_server)-1;i++){
			if ((g_ntp_server[i]=line[i])<=0x20) break;
		}
		g_ntp_server[i]=0;
	} else {
		return 0;
	}
	return 1;
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
	int err_wifi;
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
			err_wifi=err;
			break;
		case ERR_OK:
			// DNS look-up isn't needed. The server_name shows the IP address
		default:
			err_wifi=err2;
			break;
	}
	wifi_set_error(err_wifi);
	if (ERR_OK==err_wifi) return &g_server_address;
	return 0;
}

void wifi_test(void);
int connect_wifi(char show_progress){
	int i;
	if (!g_usewifi) {
		// First time calling this function. This must be done in main().
		// Second time calling this function must be from BASIC code.
		g_usewifi=1;
		return 0;
	}
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

char* get_time_now(void);
void wifi_test(void){
	int i;
	// DNS test
	ip_addr_t* ipaddr=dns_lookup("abehiroshi.la.coocan.jp");
	if (ipaddr) {
		printstr("abehiroshi.la.coocan.jp: ");
		printstr(ip4addr_ntoa(ipaddr));
		printstr("\n");
	}
	// RTC test
	for(i=0;i<10;i++){
		printstr(get_time_now());
		printstr("\n");
		sleep_ms(1000);
	}
}

int ifconfig_function(void){
	return argn_function(LIB_WIFI,
		ARG_INTEGER<<ARG1 |
		LIB_WIFI_IFCONFIG<<LIBOPTION);
}

int dns_function(void){
	return argn_function(LIB_WIFI,
		ARG_STRING<<ARG1 |
		LIB_WIFI_DNS<<LIBOPTION);
}

int wifi_statements(void){
	return ERROR_STATEMENT_NOT_DETECTED;
}
int wifi_int_functions(void){
	return ERROR_STATEMENT_NOT_DETECTED;
}
int wifi_str_functions(void){
	if (instruction_is("IFCONFIG$(")) return ifconfig_function();
	if (instruction_is("DNS$(")) return dns_function();
	return ERROR_STATEMENT_NOT_DETECTED;
}
int lib_wifi(int r0, int r1, int r2){
	switch(r2){
		case LIB_WIFI_IFCONFIG:
			switch(r0){
				case 0:
				default:
					return (int)ip4addr_ntoa(&cyw43_state.netif[0].ip_addr);
				case 1:
					return (int)ip4addr_ntoa(&cyw43_state.netif[0].netmask);
				case 2:
					return (int)ip4addr_ntoa((ip_addr_t*)&cyw43_state.netif[0].gw.addr);
				case 3:
					return (int)ip4addr_ntoa(dns_getserver(0));
			}
		case LIB_WIFI_DNS:
			return (int)ip4addr_ntoa(dns_lookup((char*)r0));
		default:
			break;
	}
	return r0;
}
