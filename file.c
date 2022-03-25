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

/*
	File-related statements/functions follow
		FCLOSE [x]
		FGET x,y 関数として呼ばれた場合は、読み込みに成功したバイト数を返す。
		FILE x
		FOPEN x$,y$[,z]
		FPRINT [ xまたはx$またはx# [ ,または; [ yまたはy$またはy# [ ... ]]]]
		FPUT x,y 関数として呼ばれた場合は、読み込みに成功したバイト数を返す。
		FPUTC x
		FREMOVE x$
		FSEEK x
		SETDIR x$
		FEOF()
		FGETC()
		FLEN()
		FSEEK()
		FINPUT$([x])
		GETDIR$()
*/

FIL* g_pFileHandles[2];
FIL g_FileHandles[2];
char g_active_handle;

void close_all_files(void){
	if (g_pFileHandles[0]) f_close(g_pFileHandles[0]);
	if (g_pFileHandles[1]) f_close(g_pFileHandles[1]);
	g_pFileHandles[0]=0;
	g_pFileHandles[1]=0;
	g_active_handle=1;
}

int lib_file(int r0, int r1, int r2){
	FIL* fhandle=g_pFileHandles[g_active_handle-1];
	unsigned char* str;
	switch(r2){
		case FILE_FCLOSE:
			if (1==r0 || 2==r0) {
				if (g_pFileHandles[r0-1]) f_close(g_pFileHandles[r0-1]);
				g_pFileHandles[r0-1]=0;
			} else {
				if (fhandle) f_close(fhandle);
				g_pFileHandles[g_active_handle-1]=0;
			}
			break;
		case FILE_FGET:
			if (f_read(fhandle,(void*)r1,r0,(int*)&g_scratch_int[0])) {
				return 0;
			} else {
				return g_scratch_int[0];
			}
			break;
		case FILE_FILE:
			g_active_handle= (2==r0) ? 2:1;
			break;
		case FILE_FPUT:
			if (f_write(fhandle,(void*)r1,r0,(int*)&g_scratch_int[0])) {
				return 0;
			} else {
				return g_scratch_int[0];
			}
			break;
		case FILE_FPUTC:
			return (1==f_putc((TCHAR)r0,fhandle)) ? 1:0;
			break;
		case FILE_FREMOVE:
			return f_unlink((TCHAR*)r0) ? -1:0;
			break;
		case FILE_FSEEK:
			f_lseek(fhandle,r0);
			break;
		case FILE_SETDIR:
			return f_chdir((char*)r0);
			break;
		case FILE_FEOF:
			return f_eof(fhandle) ? 1:0;
			break;
		case FILE_FGETC:
			if (f_read(fhandle,(char*)&g_scratch[0],1,0)) {
				// Error
				return -1;
			} else {
				// OK
				return (unsigned char)g_scratch[0];
			}
			break;
		case FILE_FLEN:
			return f_size(fhandle);
			break;
		case FILE_FSEEKFUNC:
			return f_tell(fhandle);
			break;
		case FILE_FINPUT:
			if (r0<0) r0=f_size(fhandle)-f_tell(fhandle);
			r0++;
			if (512<r0) r0=512;
			str=alloc_memory((r0+3)/4,-1);
			if (f_gets(str,r0,fhandle)) {
				str[r0-1]=0;
			} else {
				str[0]=0;
			}
			return (int)str;
			break;
		case FILE_GETDIR:
			str=alloc_memory(32,-1);
			if (f_getcwd(str,128)) {
				str[0]=0;
			}
			return (int)str;
			break;
		default:
			break;
	}
	return 0;
}

