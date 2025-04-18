/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "./compiler.h"
#include "./api.h"
#include "./display.h"

/*
	Default LCD definitions
*/

#ifndef LCD_COLUMN_RES
#define LCD_COLUMN_RES 240
#endif
#ifndef LCD_ROW_RES
#define LCD_ROW_RES 320
#endif

/*
	Local prototypings
*/
int lib_cread(int r0, int r1, int r2);

/*

CLEAR
	すべての文字列型変数と整数型配列を破棄し、整数値を０とする。また、
	PCGの使用をやめ、表示キャラクターをリセットする。
*/

int lib_display(int r0, int r1, int r2){
	static unsigned char cursorcolor=7;
	static unsigned char gcolor=7;
	static unsigned char* ppcg=0;
	static unsigned char* pgvram1=0;
	static unsigned char* pgvram2=0;
	static unsigned char* pgvram_draw=0;
	static int permanent_block_number1=0;
	static int permanent_block_number2=0;
	static int prevx1=0,prevy1=0;
	int* sp=(int*)r1;
	int i,j,gc;
	unsigned int x1,y1,x2,y2;
	if (RESET_STATIC_VARS==r2) {
		cursorcolor=7;
		gcolor=7;
		ppcg=0;
		prevx1=0;
		prevy1=0;
		pgvram1=0;
		pgvram2=0;
		pgvram_draw=0;
		permanent_block_number1=0;
		permanent_block_number2=0;
		return r0;
	} else if (0==r2) {
		// Return the static data
		switch(r0){
			case 0:
				return cursorcolor;
			case 1:
				return gcolor;
			case 2:
				return (int)ppcg;
			case 3:
				return prevx1;
			case 4:
				return prevy1;
			case 5:
				return (int)pgvram_draw;
			default:
				return 0;
		}
	}
	// Check if graphic function is available
	if ( (DISPLAY_USE_GRAPHIC & (1<<r2)) && (!pgvram_draw) && PUERULUS ) stop_with_error(ERROR_FUNCTION_CALL);
	// Set x1,y1,x2,y2 for graphic
	if (DISPLAY_USE_STACK & (1<<r2)) {
		// r1 is a pointer to stack
		x1=sp[0];
		y1=sp[1];
		x2=sp[2];
		y2=sp[3];
		if ((x1&0xF0000000) == 0x80000000) x1=prevx1;
		if ((y1&0xF0000000) == 0x80000000) y1=prevy1;
	}
	// Set graphic color
	gc=r0;
	if (gc<0) gc=gcolor;
	// Do each
	switch(r2){
		case DISPLAY_BGCOLOR:
			// void set_bgcolor(unsigned char b,unsigned char r,unsigned char g);
			// BGCOLOR r,g,b
			set_bgcolor(r0,sp[0],sp[1]);
			break;
		case DISPLAY_CLS:
			// void cls(void);
			// CLS
			cls();
			break;
		case DISPLAY_COLOR:
			// void setcursorcolor(unsigned char c);
			// COLOR x
			cursorcolor=r0;
			setcursorcolor(r0);
			break;
		case DISPLAY_CURSOR:
			//void setcursor(unsigned char x,unsigned char y,unsigned char c);
			//CURSOR x,y
			setcursor(r1,r0,cursorcolor);
			break;
		case DISPLAY_PALETTE:
		case DISPLAY_GPALETTE:
			//void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g);
			//PALETTE n,r,g,b
			set_palette(sp[0],r0,sp[1],sp[2]);
			break;
		case DISPLAY_PCG:
			//PCG x,y,z
			if (0==ppcg) break;
			ppcg[sp[0]*8+0]=sp[1]>>24;
			ppcg[sp[0]*8+1]=sp[1]>>16;
			ppcg[sp[0]*8+2]=sp[1]>>8;
			ppcg[sp[0]*8+3]=sp[1];
			ppcg[sp[0]*8+4]=r0>>24;
			ppcg[sp[0]*8+5]=r0>>16;
			ppcg[sp[0]*8+6]=r0>>8;
			ppcg[sp[0]*8+7]=r0;
			break;
		case DISPLAY_USEPCG:
			// void startPCG(unsigned char *p,int a);
			switch(r0){
				case 0:
					stopPCG();
					break;
				case 2:
					if (0==ppcg) {
						i=get_permanent_block_number();
						ppcg=alloc_memory(512,i);
					}
					startPCG(ppcg,1);
					break;
				case 1:
				default:
					if (0==ppcg) {
						i=get_permanent_block_number();
						ppcg=alloc_memory(512,i);
						startPCG(ppcg,1);
					} else {
						startPCG(ppcg,0);
					}
					break;
			}
			break;
		case DISPLAY_SCROLL:
			// SCROLL x,y
			// x:r1, y:r0
			// Break if invalid value
			if (WIDTH_Y<r0) break;
			if (WIDTH_Y<(0-r0)) break;
			if (WIDTH_X<r1) break;
			if (WIDTH_X<(0-r1)) break;
			if (0==r0 && 0==r1) break;
			memmove(&TVRAM[0],&TVRAM[(0-r0)*WIDTH_X-r1],WIDTH_X*WIDTH_Y);
			memmove(&TVRAM[attroffset],&TVRAM[attroffset+(0-r0)*WIDTH_X-r1],WIDTH_X*WIDTH_Y);
			if (0<r0) {
				for(i=0;i<WIDTH_X;i++){
					for(j=0;j<r0;j++){
						TVRAM[i+j*WIDTH_X]=0;
						TVRAM[attroffset+i+j*WIDTH_X]=cursorcolor;
					}
				}
			} else if (r0<0) {
				for(i=0;i<WIDTH_X;i++){
					for(j=0;j<(0-r0);j++){
						TVRAM[i+(WIDTH_Y-j-1)*WIDTH_X]=0;
						TVRAM[attroffset+i+(WIDTH_Y-j-1)*WIDTH_X]=cursorcolor;
					}
				}
			}
			if (0<r1) {
				for(i=0;i<r1;i++){
					for(j=0;j<WIDTH_Y;j++){
						TVRAM[i+j*WIDTH_X]=0;
						TVRAM[attroffset+i+j*WIDTH_X]=cursorcolor;
					}
				}
			} else if (r1<0) {
				for(i=0;i<(0-r1);i++){
					for(j=0;j<WIDTH_Y;j++){
						TVRAM[WIDTH_X-i-1+j*WIDTH_X]=0;
						TVRAM[attroffset+WIDTH_X-i-1+j*WIDTH_X]=cursorcolor;
					}
				}
			}
			textredraw();
			break;
		case DISPLAY_WIDTH:
			// Vertical/horizontal setting
			switch(r0){
				case 1:
					set_lcdalign(VERTICAL | (LCD_ALIGNMENT&LCD180TURN));
					break;
				case 2:
					set_lcdalign(HORIZONTAL | (LCD_ALIGNMENT&LCD180TURN));
				default:
					break;
			}
			// Width setting
			if (0<r1 & r1<=(LCD_ALIGNMENT&HORIZONTAL ? (LCD_ROW_RES/8):(LCD_COLUMN_RES/8)) && PHYLLOSOMA) {
				WIDTH_X=r1;
				cls();
			}
			switch(r1){
				case 42:
					set_videomode(VMODE_WIDETEXT,0);
					break;
				case 80:
					set_videomode(VMODE_MONOTEXT,0);
					break;
				default:
					break;
			}
			break;
		case DISPLAY_TVRAM:
			if (r0<0) return (int)&TVRAM[0];
			else return TVRAM[r0];
		case DISPLAY_BOXFILL:
			//void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c);
			g_boxfill(x1,y1,x2,y2,gc);
			prevx1=x2;
			prevy1=y2;
			break;
		case DISPLAY_CIRCLE:
			//void g_circle(int x0,int y0,unsigned int r,unsigned char c);
			g_circle(x1,y1,x2,gc);
			prevx1=x1;
			prevy1=y1;
			break;
		case DISPLAY_CIRCLEFILL:
			//void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
			g_circlefill(x1,y1,x2,gc);
			prevx1=x1;
			prevy1=y1;
			break;
		case DISPLAY_GCLS:
			//void g_clearscreen(void);
			//GCLS
			g_clearscreen();
			break;
		case DISPLAY_GCOLOR:
			//GCOLOR c
			gcolor=r0;
			break;
		case DISPLAY_GPRINT:
			//void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s);
			for(i=0;((unsigned char*)r0)[i];i++);
			prevx1=x1+i*8;
			prevy1=y1;
			g_printstr(x1,y1,x2,((int)y2<0 || PUERULUS) ? (int)y2:palette[y2],(unsigned char*)r0);
			garbage_collection((unsigned char*)r0);
			break;
		case DISPLAY_LINE:
			//void g_gline(int x1,int y1,int x2,int y2,unsigned char c);
			//void g_hline(int x1,int x2,int y,unsigned char c);
			if (y1==y2) g_hline(x1,x2,y1,gc);
			else g_gline(x1,y1,x2,y2,gc);
			prevx1=x2;
			prevy1=y2;
			break;
		case DISPLAY_POINT:
			prevx1=r1;
			prevy1=r0;
			break;
		case DISPLAY_PSET:
			// void g_pset(int x,int y,unsigned char c);
			g_pset(x1,y1,gc);
			prevx1=x1;
			prevy1=y1;
			break;
		case DISPLAY_PUTBMP2:
			// Label is used. Get CREAD() point
			r0=(int)seek_data(0x4636);
			// Now, r0 is the address of bmp data
			// Continue to following main code
		case DISPLAY_PUTBMP:
			// void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[]);
			g_putbmpmn(x1,y1,x2,y2,(unsigned char*)r0);
			prevx1=x1;
			prevy1=y1;
			break;
		case DISPLAY_USEGRAPHIC:
			// The first parameter (r1) processing
			if (PUERULUS && !permanent_block_number1) permanent_block_number1=get_permanent_block_number();
			switch(r1&3){
				case 0:
					if (PHYLLOSOMA) g_clearscreen();
					if (PUERULUS) {
						if (pgvram_draw) {
							delete_memory(pgvram1);
							delete_memory(pgvram2);
							pgvram_draw=pgvram1=pgvram2=0;
						} else {
							pgvram1=calloc_memory(X_RES*Y_RES/4,permanent_block_number1);
							pgvram_draw=pgvram1;
						}
						set_videomode(VMODE_WIDETEXT,0);
					}
					break;
				case 2:
					// clear palette
					for(i=0;i<8;i++){
						set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
					}
					for(i=0;i<8;i++){
						set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
					}
					for(i=16;i<256;i++){
						set_palette(i,255,255,255);
					}
					// clear graphic display
					cls();
					if (PUERULUS) {
						// Allocate memory for graphic VRAM and start graphic mode
						if (!pgvram1) pgvram1=calloc_memory(X_RES*Y_RES/4,permanent_block_number1);
						set_videomode(VMODE_WIDEGRPH,pgvram1);
						pgvram_draw=pgvram1;
					}
					g_clearscreen();
					break;
				case 1:
					if (PUERULUS) {
						// Allocate memory for graphic VRAM and start graphic mode
						if (!pgvram1) pgvram1=calloc_memory(X_RES*Y_RES/4,permanent_block_number1);
						set_videomode(VMODE_WIDEGRPH,pgvram1);
						pgvram_draw=pgvram1;
						//g_clearscreen();
					}
					cls();
					break;
				default:
					if (PUERULUS) {
						// Allocate memory for graphic VRAM but not start graphic mode
						if (!pgvram1) pgvram1=calloc_memory(X_RES*Y_RES/4,permanent_block_number1);
						pgvram_draw=pgvram1;
						set_gvram(pgvram_draw,pgvram_draw);
					}
					//cls();
					break;
			}
			// The second parameter (r0) processing (only for PUERULUS)
			if (!PUERULUS) break;
			if (r0<0 || 3<r0) break; // r0 should be between 0 and 3
			if (r0<=2 && !pgvram1) {
				pgvram1=calloc_memory(X_RES*Y_RES/4,permanent_block_number1);
			}
			if (1<=r0 && !pgvram2) {
				if (!permanent_block_number2) permanent_block_number2=get_permanent_block_number();
				pgvram2=calloc_memory(X_RES*Y_RES/4,permanent_block_number2);
			}
			switch(r0){ //void set_gvram(uint8_t *gvram_draw,uint8_t *gvram_disp);
				case 0:
					set_gvram(pgvram1,pgvram1);
					pgvram_draw=pgvram1;
					break;
				case 1:
					set_gvram(pgvram2,pgvram1);
					pgvram_draw=pgvram2;
					break;
				case 2:
					set_gvram(pgvram1,pgvram2);
					pgvram_draw=pgvram1;
					break;
				default:
					set_gvram(pgvram2,pgvram2);
					pgvram_draw=pgvram2;
					break;
			}
			break;
		case DISPLAY_GCOLOR_FUNC:
			//GCOLOR(x,y)
			// TODO: OPTION RGB565/GPALETTE
			j=g_color(r1,r0);
			if (PUERULUS) return j;
			for(i=0;i<256;i++){
				if (j==palette[i]) return i;
			}
			return 0-j;
		default:
			break;
	}
	return r0;
}

