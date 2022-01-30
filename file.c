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

int compile_file(unsigned char* fname, char isclass){
	int e,i,num;
	FIL fpo;
	FIL* fp=&fpo;
	unsigned char* classfile;
	unsigned char* curdir;
	unsigned short* bl;
	// Initialize
	char showfilename=1;
	begin_file_compiler();
	// Store current directory
	if (f_getcwd(g_file_buffer,g_file_buffer_size)) return show_error(ERROR_UNKNOWN,0);
	for(num=0;g_file_buffer[num]!=0x00;num++);
	curdir=cmpdata_insert_string_stack(num+1);
	if (!curdir) return show_error(ERROR_OBJ_TOO_LARGE,0);
	for(i=0;i<=num;i++) curdir[i]=g_file_buffer[i];
	// Open file
	if (f_open(fp,fname,FA_READ)) {
		// Mount and open again
		if (f_mount(&g_FatFs, "", 0)) return show_error(ERROR_FILE,0);
		if (f_open(fp,fname,FA_READ)) {
			if (isclass) {
				// Class file not found in the current directory
				// The class file may be found as "/lib/classname/classname.bas"
				// Change current directory to "/lib/classname"
				for(num=0;fname[num]!='.';num++);
				for(i=0;i<5;i++) g_file_buffer[i]="/lib/"[i];
				for(i=0;i<num;i++) g_file_buffer[i+5]=fname[i];
				g_file_buffer[i+5]=0x00;
				f_chdir(g_file_buffer);
				// Open the class file again
				if (f_open(fp,fname,FA_READ)) {
					// Error
					f_chdir(curdir);
					return show_error(ERROR_NO_CLASS_FILE,0);
				}
				// Succesfully opened the file.
			} else {
				// The main file not found
				return show_error(ERROR_FILE,0);
			}
		}
	}
	// Compile the file until EOF
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer,g_file_buffer_size,fp)) break;
		e=compile_line(g_file_buffer);
		if (ERROR_COMPILE_CLASS==e) {
			// Compiling a class is needed.
			// Close current file, first
			f_close(fp);
			// Delete all CMPDATA_LINENUM as these will be registered later
			cmpdata_delete_all(CMPDATA_LINENUM);
			// Insert a BL instruction to skip class code
			bl=object;
			object+=2;
			// Compile the class
			classfile=g_class_file;
			e=compile_file(classfile,1);
			if (e) return e;
			e=post_compilling_a_class();
			if (e) return e;
			// BL jump destination is here
			update_bl(bl,object);
			// Delete the string stack stored for file name
			cmpdata_delete_string_stack(classfile);
			// Open current file again, and continue from the beginning
			f_chdir(curdir);
			if (f_open(fp,fname,FA_READ)) return show_error(ERROR_FILE,0);
			begin_file_compiler();
			continue;
		}
		if (showfilename) {
			showfilename=0;
			printstr("Compiling ");
			printstr(fname);
			printchar('\n');
		}
		if (e<0) {
			f_close(fp);
			printstr(g_file_buffer);
			return e;
		}
	}
	f_close(fp);
	// Delete the temporary string for current directory
	cmpdata_delete_string_stack(curdir);
	return end_file_compiler();
}
