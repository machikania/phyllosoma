/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	This file provide functions for handling data used when compiling.
	The data is inserted between g_objmax and file cache.
	Data format (32 bit):
		MSB                LSB
		 +----+----+--------+
		 |type|len | data16 |
		 +----+----+--------+
		where,
			type:   data type number (unsigned char)
			len:    length of data area in number of words (unsigned char)
			        If additional area isn't used, set 1
			data16: general 16 bit data (short)
*/

/*
	CMPDATA_TEMP structure
		type:      CMPDATA_TEMP
		len:       n+1
		data16:    id
		record[1]: any data
		record[2]: any data
		...
		record[n]: any data
*/

static int* g_cmpdata;
static int* g_cmpdata_end;
static int* g_cmpdata_point;
static unsigned short g_cmpdata_id;

/*
	Initialize routine must be called when starting compiler.
*/
void cmpdata_init(void){
	g_cmpdata=(int*)g_objmax;
	g_cmpdata_end=(int*)g_objmax;
	g_cmpdata_point=(int*)g_objmax;
	g_cmpdata_id=1;
}

/*
	Returns ID as 16 bit indivisual number
	Error shouldn't happen as RAM size is less than 256K and max number of CMPDATA is less than 64K
	So, error handling isn't required.
*/
unsigned short cmpdata_get_id(void){
	if ((++g_cmpdata_id)==0) printstr("CMPDATA: no more ID!\n");
	return g_cmpdata_id;
}

/*
	Reset data point. Next search will be from the beginning.
*/

void cmpdata_reset(void){
	g_cmpdata_point=g_cmpdata;
}

/*
	Function to insert a data. The data must be defined by a pointer to int array.
		unsigned char type: Data type number (0-255)
		short data16:       16 bit data. If not required, set 0.
		int* data:          Pointer to data array. If not requird, set 0.
		unsigned char num:  Length of above data array. If not required, set 0.
*/
int cmpdata_insert(unsigned char type, short data16, int* data, unsigned char num){
	unsigned char i;
	g_cmpdata-=num+1;
	g_objmax=(unsigned short*)&g_cmpdata[0];
	if (g_objmax<object) return ERROR_OBJ_TOO_LARGE;
	g_cmpdata[0]=(type<<24)|(num+1)<<16|data16;
	for(i=0;i<num;i++){
		g_cmpdata[i+1]=data[i];
	}
	return 0;
}

/*
	Function to insert a string data.
		unsigned char type: Data type number (0-255)
		short data16:       16 bit data. If not required, set 0.
		unsigned char* str: String to insert.
		int num:            Length of string.
*/

int cmpdata_insert_string(unsigned char type, short data16, unsigned char* str, int num){
	unsigned char datanum=(num+8)>>2; // If length of str is 4, 3 integer data area is required (1 for hash, 2 for string)
	unsigned char* datastr;
	int i;
	g_cmpdata-=datanum+1;
	g_objmax=(unsigned short*)&g_cmpdata[0];
	if (g_objmax<object) return ERROR_OBJ_TOO_LARGE;
	g_cmpdata[0]=(type<<24)|(datanum+1)<<16|data16;
	g_cmpdata[1]=cmpdata_nhash(str,num);
	datastr=(unsigned char*)&g_cmpdata[2];
	for(i=0;i<num;i++){
		datastr[i]=str[i];
	}
	datastr[i]=0x00;
	return 0;
}


/*
	Find the next record with defined type. Return the pointer to the record.
*/
int* cmpdata_find(unsigned char type){
	int* ret;
	while(g_cmpdata_point<g_cmpdata_end){
		// Remember return value
		ret=g_cmpdata_point;
		// Move the point to next
		g_cmpdata_point+=(ret[0]&0x00ff0000)>>16;
		// Check if type is the same. If the same, return.
		if ((ret[0]>>24)==type) return ret;
		// If type is CMPDATA_ALL return
		if (CMPDATA_ALL==type) return ret;
	}
	return 0;
}

/*
	Find the record from beginning.
*/

int* cmpdata_findfirst(unsigned char type){
	cmpdata_reset();
	return cmpdata_find(type);
}

/*
	Delete a record.
*/
void cmpdata_delete(int* record){
	int delnum;
	int* data;
	// Ignore if invalid record.
	if (record<g_cmpdata || g_cmpdata_end<record) return;
	// Get number of word to delete.
	delnum=(record[0]&0x00ff0000)>>16;
	// Delete record by shifting data.
	for(data=record-1;g_cmpdata<=data;data--){
	     data[delnum]=data[0];
	}
	g_cmpdata+=delnum;
	g_objmax=(unsigned short*)&g_cmpdata[0];
	// Reset
	cmpdata_reset();
}

/*
	Hash used for faster search
*/
int cmpdata_nhash(unsigned char* str, int num){
	int i;
	int hash=0;
	for(i=0;i<num;i++){
		hash^=str[i];
		hash<<=7;
		hash^=str[i];
	}
	return hash;
}

int cmpdata_hash(unsigned char* str){
	int num;
	for(num=0;str[num];num++);
	return cmpdata_nhash(str,num);
}

/*
	String search
*/
int* cmpdata_nsearch_string(unsigned int type,unsigned char* str,int num){
	int i;
	int* data;
	unsigned char* strdata;
	int hash=cmpdata_nhash(str,num);
	while(data=cmpdata_find(type)){
		// Check the hash, first
		if (hash!=data[1]) continue;
		// Check the string
		strdata=(unsigned char*)(&data[2]);
		for(i=0;i<num;i++){
			if (str[i]!=strdata[i]) break;
		}
		if (i<num) continue;
		// Must end with null
		if (0x00!=strdata[i]) continue;
		// String is the same
		return data;
	}
	// Not found
	return 0;
}

int* cmpdata_search_string(unsigned int type,unsigned char* str){
	int num;
	for(num=0;str[num];num++);
	return cmpdata_nsearch_string(type,str,num);
}

int* cmpdata_nsearch_string_first(unsigned int type,unsigned char* str,int num){
	cmpdata_reset();
	return cmpdata_nsearch_string(type,str,num);
}

int* cmpdata_search_string_first(unsigned int type,unsigned char* str){
	cmpdata_reset();
	return cmpdata_search_string(type,str);
}
