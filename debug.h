/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Use embed codes for compiling instead of files in MMC card
*/

#ifndef KMBASIC_COMPILER_H
#error "compiler.h" must be included before "debug.h"
#endif

void memdump(void);

/*
	File to compile
*/

#ifdef DEBUG_MODE
#ifdef FF_DEFINED

FRESULT debug_f_open (FIL* fp, const TCHAR* path, BYTE mode);
FRESULT debug_f_close (FIL* fp);
TCHAR* debug_f_gets (TCHAR* buff, int len, FIL* fp);
FRESULT debug_f_getcwd (TCHAR* buff, UINT len);
FRESULT debug_f_chdir (const TCHAR* path);
int debug_file_exists(unsigned char* fname);
#define f_open debug_f_open
#define f_close debug_f_close
#define f_gets debug_f_gets
#define f_getcwd debug_f_getcwd
#define f_chdir debug_f_chdir
#undef file_exists
#define file_exists debug_file_exists

unsigned char* debug_fileselect(void);
#define fileselect debug_fileselect

#endif // FF_DEFINED
#endif // DEBUG_MODE


/*
	Dump code and cmpdata
*/

#ifdef DEBUG_MODE

void dump(void);

#else // DEBUG_MODE

void debug_dummy(void);
#define dump() debug_dummy()
#define dump_cmpdata() debug_dummy()
#define dump_variables() debug_dummy()

#endif // DEBUG_MODE