int bgcolor_statement(void){
	// BGCOLOR r,g,b
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		DISPLAY_BGCOLOR<<LIBOPTION);
}

int cls_statement(void){
	// CLS
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_NONE | 
		DISPLAY_CLS<<LIBOPTION);
}

int color_statement(void){
	// COLOR x
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		DISPLAY_COLOR<<LIBOPTION);
}

int cursor_statement(void){
	// CURSOR x,y
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		DISPLAY_CURSOR<<LIBOPTION);
}

int palette_statement(void){
	// PALETTE n,r,g,b
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		DISPLAY_PALETTE<<LIBOPTION);
}

int pcg_statement(void){
	// PCG x,y,z
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		DISPLAY_PCG<<LIBOPTION);
}

int usepcg_statement(void){
	// USEPCG [x]
	g_default_args[1]=1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		DISPLAY_USEPCG<<LIBOPTION);
}

int scroll_statement(void){
	// SCROLL x,y
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		DISPLAY_SCROLL<<LIBOPTION);
}

int width_statement(void){
	// WIDTH x
	g_default_args[2]=0;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		DISPLAY_WIDTH<<LIBOPTION);
}

int boxfill_statement(void){
	// BOXFILL [x1,y1],x2,y2[,c]
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_default_args[5]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		ARG_INTEGER_OPTIONAL<<ARG5 | 
		DISPLAY_BOXFILL<<LIBOPTION);
}

