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
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "./compiler.h"
#include "./api.h"

char g_ssid[] = WIFI_SSID;
char g_pass[] = WIFI_PASSWORD;

char g_cyw43_country_char1='U';
char g_cyw43_country_char2='S';

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
	wifi_test();
	return 0;
}

/*
	NTP support codes follow
*/

typedef struct NTP_T_ {
	ip_addr_t ntp_server_address;
	bool dns_request_sent;
	struct udp_pcb *ntp_pcb;
	absolute_time_t ntp_test_time;
	alarm_id_t ntp_resend_alarm;
} NTP_T;

#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800 // seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_TEST_TIME (30 * 1000)
#define NTP_RESEND_TIME (10 * 1000)

static struct tm* g_utc;

// Called with results of operation
static void ntp_result(NTP_T* state, int status, time_t *result) {
	if (status == 0 && result) {
		g_utc = gmtime(result);
		/*printf("got ntp response: %02d/%02d/%04d %02d:%02d:%02d\n", g_utc->tm_mday, g_utc->tm_mon + 1, g_utc->tm_year + 1900,
			   g_utc->tm_hour, g_utc->tm_min, g_utc->tm_sec);//*/
	}

	if (state->ntp_resend_alarm > 0) {
		cancel_alarm(state->ntp_resend_alarm);
		state->ntp_resend_alarm = 0;
	}
	state->ntp_test_time = make_timeout_time_ms(NTP_TEST_TIME);
	state->dns_request_sent = false;
}

static int64_t ntp_failed_handler(alarm_id_t id, void *user_data);

// Make an NTP request
static void ntp_request(NTP_T *state) {
	// cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
	// You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
	// these calls are a no-op and can be omitted, but it is a good practice to use them in
	// case you switch the cyw43_arch type later.
	cyw43_arch_lwip_begin();
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
	uint8_t *req = (uint8_t *) p->payload;
	memset(req, 0, NTP_MSG_LEN);
	req[0] = 0x1b;
	udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
	pbuf_free(p);
	cyw43_arch_lwip_end();
}

static int64_t ntp_failed_handler(alarm_id_t id, void *user_data)
{
	NTP_T* state = (NTP_T*)user_data;
	printf("ntp request failed\n");
	ntp_result(state, -1, NULL);
	return 0;
}

// Call back with a DNS result
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
	NTP_T *state = (NTP_T*)arg;
	if (ipaddr) {
		state->ntp_server_address = *ipaddr;
		printf("ntp address %s\n", ip4addr_ntoa(ipaddr));
		ntp_request(state);
	} else {
		printf("ntp dns request failed\n");
		ntp_result(state, -1, NULL);
	}
}

// NTP data received
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
	NTP_T *state = (NTP_T*)arg;
	uint8_t mode = pbuf_get_at(p, 0) & 0x7;
	uint8_t stratum = pbuf_get_at(p, 1);

	// Check the result
	if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
		mode == 0x4 && stratum != 0) {
		uint8_t seconds_buf[4] = {0};
		pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
		uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
		uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
		time_t epoch = seconds_since_1970;
		ntp_result(state, 0, &epoch);
	} else {
		printf("invalid ntp response\n");
		ntp_result(state, -1, NULL);
	}
	pbuf_free(p);
}

// Perform initialisation
static NTP_T* ntp_init(void) {
	NTP_T *state = calloc(1, sizeof(NTP_T));
	if (!state) {
		printf("failed to allocate state\n");
		return NULL;
	}
	state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
	if (!state->ntp_pcb) {
		printf("failed to create pcb\n");
		free(state);
		return NULL;
	}
	udp_recv(state->ntp_pcb, ntp_recv, state);
	return state;
}

struct tm* get_time_from_npt(void){
	int i,j;
	g_utc=0;
	NTP_T *state = ntp_init();
	if (!state) return 0;
	for(i=0;i<15;i++){
		if (absolute_time_diff_us(get_absolute_time(), state->ntp_test_time) < 0 && !state->dns_request_sent) {
			state->ntp_resend_alarm = add_alarm_in_ms(NTP_RESEND_TIME, ntp_failed_handler, state, true);
			cyw43_arch_lwip_begin();
			int err = dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state);
			cyw43_arch_lwip_end();
			state->dns_request_sent = true;
			if (err == ERR_OK) {
				ntp_request(state); // Cached result
			} else if (err != ERR_INPROGRESS) { // ERR_INPROGRESS means expect a callback
				printf("dns request failed\n");
				ntp_result(state, -1, NULL);
			}
		}		
		for(j=0;j<1000;j++){
			if (g_utc) break;
			sleep_ms(1);
		}
		if (g_utc) break;
	}
	free(state);
	return g_utc;
}

/*
	Test code follows
*/

void wifi_test(void){
	char buff[128];
	// DNS test
	ip_addr_t* ipaddr=dns_lookup("abehiroshi.la.coocan.jp");
	if (ipaddr) {
		printstr("abehiroshi.la.coocan.jp: ");
		printstr(ip4addr_ntoa(ipaddr));
		printstr("\n");
	}
	// NTP test
	struct tm* utc=get_time_from_npt();
	if (utc) {
		strftime(buff,128,"%Y/%m/%d %H:%M:%S %A UTC\n",utc);
		printstr(buff);
		/*printint(utc->tm_year + 1900);
		printchar('/');
		printint(utc->tm_mon + 1);
		printchar('/');
		printint(utc->tm_mday);
		printchar(' ');
		printint(utc->tm_hour);
		printchar(':');
		printint(utc->tm_min);
		printchar(':');
		printint(utc->tm_sec);
		printstr(" UTC\n");//*/
		utc->tm_hour-=7;
		time_t now=mktime(utc);
		struct tm* ts=gmtime(&now);
		strftime(buff,128,"%Y/%m/%d %H:%M:%S %A PST\n",utc);
		printstr(buff);
		/*printint(ts->tm_year + 1900);
		printchar('/');
		printint(ts->tm_mon + 1);
		printchar('/');
		printint(ts->tm_mday);
		printchar(' ');
		printint(ts->tm_hour);
		printchar(':');
		printint(ts->tm_min);
		printchar(':');
		printint(ts->tm_sec);
		printstr(" PST\n");//*/
	}
	sleep_ms(10000);
}