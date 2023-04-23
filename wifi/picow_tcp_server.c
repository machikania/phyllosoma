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

#define DEBUG_printf printf
#define BUF_SIZE 2048
#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

static char g_response[]=
	"HTTP/1.0 200 OK" "\r\n"
	"Content-Type: text/html; charset=Shift_JIS" "\r\n"
	"Content-Length: 95" "\r\n"
	"" "\r\n"
	"<html>"
	"<head><title>TCP server test</title><meta http-equiv=\"refresh\" content=\"1;/\"></head>"
	"<body>Hello TCP server world(0)!</body>"
	"</html>"
;
static char g_response_num=0x31;

//static uint8_t buffer_recv[BUF_SIZE];
static volatile char g_server_complete;
static struct tcp_pcb* g_server_pcb;

static err_t tcp_server_client_close(struct tcp_pcb *client_pcb){
	err_t err = ERR_OK;
	if (client_pcb != NULL) {
		tcp_arg(client_pcb, NULL);
		tcp_poll(client_pcb, NULL, 0);
		tcp_sent(client_pcb, NULL);
		tcp_recv(client_pcb, NULL);
		tcp_err(client_pcb, NULL);
		err = tcp_close(client_pcb);
	}
	return err;
}

static err_t tcp_server_close(struct tcp_pcb *client_pcb) {
	err_t err;
	if (g_server_pcb) {
		tcp_arg(g_server_pcb, NULL);
		tcp_close(g_server_pcb);
		g_server_pcb = NULL;
	}
	if (client_pcb) {
		err=tcp_server_client_close(client_pcb);
	}
	return err;
}

static err_t tcp_server_result(void *arg, int status) {
	if (status == 0) {
		DEBUG_printf("test success\n");
	} else {
		DEBUG_printf("test failed %d\n", status);
	}
	g_server_complete = true;
	return tcp_server_close(arg);
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	DEBUG_printf("tcp_server_sent %u\n", len);
	return ERR_OK;
}

err_t tcp_server_send_data(struct tcp_pcb *tpcb)
{
	g_response[(sizeof g_response)-1-17]=g_response_num++;
	DEBUG_printf("Writing %ld bytes to client\n", (sizeof g_response)-1);
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	err_t err = tcp_write(tpcb, g_response, (sizeof g_response)-1, TCP_WRITE_FLAG_COPY);
	if (err != ERR_OK) {
		DEBUG_printf("Failed to write data %d\n", err);
		return tcp_server_result(tpcb, -1);
	}
	return ERR_OK;
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	int i;
	if (!p) {
		return tcp_server_client_close(arg);
	}
	// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
	// can use this method to cause an assertion in debug mode, if this method is called when
	// cyw43_arch_lwip_begin IS needed
	cyw43_arch_lwip_check();
	if (p->tot_len > 0) {
		DEBUG_printf("tcp_server_recv %d err %d\n", p->tot_len, err);

		// Receive the data here if needed
		//i=pbuf_copy_partial(p, buffer_recv, p->tot_len > BUF_SIZE ? BUF_SIZE : p->tot_len, 0);
		tcp_recved(tpcb, p->tot_len);
		DEBUG_printf("Received in buffer %d\n",i);
	}
	pbuf_free(p);

	return tcp_server_send_data(arg);
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
	DEBUG_printf("tcp_server_poll_fn\n");
	return tcp_server_client_close(arg);
}

static void tcp_server_err(void *arg, err_t err) {
	if (err != ERR_ABRT) {
		DEBUG_printf("tcp_client_err_fn %d\n", err);
		tcp_server_result(arg, err);
	}
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
	if (err != ERR_OK || client_pcb == NULL) {
		DEBUG_printf("Failure in accept\n",err);
		if (client_pcb) tcp_close(client_pcb);
		return ERR_VAL;
	}
	DEBUG_printf("Client connected\n");
	
	tcp_arg(client_pcb, client_pcb);
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

void start_tcp_server(int tcp_port) {
	g_server_complete=false;
	if (!tcp_server_open(tcp_port)) {
		tcp_server_result(0, -1);
		return;
	}
	while(!g_server_complete) {
		sleep_ms(1000);
	}
}
