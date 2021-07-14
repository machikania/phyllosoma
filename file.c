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

FATFS g_FatFs;

void init_file_system(void){
	printstr("Initializing file system, ");
	if (FR_OK==f_mount(&g_FatFs, "", 0)) printstr("done\n");
	else printstr("failed\n");
}

int compile_file(unsigned char* fname){
	int e;
	FIL fpo;
	FIL* fp=&fpo;
	unsigned char* classfile;
	printstr("Compiling ");
	printstr(fname);
	printchar('\n');
	if (f_open(fp,fname,FA_READ)) {
		// Mount and open again
		if (f_mount(&g_FatFs, "", 0)) return show_error(ERROR_FILE,0);
		if (f_open(fp,fname,FA_READ)) return show_error(ERROR_FILE,0);
	}
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer,g_file_buffer_size,fp)) break;
		e=compile_line(g_file_buffer);
		if (ERROR_COMPILE_CLASS==e) {
			// Compiling a class is needed.
			// Close current file, first
			f_close(fp);
			// Compile the class
			classfile=g_class_file;
			e=compile_file(classfile);
			if (e) return e;
			// Delete the string stack stored for file name
			cmpdata_delete_string_stack(classfile);
			// Open current file again, and continue from the beginning
			if (f_open(fp,fname,FA_READ)) return show_error(ERROR_FILE,0);
			continue;
		} else if (e<0) {
			f_close(fp);
			return e;
		}
	}
	f_close(fp);
	return 0;
}