int circle_statement(void){
	// CIRCLE [x,y],r[,c]
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_default_args[4]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER_OPTIONAL<<ARG4 | 
		DISPLAY_CIRCLE<<LIBOPTION);
}

int circlefill_statement(void){
	// CIRCLEFILL [x,y],r[,c]
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_default_args[4]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER_OPTIONAL<<ARG4 | 
		DISPLAY_CIRCLEFILL<<LIBOPTION);
}

int gcls_statement(void){
	// GCLS
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_NONE | 
		DISPLAY_GCLS<<LIBOPTION);
}

int gcolor_statement(void){
	// GCOLOR c
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		DISPLAY_GCOLOR<<LIBOPTION);
}

int gpalette_statement(void){
	// GPALETTE n,r,g,b
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		DISPLAY_GPALETTE<<LIBOPTION);
}

int gprint_statement(void){
	// GPRINT [x,y],c,bc,s$
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		ARG_STRING<<ARG5 | 
		DISPLAY_GPRINT<<LIBOPTION);
}

int line_statement(void){
	// LINE [x1,y1],x2,y2[,c]
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_default_args[5]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		ARG_INTEGER_OPTIONAL<<ARG5 | 
		DISPLAY_LINE<<LIBOPTION);
}

int point_statement(void){
	// POINT x,y
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
		ARG_INTEGER<<ARG2 | 
		DISPLAY_POINT<<LIBOPTION);
}

