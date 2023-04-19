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
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "./wifi.h"
#include "../compiler.h"
#include "../api.h"

#define TCP_PORT 80
#define BUF_SIZE WIFI_BUFF_SIZE
#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

typedef struct TCP_SERVER_T_ {
	struct tcp_pcb *server_pcb;
	struct tcp_pcb *client_pcb;
	bool complete;
	int sent_len;
	int recv_len;
	int run_count;
} TCP_SERVER_T;

static TCP_SERVER_T* tcp_server_init(void) {
	//TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
	TCP_SERVER_T *state = calloc_memory((3+(sizeof(TCP_SERVER_T)))/4,get_permanent_block_number());
	if (!state) {
		DEBUG_printf("failed to allocate state\n");
		return NULL;
	}
	return state;
}

static err_t tcp_server_close(void *arg) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	err_t err = ERR_OK;
	if (state->client_pcb != NULL) {
		tcp_arg(state->client_pcb, NULL);
		tcp_poll(state->client_pcb, (err_t (*)(void *arg, struct tcp_pcb *tpcb))NULL_CALLBACK, 0);
		tcp_sent(state->client_pcb, (err_t (*)(void *arg, struct tcp_pcb *tpcb, u16_t len))NULL_CALLBACK);
		tcp_recv(state->client_pcb, (err_t(*)(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err))NULL_CALLBACK);
		tcp_err(state->client_pcb, (void (*)(void *arg, err_t err))NULL_CALLBACK);
		err = tcp_close(state->client_pcb);
		if (err != ERR_OK) {
			DEBUG_printf("close failed %d, calling abort\n", err);
			tcp_abort(state->client_pcb);
			err = ERR_ABRT;
		}
		tcp_abort(state->client_pcb);
		state->client_pcb = NULL;
	}
	if (state->server_pcb) {
		tcp_arg(state->server_pcb, NULL);
		tcp_close(state->server_pcb);
		state->server_pcb = NULL;
	}
	set_connection_flag(0);
	return err;
}

static err_t tcp_server_result(void *arg, int status) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (status == 0) {
		DEBUG_printf("test success\n");
	} else {
		//DEBUG_printf("test failed %d\n", status);
	}
	state->complete = true;
	return tcp_server_close(arg);
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	DEBUG_printf("tcp_server_sent %u\n", len);
	state->sent_len += len;

	if (state->sent_len >= BUF_SIZE) {

		// We should get the data back from the client
		state->recv_len = 0;
		DEBUG_printf("Waiting for buffer from client\n");
	}

	return ERR_OK;
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (!p) {
		DEBUG_printf("null pbuf (connection closed?)");
		set_connection_flag(0);
		return tcp_server_result(arg, -1);
	}
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	if (p->tot_len > 0) {
		DEBUG_printf("recv %d err %d\n", p->tot_len, err);
		for (struct pbuf *q = p; q != NULL; q = q->next) {
			//DUMP_BYTES(q->payload, q->len);
			tcp_receive_in_buff(q->payload,q->len);
		}
		tcp_recved(tpcb, p->tot_len);
	}
	pbuf_free(p);

	return ERR_OK;
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
	DEBUG_printf("tcp_server_poll_fn\n");
	return tcp_server_result(arg, -1); // no response is an error?
}

static void tcp_server_err(void *arg, err_t err) {
	if (err != ERR_ABRT) {
		DEBUG_printf("tcp_client_err_fn %d\n", err);
		tcp_server_result(arg, err);
	}
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (err != ERR_OK || client_pcb == NULL) {
		DEBUG_printf("Failure in accept\n");
		tcp_server_result(arg, err);
		return ERR_VAL;
	}
	DEBUG_printf("Client connected\n");

	state->client_pcb = client_pcb;
	tcp_arg(client_pcb, state);
	tcp_sent(client_pcb, tcp_server_sent);
	tcp_recv(client_pcb, tcp_server_recv);
	tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
	tcp_err(client_pcb, tcp_server_err);

	register_tcp_pcb(state->client_pcb);
	set_connection_flag(1);
	return ERR_OK;
}

static bool tcp_server_open(void *arg, int tcp_port) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), tcp_port);

	static struct tcp_pcb *pcb=0;
	if (!pcb) {
		pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	} else {
		tcp_close(pcb);
		tcp_abort(pcb);
		pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	}
	if (!pcb) {
		DEBUG_printf("failed to create pcb\n");
		return false;
	}

	err_t err = tcp_bind(pcb, NULL, tcp_port);
	if (err) {
		DEBUG_printf("failed to bind to port %u\n", tcp_port);
		return false;
	}

	state->server_pcb = tcp_listen_with_backlog(pcb, 1);
	if (!state->server_pcb) {
		DEBUG_printf("failed to listen\n");
		if (pcb) {
			tcp_close(pcb);
		}
		return false;
	}

	tcp_arg(state->server_pcb, state);
	tcp_accept(state->server_pcb, tcp_server_accept);

	return true;
}

void start_tcp_server(int tcp_port) {
	TCP_SERVER_T *state = tcp_server_init();
	if (!state) {
		wifi_set_error(__LINE__);
		return;
	}
	int i;
	for(i=0;i<10;i++){
		if (!tcp_server_open(state, tcp_port)) {
			tcp_server_result(state, -1);
			wifi_set_error(__LINE__);
			sleep_ms(300);
			continue;
		}
		break;
	}
	if (10<=i) return;
	// Resister state and closing function
	register_state(state);
	register_closing_function(tcp_server_close);
	// No error
	wifi_set_error(ERR_OK);
}
