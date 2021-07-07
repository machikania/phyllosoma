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

#ifdef DEBUG_MODE
#ifdef FF_DEFINED

FRESULT debug_f_open (FIL* fp, const TCHAR* path, BYTE mode);
FRESULT debug_f_close (FIL* fp);
FRESULT debug_f_read (FIL* fp, void* buff, UINT btr, UINT* br);
#define f_open debug_f_open
#define f_close debug_f_close
#define f_read  debug_f_read

#endif // FF_DEFINED
#endif // DEBUG_MODE

/*
	Dump code and cmpdata
*/

#ifdef DEBUG_MODE

void dump(void);
void dump_cmpdata(void);

#else // DEBUG_MODE

void debug_dummy(void);
#define dump() debug_dummy()
#define dump_cmpdata() debug_dummy()

#endif // DEBUG_MODE
