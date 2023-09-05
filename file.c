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
	if (FR_OK!=f_mount(&g_FatFs, "", 0)) printstr("Initializing file system failed\n");
}

int mmc_file_exists(unsigned char* fname){
	FILINFO fileinfo;
	fileinfo.fsize=-1;
	f_stat(fname,&fileinfo);
	return fileinfo.fsize+1;
}

int compile_file(unsigned char* fname, char isclass){
	int e,i,num,class_id;
	FIL fpo;
	FIL* fp=&fpo;
	unsigned char* classfile;
	unsigned char curdir[64];
	unsigned short* bl;
	unsigned char stackfname[13];
	// Store current g_class_id
	class_id=g_class_id;
	// Copy file name to stack
	for(i=0;i<13;i++){
		if (!(stackfname[i]=fname[i])) break;
	}
	fname=&stackfname[0];
	// Initialize
	char showfilename=1;
	begin_file_compiler();
	// Store current directory
	if (f_getcwd(g_file_buffer,g_file_buffer_size)) return show_error(ERROR_UNKNOWN,0);
	for(i=0;curdir[i]=g_file_buffer[i];i++) {
		if ((sizeof curdir/sizeof curdir[0])-1==i) return ERROR_PATH_TOO_LONG;
	}
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
				// Succesfully opened the file. Change the current dirctory
				for(i=0;curdir[i]=g_file_buffer[i];i++) {
					if ((sizeof curdir/sizeof curdir[0])-1==i) return ERROR_PATH_TOO_LONG;
				}
			} else {
				// The main file not found
				return show_error(ERROR_FILE,0);
			}
		}
	}
	// Compile the file until EOF
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer,g_file_buffer_size,fp)) break;
		g_error_linenum++;
		e=compile_line(g_file_buffer);
		if (g_before_classcode) {
			// Compiling a class file as the same file as BASIC main file
			// The codes before "OPTION CLASSCODE" will be ignored
			if (ERROR_OPTION_CLASSCODE==e) {
				g_before_classcode=0;
				g_after_classcode=1;
				e=0;
			} else if (e) {
				// Ignore the error before reaching "OPTION CLASSCODE"
				e=0;
			}
		}
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
			num=g_class_id;
			classfile=g_class_file;
			e=compile_file(classfile,1);
			if (e) return e;
			e=post_compilling_a_class();
			if (e) return e;
			// BL jump destination is here
			update_bl(bl,object);
			// Delete the string stack stored for file name
			cmpdata_delete_string_stack(classfile);
			// Restore g_class_id
			g_class_id=class_id;
			// Open current file again, and continue from the beginning
			f_chdir(curdir);
			if (f_open(fp,fname,FA_READ)) return show_error(ERROR_FILE,0);
			begin_file_compiler();
			continue;
		} else if (ERROR_OPTION_CLASSCODE==e) {
			// "OPTION CLASSCODE" found.
			// Ignore following codes.
			break;
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
	// Add "END" statement at the end
	for(i=0;g_file_buffer[i]="END"[i];i++);
	e=compile_line(g_file_buffer);
	if (e<0) return e;
	// Delete the temporary string for current directory
	cmpdata_delete_string_stack(curdir);
	return end_file_compiler();
}

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
	static DIR dj;      // Directory object
	static FILINFO fno; // File information
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
			r2=f_unlink((TCHAR*)r0) ? -1:0;
			garbage_collection((char*)r0);
			return r2;
			break;
		case FILE_FSEEK:
			f_lseek(fhandle,r0);
			break;
		case FILE_SETDIR:
			r2=f_chdir((char*)r0);
			garbage_collection((char*)r0);
			return r2;
			break;
		case FILE_FEOF:
			return f_eof(fhandle) ? 1:0;
			break;
		case FILE_FGETC:
			if (f_read(fhandle,(char*)&g_scratch[0],1,(unsigned int*)&g_scratch_int[1])) {
				// Error
				return -1;
			} else {
				// OK or EOF
				return g_scratch_int[1] ? (unsigned char)g_scratch[0]:-1;
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
		case FILE_FFIND:
			// r1: pattern
			// r0: path
			if (0==r1) {
				// find next mode
				if (FR_OK==f_findnext(&dj, &fno)) return (int)fno.fname;
			} else if (0==r0) {
				// find first mode with current directory
				if (FR_OK==f_findfirst(&dj, &fno, ".", (char*)r1)) return (int)&fno.fname[0];
			} else {
				// find first mode with specified directory
				if (FR_OK==f_findfirst(&dj, &fno, (char*)r0, (char*)r1)) return (int)&fno.fname[0];
			}
			// No file found
			return (int)"";
		case FILE_FINFO:
			switch(r0){
				case 0: // File size
					return fno.fsize;
				case 1: // Modified date
					return fno.fdate;
				case 2: // Modified time
					return fno.ftime;
				case 3: // File attribute
					return (unsigned int)fno.fattrib;
				default:
					return 0;
			}
		case FILE_FINFOSTR:
			switch(r0){
				case 0: // ISO-8601 string, YYYY-MM-DDThh:mm:ss
					str=alloc_memory(5,-1);
					sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d",
						((fno.fdate>>9)&127)+1980,
						(fno.fdate>>5)&15,
						(fno.fdate>>0)&31,
						(fno.ftime>>11)&31,
						(fno.ftime>>5)&63,
						(fno.ftime<<1)&63 );
					return (int)str;
				case 1: // Date string, YYYY-MM-DD
					str=alloc_memory(3,-1);
					sprintf(str,"%04d-%02d-%02d",
						((fno.fdate>>9)&127)+1980,
						(fno.fdate>>5)&15,
						(fno.fdate>>0)&31 );
					return (int)str;
				case 2: // Time string, hh:mm:ss
					str=alloc_memory(3,-1);
					sprintf(str,"%02d:%02d:%02d",
						(fno.ftime>>11)&31,
						(fno.ftime>>5)&63,
						(fno.ftime<<1)&63 );
					return (int)str;
				case 3: // Attribute string, "rhsda"
					str=alloc_memory(2,-1);
					if (AM_RDO & fno.fattrib) str[0]='r'; else str[0]='w'; // Read only 
					if (AM_HID & fno.fattrib) str[1]='h'; else str[1]='-'; // Hidden 
					if (AM_SYS & fno.fattrib) str[2]='s'; else str[2]='-'; // System 
					if (AM_DIR & fno.fattrib) str[3]='d'; else str[3]='-'; // Directory 
					if (AM_ARC & fno.fattrib) str[4]='a'; else str[4]='-'; // Archive 
					str[5]=0;
					return (int)str;
				case 4: // File name
					return (int)&fno.fname[0];
				default:
					return (int)"";
			}
		default:
			break;
	}
	return 0;
}

