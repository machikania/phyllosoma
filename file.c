/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./api.h"
#include "./debug.h"

/*
	Use following file APIs:
		FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);
		FRESULT f_close (FIL* fp);
		TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);
		#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
*/


int compile_file(unsigned char* fname){
	int e;
	FIL fpo;
	FIL* fp=&fpo;
	if (f_open(fp,fname,FA_READ)) return ERROR_FILE;
	while(!f_eof(fp)){
		if (f_gets(g_file_buffer,g_file_buffer_size,fp)) break;
		e=compile_line(g_file_buffer);
		if (e<0) {
			f_close(fp);
			return e;
		}
	}
	if (f_close(fp)) return ERROR_FILE;
	return 0;
}
