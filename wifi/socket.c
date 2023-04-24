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
#include "../api.h"

/*
	The structure of buffer (int*) used in this file:
		buff[0]: pointer to next buffer
		buff[1]: length of following data in bytes
		buff[2]: read point in buffer in bytes
		buff[3]: void* tcp_pcb
		buff[4-]then main data (frexible length)
*/

static int* g_socket_buffer=0;
static struct tcp_pcb* g_pcb;
static void* g_state;
static char g_tls_mode;
static char g_connected;
static char g_connection_error;
static void* g_close_func;
static int* g_header_lines=0;
static void* g_pcb_fifo[10];
static void** g_connection_id=0;
static unsigned char g_connection_id_number=0;

void init_socket_system(void){
	// TODO: call this routine before running BASIC code
	// Reset connection id
	g_connection_id=0;
	g_connection_id_number=0;
}

void init_tcp_socket(void){
	int* next;
	int i;
	// Delete all allocated memories, first
	while(g_socket_buffer){
		next=(int*)g_socket_buffer[0];
		delete_memory(g_socket_buffer);
		g_socket_buffer=next;
	}
	// Disable TLS mode
	g_tls_mode=0;
	// Flags
	g_connected=0;
	g_connection_error=0;
	// Free memory
	if (g_state) delete_memory(g_state);
	// Reset some pointers
	g_state=0;
	g_pcb=0;
	g_close_func=0;
	// Reset FIFO buffer
	for(i=0;i<(sizeof g_pcb_fifo)/(sizeof g_pcb_fifo[0]);i++) g_pcb_fifo[i]=0;
}

void init_tls_socket(void){
	init_tcp_socket();
	g_tls_mode=1;
}

void* new_connection_id(void* tcp_pcb){
	if (!g_connection_id) {
		g_connection_id=(void**)alloc_memory(256,get_permanent_block_number());
	}
	g_connection_id[g_connection_id_number]=tcp_pcb;
	return &g_connection_id[g_connection_id_number++];
}

char add_pcb_to_fifo(void* tcp_pcb){
	int i;
	for(i=0;i<(sizeof g_pcb_fifo)/(sizeof g_pcb_fifo[0]);i++){
		if (g_pcb_fifo[i]) continue;
		g_pcb_fifo[i]=tcp_pcb;
		return 1;
	}
	return 0;
}

void* shift_pcb_fifo(void){
	int i;
	asm("cpsid i");
	void* ret=g_pcb_fifo[0];
	if (ret) {
		for(i=1;i<(sizeof g_pcb_fifo)/(sizeof g_pcb_fifo[0]);i++){
			g_pcb_fifo[i-1]=g_pcb_fifo[i];
		}
		g_pcb_fifo[(sizeof g_pcb_fifo)/(sizeof g_pcb_fifo[0])-1]=0;
	}
	asm("cpsie i");
	return ret;
}

char* tcp_receive_in_buff(char* data, int bytes, void* tcp_pcb){
	int* buff;
	int* new_buffer=alloc_memory(4+(3+bytes)/4,get_permanent_block_number());
	if (!new_buffer) return 0;
	if (g_socket_buffer) {
		// Add to the end of buffer arrays
		buff=g_socket_buffer;
		while(buff[0]) buff=(int*)buff[0];
		buff[0]=(int)new_buffer;
	} else {
		// The first buffer array
		g_socket_buffer=new_buffer;
	}
	new_buffer[0]=0;
	new_buffer[1]=bytes;
	new_buffer[2]=0;
	new_buffer[3]=(int)tcp_pcb;
	memcpy(&new_buffer[4],data,bytes);
	return (char*)&new_buffer[4];
}

