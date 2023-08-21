/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu

   Some part of this file came from codes with following license,
   and the license has been changed to LGPL

/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

#include "./wifi.h"
#include "../compiler.h"
#include "../api.h"
#include "../core1.h"

#define TLS_CLIENT_TIMEOUT_SECS  15

typedef struct TLS_CLIENT_T_ {
	struct altcp_pcb *pcb;
	u16_t port;
	bool complete;
	bool core1;
} TLS_CLIENT_T;

static struct altcp_tls_config *tls_config = NULL;

static err_t tls_client_close(void *arg) {
	TLS_CLIENT_T *state = (TLS_CLIENT_T*)arg;
	err_t err = ERR_OK;

	state->complete = true;
	if (state->pcb != NULL) {
		altcp_arg(state->pcb, NULL);
		altcp_poll(state->pcb, NULL, 0);
		altcp_recv(state->pcb, NULL);
		altcp_err(state->pcb, NULL);
		err = altcp_close(state->pcb);
		if (err != ERR_OK) {
			printf("close failed %d, calling abort\n", err);
			altcp_abort(state->pcb);
			err = ERR_ABRT;
		}
		state->pcb = NULL;
	}
	// Restart core1
	if (state->core1) start_core1();
	// Put down the connection flag
	set_connection_flag(0);
	return err;
}

static err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err) {
	TLS_CLIENT_T *state = (TLS_CLIENT_T*)arg;
	if (err != ERR_OK) {
		printf("connect failed %d\n", err);
		return tls_client_close(state);
	}

	register_tcp_pcb(state->pcb);

	err_t e;
	e=send_header_if_exists();
	if (ERR_OK==e) {
		DEBUG_printf("Request header was sent\n%s\n");
	} else {
		DEBUG_printf("failed to send header\n");
	}

	set_connection_flag(1);
	return e;
}

static err_t tls_client_poll(void *arg, struct altcp_pcb *pcb) {
	printf("timed out");
	wifi_set_error(WIFI_ERROR_CONNECTION_ERROR);
	return tls_client_close(arg);
}

static void tls_client_err(void *arg, err_t err) {
	TLS_CLIENT_T *state = (TLS_CLIENT_T*)arg;
	printf("tls_client_err %d\n", err);
	state->pcb = NULL; /* pcb freed by lwip when _err function is called */
}

static err_t tls_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
	TLS_CLIENT_T *state = (TLS_CLIENT_T*)arg;
	if (!p) {
		printf("connection closed\n");
		wifi_set_error(WIFI_ERROR_CONNECTION_CLOSED);
		return tls_client_close(state);
	}

	if (p->tot_len > 0) {
		/* For simplicity this examples creates a buffer on stack the size of the data pending here, 
		   and copies all the data to it in one go.
		   Do be aware that the amount of data can potentially be a bit large (TLS record size can be 16 KB),
		   so you may want to use a smaller fixed size buffer and copy the data to it using a loop, if memory is a concern */
		/*char buf[p->tot_len + 1];

		pbuf_copy_partial(p, buf, p->tot_len, 0);
		buf[p->tot_len] = 0;

		printf("***\nnew data received from server:\n***\n\n%s\n", buf);//*/
		
		for (struct pbuf *q = p; q != NULL; q = q->next) {
			tcp_receive_in_buff(q->payload,q->len,state->pcb);
		}

		altcp_recved(pcb, p->tot_len);
	}
	pbuf_free(p);

	return ERR_OK;
}

static void tls_client_connect_to_server_ip(const ip_addr_t *ipaddr, TLS_CLIENT_T *state)
{
	err_t err;

	printf("connecting to server IP %s port %d\n", ipaddr_ntoa(ipaddr), state->port);
	err = altcp_connect(state->pcb, ipaddr, state->port, tls_client_connected);
	if (err != ERR_OK)
	{
		fprintf(stderr, "error initiating connect, err=%d\n", err);
		tls_client_close(state);
	}
}

static void tls_client_dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg)
{
	if (ipaddr)
	{
		printf("DNS resolving complete\n");
		tls_client_connect_to_server_ip(ipaddr, (TLS_CLIENT_T *) arg);
	}
	else
	{
		//printf("error resolving hostname %s\n", hostname);
		wifi_set_error(WIFI_ERROR_DNS_ERROR);
		tls_client_close(arg);
	}
}


static bool tls_client_open(const char *hostname, void *arg) {
	err_t err;
	ip_addr_t server_ip;
	TLS_CLIENT_T *state = (TLS_CLIENT_T*)arg;

	state->pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);
	if (!state->pcb) {
		printf("failed to create pcb\n");
		return false;
	}

	altcp_arg(state->pcb, state);
	altcp_poll(state->pcb, tls_client_poll, TLS_CLIENT_TIMEOUT_SECS * 2);
	altcp_recv(state->pcb, tls_client_recv);
	altcp_err(state->pcb, tls_client_err);

	/* Set SNI */
	mbedtls_ssl_set_hostname(altcp_tls_context(state->pcb), hostname);

	printf("resolving %s\n", hostname);

	// cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
	// You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
	// these calls are a no-op and can be omitted, but it is a good practice to use them in
	// case you switch the cyw43_arch type later.
	cyw43_arch_lwip_begin();

	err = dns_gethostbyname(hostname, &server_ip, tls_client_dns_found, state);
	if (err == ERR_OK)
	{
		/* host is in DNS cache */
		tls_client_connect_to_server_ip(&server_ip, state);
	}
	else if (err != ERR_INPROGRESS)
	{
		printf("error initiating DNS resolving, err=%d\n", err);
		tls_client_close(state->pcb);
		wifi_set_error(WIFI_ERROR_DNS_ERROR);
	}

	cyw43_arch_lwip_end();

	return err == ERR_OK || err == ERR_INPROGRESS;
}

// Perform initialisation
static TLS_CLIENT_T* tls_client_init(void) {
	static TLS_CLIENT_T s_state;
	TLS_CLIENT_T *state =memset(&s_state,0,sizeof(TLS_CLIENT_T));
	return state;
}

void start_tls_client(const char* servername, int tcp_port) {
	// Stop core1 first
	bool core1=is_core1_started();
	if (core1) stop_core1();
	// Initialize socket
	init_tls_socket();
	/* No CA certificate checking */
	if (!tls_config) tls_config = altcp_tls_create_config_client(NULL, 0);

	TLS_CLIENT_T *state = tls_client_init();

	state->port=tcp_port;
	state->core1=core1;
	if (!tls_client_open(servername, state)) {
		return;
	}
	// Resister state and closing function
	register_state(state);
	register_closing_function(tls_client_close);
	// No error
	wifi_set_error(ERR_OK);
}