int pset_statement(void){
	// PSET [x,y][,c]
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_default_args[3]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER_OPTIONAL<<ARG3 | 
		DISPLAY_PSET<<LIBOPTION);
}

int putbmp_callback(void){
	int e;
	e=set_value_in_register(0,0xf800f000);
	if (e) return e;
	// Embed bl instruction
	object-=2;
	e=goto_label();
	if (e) return e;
	// Get pc in r1 and call restore library
	check_object(1);
	(object++)[0]=0x4679; //      	mov	r1, pc
	return call_lib_code(LIB_RESTORE);
}


int putbmp_statement(void){
	// PUTBMP [x,y],m,n,bbb
	int e;
	unsigned char* sbefore=source;
	unsigned short* obefore=object;
	// bbb may be label
	g_default_args[1]=0x80000001;
	g_default_args[2]=0x80000002;
	g_callback_args[5]=putbmp_callback;
	e=argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		ARG_CALLBACK<<ARG5 | 
		DISPLAY_PUTBMP2<<LIBOPTION);
	if (0==e) return 0;
	// Rewind source and object
	source=sbefore;
	rewind_object(obefore);
	// bbb must be pointer
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		ARG_INTEGER<<ARG3 | 
		ARG_INTEGER<<ARG4 | 
		ARG_INTEGER<<ARG5 | 
		DISPLAY_PUTBMP<<LIBOPTION);
}