int tcp_read_from_buffer(char* dest, int bytes, void** connection_id){
	// The third parameter is either connection_id or tcp_pcb (depend on which used by server or client mode)
	int* prev_socket_buffer;
	int* buff;
	int buffer_len,read_point;
	char* buffer_point;
	int valid_bytes=0;
	void* pcb=connection_id ? (void*)connection_id:g_pcb;
	while(0<bytes){
		if (!g_socket_buffer) break; // No buffer remaining
		// Check tcp_pcb
		prev_socket_buffer=0;
		buff=g_socket_buffer;
		while(buff[3]!=(int)pcb){
			prev_socket_buffer=buff;
			buff=(int*)buff[0];
			if (!buff) break;
		}
		if (!buff) break;
		// Buffer designated for tcp_pcb found 
		buffer_len=buff[1];
		read_point=buff[2];
		buffer_point=((char*)&buff[4])+read_point;
		if (bytes<=buffer_len-read_point) {
			// Data size is equal to or larger than buffer size
			memcpy(dest,buffer_point,bytes);
			valid_bytes+=bytes;
			read_point+=bytes;
			bytes=0;
		} else {
			// Data size is smaller than buffer size
			memcpy(dest,buffer_point,buffer_len-read_point);
			bytes-=buffer_len-read_point;
			dest+=buffer_len-read_point;
			valid_bytes+=buffer_len-read_point;
			read_point=buffer_len;
		}
		buff[2]=read_point;
		// Shift buffer
		if (buffer_len<=read_point) {
			// Stop interrupt before staring deletion process
			asm("cpsid i");
			if (prev_socket_buffer) { // Current buff is not the first one
				// Skip a buffer
				prev_socket_buffer[0]=buff[0];
				// Delete the current buffer
				delete_memory(buff);
			} else { // Current buff is the first one (the same as g_socket_buffer)
				// Delete the first buffer
				prev_socket_buffer=g_socket_buffer;
				// Reached at the end point of buffer
				g_socket_buffer=(int*)prev_socket_buffer[0];
				// Delete previous buffer
				delete_memory(prev_socket_buffer);
			}
			// Restart interrupt
			asm("cpsie i");
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

void connection_error(void){
	g_connection_error=1;
}

err_t send_header_if_exists(void){
	err_t err;
	if (!g_header_lines) return ERR_OK;
	err=machikania_tcp_write(&g_header_lines[1],g_header_lines[0],0);
	delete_memory(g_header_lines);
	g_header_lines=0;
	return err;
}

err_t machikania_tcp_write(const void* arg, u16_t len, void** connection_id){
	err_t err;
	int len_sent;
	void* pcb=connection_id ? connection_id[0]:g_pcb;
	if (!pcb) {
		if (g_state) {
			// TCPSEND is called after starting TCP connection, so this is an error
			return ERR_CONN;
		} else {
			// TCPSEND is called before starting TCP connection
			// This is for registering header lines that will be sent just after connection
			if (g_header_lines) delete_memory(g_header_lines);
			g_header_lines=alloc_memory(1+(3+len)/4,get_permanent_block_number());
			g_header_lines[0]=len;
			memcpy(&g_header_lines[1],arg,len);
			return ERR_OK;
		}
	}
	while (0<len) {
		// Maximim sending size is WIFI_BUFF_SIZE
		if (len<=WIFI_BUFF_SIZE) len_sent=len;
		else len_sent=WIFI_BUFF_SIZE;
		// Wait until buffer is clear, then send
		if (g_tls_mode) {
			while(ERR_OK!=altcp_output(pcb)) sleep_ms(1);
			err=altcp_write(pcb,arg,len_sent,TCP_WRITE_FLAG_COPY);
		} else { 
			while(ERR_OK!=tcp_output(pcb)) sleep_ms(1);
			err=tcp_write(pcb,arg,len_sent,TCP_WRITE_FLAG_COPY);
		}
		if (ERR_OK!=err) return err;
		len-=len_sent;
	}
	return ERR_OK;
}

err_t machikania_tcp_close(void** connection_id){
	err_t e;
	// Execute registered closing function
	err_t (*f)(void* state)=g_close_func;
	if (connection_id) {
		// Close connection to client as server
		e=tcp_server_client_close(connection_id);
	} else {
		// Close connection to server
		if (g_close_func) e=f(g_state);
		// Initialize socket
		init_tcp_socket();
		// Delete string buffer to be sent as header
		if (g_header_lines) delete_memory(g_header_lines);
		g_header_lines=0;
	}
	return e;
}

int machikania_tcp_status(int mode, void** connection_id){
	int i;
	int* buff;
	if (connection_id) {
		// Server mode
		switch(mode){
			case 0: // Connected (1 or 0)
			default:
				return connection_id[0] ? 1:0;
			case 1: // Received total bytes numnber
				i=0;
				buff=g_socket_buffer;
				while(buff){
					if (buff[3]!=(int)connection_id) continue;
					i+=buff[1]-buff[2];
					buff=(int*)buff[0];
				}
				return i;
			case 2: // Sending data remaining (non-zero or 0)
				return connection_id[0] ? tcp_output(connection_id[0]) : 0;
			case 3: // Which connection error occured or not
				return g_connection_error;
		}
	} else {
		// Client mode
		switch(mode){
			case 0: // Connected (1 or 0)
			default:
				return g_connected;
			case 1: // Received total bytes numnber
				i=0;
				buff=g_socket_buffer;
				while(buff){
					i+=buff[1]-buff[2];
					buff=(int*)buff[0];
				}
				return i;
			case 2: // Sending data remaining (non-zero or 0)
				if (g_tls_mode) return altcp_output(g_pcb);
				else return tcp_output(g_pcb);
			case 3: // Which connection error occured or not
				return g_connection_error;
		}
	}
}
