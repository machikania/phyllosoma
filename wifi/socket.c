/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/altcp.h"
#include "./wifi.h"
#include "../compiler.h"

/*
	The structure of buffer used in this file:
		first 4 bytes (32 bits): pointer to next buffer,
		next 4 bytes (32 bits): length of following data in bytes,
		then main data (frexible length)
*/

static int* g_socket_buffer=0;
static int g_read_point_in_buffer=0;
static struct tcp_pcb* g_pcb;
static void* g_state;
static char g_tls_mode;
static char g_connected;
static void* g_close_func;

void init_tcp_socket(void){
	int* next;
	// Delete all allocated memories, first
	while(g_socket_buffer){
		next=(int*)g_socket_buffer[0];
		delete_memory(g_socket_buffer);
		g_socket_buffer=next;
	}
	// Set the read point to zero
	g_read_point_in_buffer=0;
	// Disable TLS mode
	g_tls_mode=0;
	// Flags
	g_connected=0;
	// Free memory
	if (g_state) delete_memory(g_state);
	// Reset some pointers
	g_state=0;
	g_pcb=0;
	g_close_func=0;
}

void init_tls_socket(void){
	init_tcp_socket();
	g_tls_mode=1;
}

char* tcp_receive_in_buff(char* data, int bytes){
	int* new_buffer=alloc_memory(2+(3+bytes)/4,get_permanent_block_number());
	if (!new_buffer) return 0;
	if (g_socket_buffer) g_socket_buffer[0]=(int)new_buffer;
	g_socket_buffer=new_buffer;
	g_socket_buffer[0]=0;
	g_socket_buffer[1]=bytes;
	memcpy(&g_socket_buffer[2],data,bytes);
	return (char*)&g_socket_buffer[2];
}

int tcp_read_from_buffer(char* dest, int bytes){
	int* prev_socket_buffer;
	int buffer_len;
	char* buffer_point;
	int valid_bytes=0;
	while(0<bytes){
		if (!g_socket_buffer) break; // No buffer remaining
		buffer_len=g_socket_buffer[1];
		buffer_point=((char*)&g_socket_buffer[2])+g_read_point_in_buffer;
		if (bytes<=buffer_len-g_read_point_in_buffer) {
			// Current buffer contains all the data
			memcpy(dest,buffer_point,bytes);
			valid_bytes+=bytes;
			g_read_point_in_buffer+=bytes;
			bytes=0;
		} else {
			// Current buffer contains part of data needed
			memcpy(dest,buffer_point,buffer_len-g_read_point_in_buffer);
			bytes-=buffer_len-g_read_point_in_buffer;
			dest+=buffer_len-g_read_point_in_buffer;
			g_read_point_in_buffer=buffer_len;
		}
		// Shift buffer
		if (buffer_len<=g_read_point_in_buffer) {
			prev_socket_buffer=g_socket_buffer;
			// Reached at the end point of buffer
			g_read_point_in_buffer=0;
			g_socket_buffer=(int*)g_socket_buffer[0];
			// Delete previous buffer
			delete_memory(prev_socket_buffer);
		}
	}
	return valid_bytes;
}

void register_state(void* state){
	g_state=state;
}

void register_tcp_pcb(void* pcb){
	g_pcb=pcb;
}

void register_closing_function(void* func){
	g_close_func=func;
}

void set_connection_flag(int flag){
	g_connected=flag ? 1:0;
}

err_t machikania_tcp_write(const void* arg, u16_t len){
	err_t err;
	int len_sent;
	if (!g_pcb) return ERR_CONN;
	while (0<len) {
		// Maximim sending size is WIFI_BUFF_SIZE
		if (len<=WIFI_BUFF_SIZE) len_sent=len;
		else len=WIFI_BUFF_SIZE;
		// Wait until buffer is clear, then send
		if (g_tls_mode) {
			while(ERR_OK!=altcp_output(g_pcb)) sleep_ms(1);
			err=altcp_write(g_pcb,arg,len,TCP_WRITE_FLAG_COPY);
		} else { 
			while(ERR_OK!=tcp_output(g_pcb)) sleep_ms(1);
			err=tcp_write(g_pcb,arg,len,TCP_WRITE_FLAG_COPY);
		}
		if (ERR_OK!=err) return err;
		len-=len_sent;
	}
	return ERR_OK;
}

err_t machikania_tcp_close(void){
	err_t e;
	err_t (*f)(void* state)=g_close_func;
	e=f(g_state);
	init_tcp_socket();
	return e;
}

int machikania_tcp_status(int mode){
	int i;
	switch(mode){
		case 0: // Connected (1 or 0)
		default:
			return g_connected;
		case 1: // Received bytes numnber
			i=0;
			while(g_socket_buffer){
				i+=g_socket_buffer[1]-g_read_point_in_buffer;
				g_socket_buffer=(int*)g_socket_buffer[0];
			}
			return i;
		case 2: // Sending data remaining (non-zero or 0)
			if (g_tls_mode) return altcp_output(g_pcb);
			else return altcp_output(g_pcb);
	}
}
