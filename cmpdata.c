/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./api.h"

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
	CMPDATA_TEMP structure, used to store any temporary data
		type:      CMPDATA_TEMP
		len:       n+1
		data16:    id
		record[1]: any data
		record[2]: any data
		...
		record[n]: any data
	
	CMPDATA_STRSTACK used to store strings as stack
		type:      CMPDATA_TEMP
		len:       n+1
		data16:    id
		record[1]: start string
		...
		record[n]: end string
	
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
	g_cmpdata_id=ALLOC_BLOCK_NUM; // Avoid collision between id and variable number
}

/*
	Returns current record (g_cmpdata_point)
*/

int* cmpdata_current_record(void){
	return g_cmpdata_point;
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
	unsigned int i;
	// Check the type
	if (CMPDATA_STRSTACK==type) {
		// Store the new record in the end as stack
		g_cmpdata_point=cmpdata_findfirst(CMPDATA_STRSTACK);
		if (!g_cmpdata_point) g_cmpdata_point=g_cmpdata_end;
		// Shift the record
		for(i=0;(&g_cmpdata[i])<g_cmpdata_point;i++){
			g_cmpdata[i-num-1]=g_cmpdata[i];
		}
		g_cmpdata_point-=num+1;
	} else {
		// Store the new record in the beginning
		g_cmpdata_point=g_cmpdata-num-1;
	}
	g_cmpdata-=num+1;
	g_objmax=(unsigned short*)&g_cmpdata[0];
	if (g_objmax<object) return ERROR_OBJ_TOO_LARGE;
	// Store the new record at the position
	g_cmpdata_point[0]=(type<<24)|(num+1)<<16|data16;
	if (data) {
		for(i=0;i<num;i++){
			g_cmpdata_point[i+1]=data[i];
		}
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

int* cmpdata_findfirst_with_id(unsigned char type, unsigned short id){
	int* data;
	cmpdata_reset();
	while(data=cmpdata_find(type)){
		if ((data[0]&0xffff)==id) break;
	}
	return data;
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
	Delete all record of specific type
*/
void cmpdata_delete_all(unsigned char type){
	int* data;
	cmpdata_reset();
	while(data=cmpdata_find(type)) cmpdata_delete(data);
}

/*
	Delete all records with invalid object positions.
*/
void cmpdata_delete_invalid(void){
	const static int const types[7]={
		CMPDATA_GOTO_NUM_BL,
		CMPDATA_GOTO_LABEL_BL,
		CMPDATA_DATA_LABEL_BL,
		CMPDATA_BREAK_BL,
		CMPDATA_IF_BL,
		CMPDATA_ENDIF_BL,
		CMPDATA_CONTINUE,
	};
	int typenum;
	int* data;
	for(typenum=0;typenum<(sizeof types/sizeof types[0]);typenum++){
		do {
			cmpdata_reset();
			while(data=cmpdata_find(types[typenum])){
				if ((int)object<data[1]) {
					// This is invalid
					cmpdata_delete(data);
					break;
				}
			}
		} while (data);
	}
}

/*
	Hash used for faster string search
*/
int cmpdata_nhash(unsigned char* str, int num){
	int i;
	int hash=0;
	for(i=0;i<num;i++){
		hash=hash<<6 ^ hash>>26;
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

/*
	String stack
*/
unsigned char* cmpdata_insert_string_stack(int num){
	int i;
	unsigned char* res;
	// Create a CMPDATA record
	i=cmpdata_insert(CMPDATA_STRSTACK,cmpdata_get_id(),0,(num+3)/4);
	if (i) return 0;
	return (unsigned char*)(&g_cmpdata_point[1]);
}

void cmpdata_delete_string_stack(unsigned char* str){
	// Delete only the top of stack
	int* data;
	data=cmpdata_findfirst(CMPDATA_STRSTACK);
	if (str==(unsigned char*)(&data[1])) cmpdata_delete(data);
}
