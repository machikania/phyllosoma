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

#define POLL_TIME_S 5

static struct tcp_pcb* g_server_pcb;
static char g_tcp_accept_mode;

err_t tcp_server_client_close(void* arg){
	struct tcp_pcb *client_pcb;
	struct tcp_pcb ** connection_id=(struct tcp_pcb **)arg;
	err_t err = ERR_OK;
	if (!connection_id) return ERR_OK;

	client_pcb=(struct tcp_pcb *)connection_id[0];
	if (client_pcb != NULL) {
		tcp_arg(client_pcb, NULL);
		tcp_poll(client_pcb, NULL, 0);
		tcp_sent(client_pcb, NULL);
		tcp_recv(client_pcb, NULL);
		tcp_err(client_pcb, NULL);
		err = tcp_close(client_pcb);
		connection_id[0]=0;
	}
	return err;
}

static err_t tcp_server_close(void *arg) {
	// Shut down the server
	if (g_server_pcb) {
		tcp_arg(g_server_pcb, NULL);
		tcp_close(g_server_pcb);
		tcp_abort(g_server_pcb);
		g_server_pcb = NULL;
	}
	return ERR_OK;
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	DEBUG_printf("tcp_server_sent %u\n", len);
	sent_bytes(len);
	return ERR_OK;
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	if (!p) {
		// Closed by client?
		return tcp_server_client_close(arg);
	}
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	if (p->tot_len > 0) {
		DEBUG_printf("tcp_server_recv %d err %d\n", p->tot_len, err);
		for (struct pbuf *q = p; q != NULL; q = q->next) {
			tcp_receive_in_buff(q->payload,q->len,arg);
		}
		tcp_recved(tpcb, p->tot_len);
		DEBUG_printf("Received in buffer %d\n",p->tot_len);
	}
	pbuf_free(p);

	// Register connection_id in FIFO buffer if mode 1
	if (1==g_tcp_accept_mode) {
		if (!add_pcb_to_fifo(arg)) {
			// FIFO buffer is full
			tcp_close(arg);
			return ERR_ABRT;
		}
	}

	return ERR_OK;
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
	DEBUG_printf("tcp_server_poll_fn\n");
	return tcp_server_client_close(arg);
}

static void tcp_server_err(void *arg, err_t err) {
	if (err != ERR_ABRT) {
		DEBUG_printf("tcp_client_err_fn %d\n", err);
		tcp_server_client_close(arg);
	}
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
	if (err != ERR_OK || client_pcb == NULL) {
		DEBUG_printf("Failure in accept\n",err);
		if (client_pcb) tcp_close(client_pcb);
		return ERR_VAL;
	}
	DEBUG_printf("Client connected\n");
	// Assign new connection ID with tcp_pcb
	void* connection_id=new_connection_id(client_pcb);
	
	// Register connection_id in FIFO buffer if mode 0
	if (0==g_tcp_accept_mode) {
		if (!add_pcb_to_fifo(connection_id)) {
			// FIFO buffer is full
			tcp_close(client_pcb);
			return ERR_ABRT;
		}
	}

	tcp_arg(client_pcb, connection_id);
	tcp_sent(client_pcb, tcp_server_sent);
	tcp_recv(client_pcb, tcp_server_recv);
	tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
	tcp_err(client_pcb, tcp_server_err);
	return ERR_OK;
}

static bool tcp_server_open(int tcp_port) {
	DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), tcp_port);

	struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		DEBUG_printf("failed to create pcb\n");
		return false;
	}

	err_t err = tcp_bind(pcb, NULL, tcp_port);
	if (err) {
		DEBUG_printf("failed to bind to port %u\n", tcp_port);
		return false;
	}

	g_server_pcb = tcp_listen_with_backlog(pcb, 1);
	if (!g_server_pcb) {
		DEBUG_printf("failed to listen\n");
		if (pcb) {
			tcp_close(pcb);
		}
		return false;
	}

	void* arg=g_server_pcb;
	tcp_arg(g_server_pcb, arg);
	tcp_accept(g_server_pcb, tcp_server_accept);

	return true;
}

void start_tcp_server(int tcp_port, int tcp_accept_mode) {
	if (!tcp_server_open(tcp_port)) return;
	// Resister state and closing function
	register_state(g_server_pcb);
	register_closing_function(tcp_server_close);
	// Set tcp_accept mode
	// 0: TCPACCEPT() returns connection-accepted IDs
	// 1: TCPACCEPT() returns data-received IDs
	g_tcp_accept_mode=tcp_accept_mode;
	// No error
	wifi_set_error(ERR_OK);
}