int lib_fopen_main(int r0, int r1, int r2){
	char* filename=(char*)r2;
	char* modestr=(char*)r1;
	char mode;
	static DIR dobj;
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
			mode=FA_OPEN_APPEND | FA_WRITE;
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
		// Check if directory can be open (or if the card is inserted)
		if (FR_OK==f_opendir(&dobj,".")) {
			// The card is inserted, but file cannot be open
			g_pFileHandles[r0-1]=0;
			return 0;
		}
		// Mount again
		if (f_mount(&g_FatFs, "", 0)) {
			// Cannot mount. Return zero
			g_pFileHandles[r0-1]=0;
			return 0;
		}
		// Open again
		if (f_open(&g_FileHandles[r0-1],filename,mode)){
			// Cannot open the file. Return zero
			g_pFileHandles[r0-1]=0;
			return 0;
		}
	}
	// File sucessfully opened. Return file handle
	g_pFileHandles[r0-1]=&g_FileHandles[r0-1];
	return r0;
}
int lib_fopen(int r0, int r1, int r2){
	char* filename=(char*)r2;
	char* modestr=(char*)r1;
	r0=lib_fopen_main(r0,r1,r2);
	garbage_collection((char*)r1);
	garbage_collection((char*)r2);
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
int ffind_function(void){
	g_default_args[1]=0;
	g_default_args[2]=0;
	return argn_function(LIB_FILE,
		ARG_STRING_OPTIONAL<<ARG1 |
		ARG_STRING_OPTIONAL<<ARG2 |
		FILE_FFIND<<LIBOPTION);	
}
int finfo_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FINFO<<LIBOPTION);
}
int finfostr_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FINFOSTR<<LIBOPTION);
}
