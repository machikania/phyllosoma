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

#ifdef MACHIKANIA_DEBUG_MODE
#define MACHIKANIA_DEBOG_MODE_STR " (embed)"
#else
#define MACHIKANIA_DEBOG_MODE_STR ""
#endif

/*
	File to compile
*/

#ifdef MACHIKANIA_DEBUG_MODE
#ifdef FF_DEFINED

FRESULT debug_f_open (FIL* fp, const TCHAR* path, BYTE mode);
FRESULT debug_f_close (FIL* fp);
TCHAR* debug_f_gets (TCHAR* buff, int len, FIL* fp);
FRESULT debug_f_getcwd (TCHAR* buff, UINT len);
FRESULT debug_f_chdir (const TCHAR* path);
int debug_file_exists(unsigned char* fname);
FRESULT debug_f_read (FIL* fp, TCHAR* buff, UINT btr, UINT* br);
FRESULT debug_f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);
int debug_f_putc (TCHAR c, FIL* fp);
FRESULT debug_f_unlink (const TCHAR* path);
FRESULT debug_f_mount (FATFS* fs, const TCHAR* path, BYTE opt);
FRESULT debug_f_lseek (FIL* fp, FSIZE_t ofs);
FRESULT debug_f_opendir (DIR* dp, const TCHAR* path);
FRESULT debug_f_findnext (DIR* dp, FILINFO* fno);
FRESULT debug_f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);

#define f_open debug_f_open
#define f_close debug_f_close
#define f_gets debug_f_gets
#define f_getcwd debug_f_getcwd
#define f_chdir debug_f_chdir
#undef file_exists
#define file_exists debug_file_exists
#define f_read debug_f_read
#define f_write debug_f_write
#define f_putc debug_f_putc
#define f_unlink debug_f_unlink
#define f_mount debug_f_mount
#define f_lseek debug_f_lseek
#define f_opendir debug_f_opendir	
#define f_findnext debug_f_findnext
#define f_findfirst debug_f_findfirst

unsigned char* debug_fileselect(void);
#define fileselect debug_fileselect

#endif // FF_DEFINED
#endif // MACHIKANIA_DEBUG_MODE


/*
	Dump code and cmpdata
*/

#ifdef MACHIKANIA_DEBUG_MODE

void dump(void);

#else // MACHIKANIA_DEBUG_MODE

void debug_dummy(void);
#define dump() debug_dummy()
#define dump_cmpdata() debug_dummy()
#define dump_variables() debug_dummy()

#endif // MACHIKANIA_DEBUG_MODE

