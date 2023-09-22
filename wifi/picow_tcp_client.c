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

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "./wifi.h"
#include "../compiler.h"
#include "../api.h"

#define POLL_TIME_S 5

typedef struct TCP_CLIENT_T_ {
	struct tcp_pcb *tcp_pcb;
	ip_addr_t remote_addr;
	int sent_len;
} TCP_CLIENT_T;

static err_t tcp_client_close(void *arg) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	err_t err = ERR_OK;
	if (state->tcp_pcb != NULL) {
		tcp_arg(state->tcp_pcb, NULL);
		tcp_poll(state->tcp_pcb, NULL, 0);
		tcp_sent(state->tcp_pcb, NULL);
		tcp_recv(state->tcp_pcb, NULL);
		tcp_err(state->tcp_pcb, NULL);
		err = tcp_close(state->tcp_pcb);
		if (err != ERR_OK) {
			DEBUG_printf("close failed %d, calling abort\n", err);
			tcp_abort(state->tcp_pcb);
			err = ERR_ABRT;
		}
		state->tcp_pcb = NULL;
	}
	set_connection_flag(0);
	return err;
}

// Called with results of operation
static err_t tcp_result(void *arg, int status) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (status == 0) {
		DEBUG_printf("test success\n");
	} else {
		//DEBUG_printf("test failed %d\n", status);
	}
	return tcp_client_close(arg);
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	DEBUG_printf("tcp_client_sent %u\n", len);
	state->sent_len += len;
	sent_bytes(len);
	return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (err != ERR_OK) {
		printf("connect failed %d\n", err);
		return tcp_result(arg, err);
	}
	DEBUG_printf("Waiting for buffer from server\n");
	register_tcp_pcb(state->tcp_pcb);

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

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
	DEBUG_printf("Connection time out\n");
	wifi_set_error(WIFI_ERROR_CONNECTION_ERROR);
	return tcp_result(arg, -1); // no response is an error?
}

static void tcp_client_err(void *arg, err_t err) {
	if (err != ERR_ABRT) {
		DEBUG_printf("tcp_client_err %d\n", err);
		tcp_result(arg, err);
	}
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (!p) {
		DEBUG_printf("null pbuf (connection closed?)");
		wifi_set_error(WIFI_ERROR_CONNECTION_CLOSED);
		return tcp_result(arg, -1);
	}
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	if (p->tot_len > 0) {
		DEBUG_printf("recv %d err %d\n", p->tot_len, err);
		for (struct pbuf *q = p; q != NULL; q = q->next) {
			tcp_receive_in_buff(q->payload,q->len,state->tcp_pcb);
		}
		tcp_recved(tpcb, p->tot_len);
	}
	pbuf_free(p);

	return ERR_OK;
}

static bool tcp_client_open(void *arg, int tcp_port) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	DEBUG_printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), tcp_port);
	state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
	if (!state->tcp_pcb) {
		DEBUG_printf("failed to create pcb\n");
		return false;
	}

	tcp_arg(state->tcp_pcb, state);
	tcp_poll(state->tcp_pcb, tcp_client_poll, POLL_TIME_S * 2);
	tcp_sent(state->tcp_pcb, tcp_client_sent);
	tcp_recv(state->tcp_pcb, tcp_client_recv);
	tcp_err(state->tcp_pcb, tcp_client_err);

	// cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
	// You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
	// these calls are a no-op and can be omitted, but it is a good practice to use them in
	// case you switch the cyw43_arch type later.
	cyw43_arch_lwip_begin();
	err_t err = tcp_connect(state->tcp_pcb, &state->remote_addr, tcp_port, tcp_client_connected);
	cyw43_arch_lwip_end();

	return err == ERR_OK;
}

// Perform initialisation
static TCP_CLIENT_T* tcp_client_init(const char* ipaddr) {
	static TCP_CLIENT_T s_state;
	TCP_CLIENT_T *state =memset(&s_state,0,sizeof(TCP_CLIENT_T));
	ip4addr_aton(ipaddr, &state->remote_addr);
	return state;
}

void start_tcp_client(const char* ipaddr, int tcp_port) {
	// Initialize socket
	init_tcp_socket();
	// Create state structure
	TCP_CLIENT_T *state = tcp_client_init(ipaddr);
	if (!state) {
		wifi_set_error(__LINE__);
		return;
	}
	// Open the client socket
	if (!tcp_client_open(state, tcp_port)) {
		delete_memory(state);
		tcp_result(state, -1);
		wifi_set_error(__LINE__);
		return;
	}
	// Resister state and closing function
	register_state(state);
	register_closing_function(tcp_client_close);
	// No error
	wifi_set_error(ERR_OK);
}
