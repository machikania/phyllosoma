/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#define DISPLAY_BGCOLOR 1
#define DISPLAY_CLS 2
#define DISPLAY_COLOR 3
#define DISPLAY_CURSOR 4
#define DISPLAY_PALETTE 5
#define DISPLAY_PCG 6
#define DISPLAY_USEPCG 7
#define DISPLAY_SCROLL 8
#define DISPLAY_WIDTH 9
#define DISPLAY_TVRAM 10
#define DISPLAY_BOXFILL 11
#define DISPLAY_CIRCLE 12
#define DISPLAY_CIRCLEFILL 13
#define DISPLAY_GCLS 14
#define DISPLAY_GCOLOR 15
#define DISPLAY_GPALETTE 16
#define DISPLAY_GPRINT 17
#define DISPLAY_LINE 18
#define DISPLAY_POINT 19
#define DISPLAY_PSET 20
#define DISPLAY_PUTBMP 21
#define DISPLAY_PUTBMP2 22
#define DISPLAY_USEGRAPHIC 23
#define DISPLAY_GCOLOR_FUNC 24
#define DISPLAY_USE_STACK (\
	(1<<DISPLAY_BGCOLOR) |\
	(1<<DISPLAY_PALETTE) |\
	(1<<DISPLAY_PCG) |\
	(1<<DISPLAY_BOXFILL) |\
	(1<<DISPLAY_CIRCLE) |\
	(1<<DISPLAY_CIRCLEFILL) |\
	(1<<DISPLAY_GPALETTE) |\
	(1<<DISPLAY_GPRINT) |\
	(1<<DISPLAY_LINE) |\
	(1<<DISPLAY_PUTBMP) |\
	(1<<DISPLAY_PUTBMP2) |\
	(1<<DISPLAY_PSET) )

void display_init(void);
int display_statements(void);
int display_functions(void);
int lib_display(int r0, int r1, int r2);
