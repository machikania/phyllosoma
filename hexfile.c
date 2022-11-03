/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "./compiler.h"
#include "./api.h"

extern FATFS g_FatFs; // file.c

/*
 Intel HEX format example
 :040bf000ffffffcf35
    +--------------------- Byte count
    |    +---------------- Address
    |    |  +------------- Record type (00:Data, 01:EOF, 04: Extended linear addres, 05: Start Linear Address)
    |    |  |        +---- Data
    |    |  |        |  +- Checksum
    |    |  |        |  |
 : 04 0bf0 00 ffffffcf 35
 : 02 0000 04 2000     DA
 : 04 0000 05 200001E9 ED
*/

#define HEX_DATA 0x00
#define HEX_EOF 0x01
#define HEX_EXTENDED_ADDRESS 0x04
#define HEX_START_ADDRESS 0x05

struct hexline {
	unsigned char bytes;
	unsigned short laddr;
	unsigned char type;
	char data[255];
	unsigned char checksum;
};

#define get4(a) do{\
		a<<=4;\
		if ('0'<=line[0] && line[0]<='9') a|=line[0]-'0';\
		else if ('a'<=line[0] && line[0]<='f') a|=line[0]-'a'+10;\
		else if ('A'<=line[0] && line[0]<='F') a|=line[0]-'A'+10;\
		else return 0;\
		line++;\
	} while(0)
	
struct hexline* getHexLine(char* line){
	static struct hexline res;
	int i;
	unsigned char checksum;
	memset(&res, 0, sizeof(res));
	get4(res.bytes);
	get4(res.bytes);
	get4(res.laddr);
	get4(res.laddr);
	get4(res.laddr);
	get4(res.laddr);
	get4(res.type);
	get4(res.type);
	checksum=res.bytes+(res.laddr&255)+(res.laddr>>8)+res.type;
	for(i=0;i<res.bytes;i++) {
		get4(res.data[i]);
		get4(res.data[i]);
		checksum+=res.data[i];
	}
	get4(res.checksum);
	get4(res.checksum);
	checksum+=res.checksum;
	if (checksum) return 0;
	if (0x00!=line[0] && 0x0d!=line[0] && 0x0a!=line[0]) return 0;
	return &res;
}

char* runHexMain(char* fname){
	int i;
	// File handle
	FIL fpo;
	FIL* fp=&fpo;
	// HEX file data
	char* addr;
	unsigned int start_address;
	struct hexline* hexdata;
	// Create file buffer at the end of kmbasic_object
	const int bsize=128;
	char* const fbuff=(char*)&kmbasic_object[((sizeof kmbasic_object)-bsize)/(sizeof kmbasic_object[0])];
	// Open file
	if (f_open(fp,fname,FA_READ)) {
		// Mount and open again
		if (f_mount(&g_FatFs, "", 0)) return "mount error";
		if (f_open(fp,fname,FA_READ)) return "file open error";
	}
	// Read file
	while(!f_eof(fp)){
		if (!f_gets(fbuff,bsize,fp)) break;
		// Get info
		if (':'==fbuff[0]) hexdata=getHexLine(&fbuff[1]);
		if (!hexdata) return "HEX format error";
		if (HEX_EOF==hexdata->type) break;
		switch(hexdata->type){
			case HEX_EXTENDED_ADDRESS:
				i=hexdata->data[0]<<8;
				i+=hexdata->data[1];
				addr=(char*)(i<<16);
				break;
			case HEX_DATA:
				if (&addr[hexdata->laddr]<(char*)&kmbasic_object[0] || 
					fbuff<=&addr[hexdata->laddr+hexdata->bytes]) return "HEX region doesn't fit";
				for(i=0;i<hexdata->bytes;i++) addr[hexdata->laddr+i]=hexdata->data[i];
				break;
			case HEX_START_ADDRESS:
				start_address=0;
				for(i=0;i<hexdata->bytes;i++) {
					start_address<<=8;
					start_address|=hexdata->data[i];
				}
				break;
			default:
				return "HEX type error";
		}
	}
	f_close(fp);
	printstr("OK");
	watchdog_reboot(start_address,0x20040000,1000);
	sleep_ms(2000);
}

void runHex(char* fname){
	char* e;
	printstr("Loading ");
	printstr(fname);
	printstr(" ... ");
	e=runHexMain(fname);
	printstr(e);
	while(1) sleep_ms(1000);
}
