/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "./compiler.h"
#include "./api.h"
#include "./display.h"

/*
	Local prototypings
*/
int lib_cread(int r0, int r1, int r2);

/*

CLEAR
	すべての文字列型変数と整数型配列を破棄し、整数値を０とする。また、
	PCGの使用をやめ、表示キャラクターをリセットする。
*/

void display_init(void){
    // Enable SPI at 20 MHz and connect to GPIOs
	spi_init(LCD_SPICH, 20000 * 1000);
	gpio_set_function(LCD_SPI_RX, GPIO_FUNC_SPI);
	gpio_set_function(LCD_SPI_TX, GPIO_FUNC_SPI);
	gpio_set_function(LCD_SPI_SCK, GPIO_FUNC_SPI);
	
	gpio_init(LCD_CS);
	gpio_put(LCD_CS, 1);
	gpio_set_dir(LCD_CS, GPIO_OUT);
	gpio_init(LCD_DC);
	gpio_put(LCD_DC, 1);
	gpio_set_dir(LCD_DC, GPIO_OUT);
	gpio_init(LCD_RESET);
	gpio_put(LCD_RESET, 1);
	gpio_set_dir(LCD_RESET, GPIO_OUT);
	
	init_textgraph();
}

int lib_display(int r0, int r1, int r2){
	static unsigned char cursorcolor=7;
	static unsigned char gcolor=7;
	static unsigned char* ppcg=0;
	static int prevx1=0,prevy1=0;
	int* sp=(int*)r1;
	int i,j,gc;
	unsigned char* bmp;
	unsigned int x1,y1,x2,y2;
	// Set x1,y1,x2,y2 for graphic
	if (r1&0xfff0000) {
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
			//SCROLL x,y
			//	画面を横方向、もしくは縦方向(斜めも可)に動かす。動かす方向と大きさ
			//	は、x, yでそれぞれ、横方向の移動度、縦方向の移動度として指定する。
			// TODO: here
			break;
		case DISPLAY_WIDTH:
			//WIDTH x
			//	キャラクターディスプレイの横幅を文字数で指定。xは30、36、40、48、
			//	もしくは80。
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
			g_circle(x1,y1,x1,gc);
			prevx1=x1;
			prevy1=y1;
			break;
		case DISPLAY_CIRCLEFILL:
			//void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
			g_circlefill(x1,y1,x1,gc);
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
			prevx1=x1;
			prevy1=y1;
			g_printstr(x1,y1,x1,y1,(unsigned char*)r0);
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
			switch(r0&3){
				case 0:
					set_graphmode(0);
					break;
				case 2:
					// TODO: clear palette
					// TODO: clear graphic display
				case 1:
				default:
					set_graphmode(1);
					break;
			}
			break;
		case DISPLAY_GCOLOR_FUNC:
			//GCOLOR(x,y)
			// TODO: here
			break;
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
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER<<ARG1 | 
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
	// USEGRAPHIC [x]
	g_default_args[1]=1;
	return argn_function(LIB_DISPLAY_FUNCTION,
		ARG_INTEGER_OPTIONAL<<ARG1 | 
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
	return ERROR_SYNTAX;
}

int display_functions(void){
	if (instruction_is("TVRAM(")) return tvram_function();
	if (instruction_is("GCOLOR(")) return gcolor_function();
	return ERROR_SYNTAX;
}
