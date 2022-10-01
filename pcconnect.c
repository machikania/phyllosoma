/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "./compiler.h"
#include "./api.h"

#define PC_CONNECT_BUFFER_SIZE 256
static int g_connect_file_size;
static char g_connect_buffer[PC_CONNECT_BUFFER_SIZE];

void communication_error(void){
	printf("\nAn error occured in MachiKania during the commnucation\n");
	printf("Restart the application and reset MachiKania\n");
	while(1) sleep_ms(1000);
}

char* receive_command(void){
	// Note that USB-serial connection doesn't cause error (1 M bytes communication tested)
	static char command[17]="command command ";
	int i;
	unsigned int c;
	for(i=0;i<16;i++) {
		//while((c=getchar_timeout_us(1000))==PICO_ERROR_TIMEOUT);
		c=getchar_timeout_us(2000000);
		if (PICO_ERROR_TIMEOUT==c) {
			printf("\nCommand request timeout");
			communication_error();
		}
		command[i]=c;
	}
	return command;
}

void pc_connected(void){
	char* command;
	unsigned int c;
	int i;
	FIL fh;
	printf("SENDCMDS\x08\x08\x08\x08\x08\x08\x08\x08");
	while(1){
		command=receive_command();
		if (!strncmp(command,"DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08",16)) {
			// All done
			f_chdir("/");
			printf("ALL DONE\x08\x08\x08\x08\x08\x08\x08\x08");
			return;
		} else if (!strncmp(command,"CD:",3)) {
			// Change directory
			printf("chdir: %s\n",command+3);
			while(1){
				if (FR_OK==f_chdir(command+3)) break;
				if (FR_OK==f_mkdir(command+3)) {
					if (FR_OK==f_chdir(command+3)) break;
				}
				printf("\nchdir failed");
				communication_error();
			}
			printf("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
		} else if (!strncmp(command,"SIZE:",5)) {
			g_connect_file_size=strtol(command+5,NULL,10);
			printf("size: %d, ",g_connect_file_size);
			printf("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
		} else if (!strncmp(command,"CP:",3)) {
			printf("copy: %s\n",command+3);
			if (FR_OK!=f_open(&fh,command+3,FA_WRITE|FA_CREATE_ALWAYS)) {
				printf("\nFile cannot be open");
				communication_error();
			}
			printf("SENDFILE\x08\x08\x08\x08\x08\x08\x08\x08");
			for(i=0;i<g_connect_file_size;i++) {
				//while((c=getchar_timeout_us(100000))==PICO_ERROR_TIMEOUT);
				c=getchar_timeout_us(2000000);
				if (PICO_ERROR_TIMEOUT==c) {
					printf("\nCommand request timeout");
					communication_error();
				}
				g_connect_buffer[(PC_CONNECT_BUFFER_SIZE-1)&i]=c;
				if ((PC_CONNECT_BUFFER_SIZE-1)==((PC_CONNECT_BUFFER_SIZE-1)&i)) {
					printf("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
					if (FR_OK!=f_write(&fh,g_connect_buffer,PC_CONNECT_BUFFER_SIZE,NULL)) {
						printf("\fCannot write to file");
						communication_error();
					}
				} else if (g_connect_file_size-1==i) {
					if (FR_OK!=f_write(&fh,g_connect_buffer,(PC_CONNECT_BUFFER_SIZE-1)&i+1,NULL)) {
						printf("\fCannot write to file");
						communication_error();
					}
				}
			}
			f_close(&fh);
			printf("DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08");
		} else {
			printf("\n%s ???\n",command);
			communication_error();
		}
	}
}

void connect2pc(void){
	int i;
	sleep_ms(100);
	printf("MACHIKAP\x08\x08\x08\x08\x08\x08\x08\x08");
	sleep_ms(100);
	for(i=0;i<16;i++){
		if ("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"[i]!=getchar_timeout_us(1000)) break;
		if (15==i) pc_connected();
	}
}