int lib_fopen(int r0, int r1, int r2){
	char* filename=(char*)r2;
	char* modestr=(char*)r1;
	char mode;
	// Check file mode
	switch(modestr[0]){
		case 'r':
		case 'R':
			mode=FA_OPEN_EXISTING | FA_READ;
			if ('+'==modestr[1]) mode|=FA_WRITE;
			break;
		case 'w':
		case 'W':
			mode=FA_CREATE_ALWAYS | FA_WRITE;
			if ('+'==modestr[1]) mode|=FA_READ;
			break;
		case 'a':
		case 'A':
			mode=FA_CREATE_ALWAYS | FA_WRITE;
			if ('+'==modestr[1]) mode|=FA_READ;
			break;
		default:
			stop_with_error(ERROR_SYNTAX);
	}
	// File handle is either 1 or 2
	if (2!=r0) r0=1;
	// Close file if it was open
	if (g_pFileHandles[r0-1]) f_close (g_pFileHandles[r0-1]);
	// Open file
	if (f_open(&g_FileHandles[r0-1],filename,mode)){
		// Cannot open the file. Return zero
		g_pFileHandles[r0-1]=0;
		return 0;
	}
	// File sucessfully opened. Return file handle
	g_pFileHandles[r0-1]=&g_FileHandles[r0-1];
	return r0;
}

#ifdef printstr
	#undef printstr
#endif
#define printstr(a) fprintstr(a)
void fprintstr(char* str){
	FIL* fhandle=g_pFileHandles[g_active_handle-1];
	if (f_puts(str,fhandle)<0) stop_with_error(ERROR_FILE);
}

int lib_fprint_main(int r0, int r1, int r2){
	// Copy the code from lib_print_main
	int i;
	float f;
	char* buff=(char*)&g_scratch[0];
	switch(r1&0x0f){
		case 0x01: // string
			if (r0) {
				for(i=0;((unsigned char*)r0)[i];i++);
				printstr((unsigned char*)r0);
				garbage_collection((char*)r0);
			} else {
				i=0;
			}
			if (0x00 == (r1&0xf0)) printstr("\n");
			break;
		case 0x02: // float
			g_scratch_int[0]=r0;
			f=g_scratch_float[0];
			if (0x00 == (r1&0xf0)) i=snprintf(buff,sizeof g_scratch,"%g\n",f);
			else i=snprintf(buff,sizeof g_scratch,"%g",f);
			printstr(buff);
			break;
		default:   // integer
			if (0x00 == (r1&0xf0)) i=snprintf(buff,sizeof g_scratch,"%d\n",(int)r0);
			else i=snprintf(buff,sizeof g_scratch,"%d",(int)r0);
			printstr(buff);
			break;
	}
	if (0x20==(r1&0xf0)) {
		// ","
		printstr(&("                "[i&0xf]));
	}
	return r0;
}

int fclose_statement(void){
	g_default_args[1]=0;
	return argn_function(LIB_FILE,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		FILE_FCLOSE<<LIBOPTION);
}
int fget_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		FILE_FGET<<LIBOPTION);
}
int file_statement(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FILE<<LIBOPTION);
}
int fopen_function(void){
	g_default_args[3]=1;
	return argn_function(LIB_FOPEN,
		ARG_STRING<<ARG1 | 
		ARG_STRING<<ARG2 | 
		ARG_INTEGER_OPTIONAL<<ARG3);
}
int fput_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		FILE_FPUT<<LIBOPTION);
}
int fputc_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FPUTC<<LIBOPTION);
}
int fremove_function(void){
	return argn_function(LIB_FILE,
		ARG_STRING<<ARG1 |
		FILE_FREMOVE<<LIBOPTION);
}
int fseek_statement(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FSEEK<<LIBOPTION);
}
int setdir_function(void){
	return argn_function(LIB_FILE,
		ARG_STRING<<ARG1 |
		FILE_SETDIR<<LIBOPTION);
}
int feof_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FEOF<<LIBOPTION);
}
int fgetc_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FGETC<<LIBOPTION);
}
int flen_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FLEN<<LIBOPTION);
}
int fseek_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FSEEKFUNC<<LIBOPTION);
}
int finput_function(void){
	g_default_args[1]=-1;
	return argn_function(LIB_FILE,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		FILE_FINPUT<<LIBOPTION);
}
int getdir_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_GETDIR<<LIBOPTION);
}