int usegraphic_statement(void){
	// USEGRAPHIC [x[,y]]
	g_default_args[1]=1;
	g_default_args[2]=255;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		DISPLAY_USEGRAPHIC<<LIBOPTION);
}

int tvram_function(void){
	// TVRAM([x])
	g_default_args[1]=-1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		DISPLAY_TVRAM<<LIBOPTION);
}

int gcolor_function(void){
	/// GCOLOR(x,y)
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 | 
		DISPLAY_GCOLOR_FUNC<<LIBOPTION);
}

int display_statements(void){
	if (instruction_is("BGCOLOR")) return bgcolor_statement();
	if (instruction_is("BOXFILL")) return boxfill_statement();
	if (instruction_is("CIRCLE")) return circle_statement();
	if (instruction_is("CIRCLEFILL")) return circlefill_statement();
	if (instruction_is("CLS")) return cls_statement();
	if (instruction_is("COLOR")) return color_statement();
	if (instruction_is("CURSOR")) return cursor_statement();
	if (instruction_is("GCLS")) return gcls_statement();
	if (instruction_is("GCOLOR")) return gcolor_statement();
	if (instruction_is("GPALETTE")) return gpalette_statement();
	if (instruction_is("GPRINT")) return gprint_statement();
	if (instruction_is("LINE")) return line_statement();
	if (instruction_is("PALETTE")) return palette_statement();
	if (instruction_is("PCG")) return pcg_statement();
	if (instruction_is("POINT")) return point_statement();
	if (instruction_is("PSET")) return pset_statement();
	if (instruction_is("PUTBMP")) return putbmp_statement();
	if (instruction_is("SCROLL")) return scroll_statement();
	if (instruction_is("USEGRAPHIC")) return usegraphic_statement();
	if (instruction_is("USEPCG")) return usepcg_statement();
	if (instruction_is("WIDTH")) return width_statement();
	return ERROR_STATEMENT_NOT_DETECTED;
}

int display_functions(void){
	if (instruction_is("TVRAM(")) return tvram_function();
	if (instruction_is("GCOLOR(")) return gcolor_function();
	return ERROR_SYNTAX;
}
