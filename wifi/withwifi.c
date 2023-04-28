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
static char g_initial_ntp=0;

void board_led(int led_on){
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on ? 1:0);
}

int ini_file_wifi(char* line){
	int i;
	if (!strncmp(line,"USEWIFI",7)) {
		g_usewifi=1;
	} else if (!strncmp(line,"INITIALNTP",10)) {
		g_initial_ntp=1;
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

void pre_run_wifi(void){
	init_socket_system();
}

void post_run_wifi(void){
	// Close all connection if running
	machikania_tcp_close(0);
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

int connect_wifi(char show_progress){
	int i;
	if (!g_usewifi) {
		// First time calling this function. This must be done in main().
		// Second time calling this function must be from BASIC code.
		g_usewifi=1;
		return 0;
	}
	if (show_progress) printstr("\nInitialising wifi... ");
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
	if (g_initial_ntp) {
		if (show_progress) printstr("Connecting to NTP server... ");
		for(i=0;i<5;i++){
			if (lib_wifi(0,0,LIB_WIFI_NTP)) {
				if (show_progress) printstr("failed.\n");
				if (show_progress) printstr("Try it again... ");
			} else {
				if (show_progress) printstr("OK \n");
				break;
			}
		}
	}
	sleep_ms(1000);
	return 0;
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

int ntp_function(void){
	return argn_function(LIB_WIFI,
		LIB_WIFI_NTP<<LIBOPTION);
}

int wifierr_int_function(void){
	return argn_function(LIB_WIFI,
		LIB_WIFI_ERR_INT<<LIBOPTION);
}

int wifierr_str_function(void){
	return argn_function(LIB_WIFI,
		LIB_WIFI_ERR_STR<<LIBOPTION);
}

int tcpclient_function(void){
	g_default_args[2]=80;
	return argn_function(LIB_WIFI,
		ARG_STRING<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_WIFI_TCPCLIENT<<LIBOPTION);
}

int tlsclient_function(void){
	g_default_args[2]=443;
	return argn_function(LIB_WIFI,
		ARG_STRING<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_WIFI_TLSCLIENT<<LIBOPTION);
}

int tcpserver_statement(void){
	g_default_args[1]=80;
	g_default_args[2]=1;
	return argn_function(LIB_WIFI,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_WIFI_TCPSERVER<<LIBOPTION);
}

int tcpstatus_function(void){
	g_default_args[2]=0;
	return argn_function(LIB_WIFI,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_WIFI_TCPSTATUS<<LIBOPTION);
}

int tcpsend_function(void){
	// Test if string
	char* sbefore=source;
	unsigned short* obefore=object;
	int e=get_string();
	source=sbefore;
	rewind_object(obefore);
	// The 1st argument can be string or integer
	g_default_args[2]=-1;
	g_default_args[3]=0;
	if (e) {
		return argn_function(LIB_WIFI,
			ARG_INTEGER<<ARG1 |
			ARG_INTEGER_OPTIONAL<<ARG2 |
			ARG_INTEGER_OPTIONAL<<ARG3 |
			LIB_WIFI_TCPSEND<<LIBOPTION);
	} else {
		return argn_function(LIB_WIFI,
			ARG_STRING<<ARG1 |
			ARG_INTEGER_OPTIONAL<<ARG2 |
			ARG_INTEGER_OPTIONAL<<ARG3 |
			LIB_WIFI_TCPSEND<<LIBOPTION);
	}
}

int tcpreceive_function(void){
	g_default_args[3]=0;
	return argn_function(LIB_WIFI,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		ARG_INTEGER_OPTIONAL<<ARG3 |
		LIB_WIFI_TCPRECEIVE<<LIBOPTION);
}

int tcpclose_function(void){
	g_default_args[1]=0;
	return argn_function(LIB_WIFI,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		LIB_WIFI_TCPCLOSE<<LIBOPTION);
}

int tcpaccept_function(void){
	return argn_function(LIB_WIFI,
		LIB_WIFI_TCPACCEPT<<LIBOPTION);
}

int wifi_statements(void){
	if (instruction_is("NTP")) return ntp_function();
	if (instruction_is("TCPCLIENT")) return tcpclient_function();
	if (instruction_is("TLSCLIENT")) return tlsclient_function();
	if (instruction_is("TCPSERVER")) return tcpserver_statement();
	if (instruction_is("TCPSEND")) return tcpsend_function();
	if (instruction_is("TCPRECEIVE")) return tcpreceive_function();
	if (instruction_is("TCPCLOSE")) return tcpclose_function();
	return ERROR_STATEMENT_NOT_DETECTED;
}

int wifi_int_functions(void){
	if (instruction_is("NTP(")) return ntp_function();
	if (instruction_is("WIFIERR(")) return wifierr_int_function();
	if (instruction_is("TCPCLIENT(")) return tcpclient_function();
	if (instruction_is("TLSCLIENT(")) return tlsclient_function();
	if (instruction_is("TCPSTATUS(")) return tcpstatus_function();
	if (instruction_is("TCPSEND(")) return tcpsend_function();
	if (instruction_is("TCPRECEIVE(")) return tcpreceive_function();
	if (instruction_is("TCPCLOSE(")) return tcpclose_function();
	if (instruction_is("TCPACCEPT(")) return tcpaccept_function();
	return ERROR_STATEMENT_NOT_DETECTED;
}

int wifi_str_functions(void){
	if (instruction_is("IFCONFIG$(")) return ifconfig_function();
	if (instruction_is("DNS$(")) return dns_function();
	if (instruction_is("WIFIERR$(")) return wifierr_str_function();
	return ERROR_STATEMENT_NOT_DETECTED;
}

int lib_wifi(int r0, int r1, int r2){
	int* sp=(int*)r1;
	static char iso8601str[]="YYYY-MM-DDThh:mm:ss";
	time_t* now;
	switch(r2){
		case LIB_WIFI_ERR_INT:
			return wifi_error();
		case LIB_WIFI_ERR_STR:
			return (int)wifi_error_str();
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
		case LIB_WIFI_NTP:
			now=get_ntp_time(g_ntp_server);
			if (!now) {
				r0=wifi_error();
				return r0 ? r0:1;
			}
			set_time_from_utc(now[0]);
			return 0;
		case LIB_WIFI_TCPCLIENT:
			start_tcp_client(ip4addr_ntoa(dns_lookup((char*)r1)),r0);
			return 0;
		case LIB_WIFI_TCPSTATUS:
			return machikania_tcp_status(r1,(void**)r0);
		case LIB_WIFI_TCPSEND:
			if (sp[1]<0) return machikania_tcp_write((char*)sp[0],strlen((char*)sp[0]),(void**)r0);
			return machikania_tcp_write((char*)sp[0],sp[1],(void**)r0);
		case LIB_WIFI_TCPRECEIVE:
			return tcp_read_from_buffer((char*)sp[0],sp[1],(void**)r0);
		case LIB_WIFI_TCPCLOSE:
			return machikania_tcp_close((void**)r0);
		case LIB_WIFI_TCPSERVER:
			start_tcp_server(r1,r0);
			return 0;
		case LIB_WIFI_TCPACCEPT:
			return (int)shift_pcb_fifo();
		case LIB_WIFI_TLSCLIENT:
			return (int)run_tls_client_test();
		default:
			break;
	}
	return r0;
}
