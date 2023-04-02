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

static const char g_request[]=
	"GET / HTTP/1.0" "\r\n"
	"Connection: Close" "\r\n"
	"Accept: */*" "\r\n"
	"Host: abehiroshi.la.coocan.jp" "\r\n"
	"" "\r\n"
;

#define TCP_PORT 80
#define BUF_SIZE 2048

#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

#if 1
static void dump_bytes(const uint8_t *bptr, uint32_t len) {
	unsigned int i = 0;

	printf("dump_bytes %d", len);
	for (i = 0; i < len;) {
		if ((i & 0x0f) == 0) {
			//printf("\n");
		} else if ((i & 0x07) == 0) {
			//printf(" ");
		}
		//printf("%02x ", bptr[i++]);
		//printf("%c", bptr[i++]);
		printchar(bptr[i++]);
	}
	//printf("\n");
	printstr("\n");
}
#define DUMP_BYTES dump_bytes
#else
#define DUMP_BYTES(A,B)
#endif

typedef struct TCP_CLIENT_T_ {
	struct tcp_pcb *tcp_pcb;
	ip_addr_t remote_addr;
	uint8_t buffer[BUF_SIZE];
	int buffer_len;
	int sent_len;
	bool complete;
	int run_count;
	bool connected;
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
	return err;
}

// Called with results of operation
static err_t tcp_result(void *arg, int status) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (status == 0) {
		DEBUG_printf("test success\n");
	} else {
		DEBUG_printf("test failed %d\n", status);
	}
	state->complete = true;
	return tcp_client_close(arg);
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	DEBUG_printf("tcp_client_sent %u\n", len);
	state->sent_len += len;

	if (state->sent_len >= BUF_SIZE) {

		state->run_count++;
		if (state->run_count >= TEST_ITERATIONS) {
			tcp_result(arg, 0);
			return ERR_OK;
		}

		// We should receive a new buffer from the server
		state->buffer_len = 0;
		state->sent_len = 0;
		DEBUG_printf("Waiting for buffer from server\n");
	}

	return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (err != ERR_OK) {
		printf("connect failed %d\n", err);
		return tcp_result(arg, err);
	}
	state->connected = true;
	DEBUG_printf("Waiting for buffer from server\n");

	err_t e;
	e=tcp_write(state->tcp_pcb, g_request, sizeof g_request-1,0);
	if (ERR_OK==e) {
		DEBUG_printf("Request was sent\n%s\n",g_request);
	} else {
		DEBUG_printf("failed to write request, err=%d\n",e);
	}
 
	return ERR_OK;
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
	DEBUG_printf("tcp_client_poll\n");
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
		return tcp_result(arg, -1);
	}
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	if (p->tot_len > 0) {
		DEBUG_printf("recv %d err %d\n", p->tot_len, err);
		for (struct pbuf *q = p; q != NULL; q = q->next) {
			DUMP_BYTES(q->payload, q->len);
		}
		// Receive the buffer
		const uint16_t buffer_left = BUF_SIZE - state->buffer_len;
		state->buffer_len += pbuf_copy_partial(p, state->buffer + state->buffer_len,
											   p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
		tcp_recved(tpcb, p->tot_len);
	}
	pbuf_free(p);

	// If we have received the whole buffer, send it back to the server
	if (state->buffer_len == BUF_SIZE) {
		DEBUG_printf("Writing %d bytes to server\n", state->buffer_len);
		err_t err = tcp_write(tpcb, state->buffer, state->buffer_len, TCP_WRITE_FLAG_COPY);
		if (err != ERR_OK) {
			DEBUG_printf("Failed to write data %d\n", err);
			return tcp_result(arg, -1);
		}
	}
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

	state->buffer_len = 0;

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
	//TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T)); 
	TCP_CLIENT_T *state = calloc_memory((3+(sizeof(TCP_CLIENT_T)))/4,get_permanent_block_number());
	if (!state) {
		DEBUG_printf("failed to allocate state\n");
		return NULL;
	}
	ip4addr_aton(ipaddr, &state->remote_addr);
	return state;
}

void run_tcp_client_test(const char* ipaddr, int tcp_port) {
	TCP_CLIENT_T *state = tcp_client_init(ipaddr);
	if (!state) {
		return;
	}
	if (!tcp_client_open(state, tcp_port)) {
		tcp_result(state, -1);
		return;
	}
	while(!state->complete) {
		// the following #ifdef is only here so this same example can be used in multiple modes;
		// you do not need it in your code
#if PICO_CYW43_ARCH_POLL
		// if you are using pico_cyw43_arch_poll, then you must poll periodically from your
		// main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
		cyw43_arch_poll();
		// you can poll as often as you like, however if you have nothing else to do you can
		// choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
		cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
		// if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
		// is done via interrupt in the background. This sleep is just an example of some (blocking)
		// work you might be doing.
		sleep_ms(1000);
#endif
	}
	//free(state);
	delete_memory(state);
}
