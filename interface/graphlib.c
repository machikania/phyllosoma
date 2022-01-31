//LCDãE­ã¹ããEã°ã©ãã£ãE¯ã©ã¤ãã©ãª

#include "graphlib.h"
#include "LCDdriver.h"

unsigned char TVRAM[ATTROFFSET*2+1] __attribute__ ((aligned (4)));
unsigned char *fontp; //ãã©ã³ãæ ¼ç´ã¢ãã¬ã¹ãåEæåæãEFontDataãRAMæE®ãããã¨ã§PCGãå®ç¾
unsigned int bgcolor; // ããã¯ã°ã©ã³ãã«ã©ã¼
unsigned char twidth; //ãE­ã¹ãEè¡æå­æ°
unsigned char *cursor;
unsigned char cursorcolor;
unsigned short palette[256];
int WIDTH_X; // æ¨ªæ¹åæå­æ°
int WIDTH_Y; // ç¸¦æ¹åæå­æ°

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g){
//ãE­ã¹ãï¼ã°ã©ãã£ãE¯å±ç¨ã«ã©ã¼ãã¬ãEè¨­å®E
	palette[n]=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

void g_pset(int x,int y,unsigned char c)
// (x,y)ã®ä½ç½®ã«ã«ã©ã¼ãã¬ãEçªå·cã§ç¹ãæç»
{
	if(x>=0 && x<X_RES && y>=0 && y<Y_RES)
		drawPixel(x,y,palette[c]);
}

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[])
// æ¨ªm*ç¸¦nããããEã­ã£ã©ã¯ã¿ã¼ãåº§æ¨x,yã«è¡¨ç¤º
// unsigned char bmp[m*n]éåEã«ãåç´ã«ã«ã©ã¼çªå·ãä¸¦ã¹ãE
// ã«ã©ã¼çªå·ãEã®é¨åEEéæEè²ã¨ãã¦æ±ãE
{
	int i,j;
	int skip,outflag;
	const unsigned char *p;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //ç»é¢å¤E
	outflag=0;

	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //ç»é¢ä¸é¨ã«åEãå ´åE
			i=0;
			p=bmp-y*m;
		}
		else{
			i=y;
			p=bmp;
		}
		for(;i<y+n;i++){
			if(i>=Y_RES) return; //ç»é¢ä¸é¨ã«åEãå ´åE
			if(x<0){ //ç»é¢å·¦ã«åEãå ´åãEæ®ãé¨åEEã¿æç»
				j=0;
				p+=-x;
			}
			else{
				j=x;
			}
			skip=1;
			for(;j<x+m;j++){
				if(j>=X_RES){ //ç»é¢å³ã«åEãå ´åE
					p+=x+m-j;
					break;
				}
				if(*p!=0){ //ã«ã©ã¼çªå·ãEã®å ´åãEæEã¨ãã¦å¦çE
					if(skip){
						if(outflag) checkSPIfinish();
						LCD_setAddrWindow(j,i,m,n);
						skip=0;
					}
					LCD_WriteData2_notfinish(palette[*p]);
					outflag=1;
				}
				else skip=1;
				p++;
			}
		}
		if(outflag) checkSPIfinish();

	}
	else{
		// horizontal case
		if(x<0){ //ç»é¢å·¦é¨ã«åEãå ´åE
			j=0;
			p=bmp-x;
		}
		else{
			j=x;
			p=bmp;
		}
		for(;j<x+m;j++){
			if(j>=X_RES) return; //ç»é¢å³é¨ã«åEãå ´åE
			if(y<0){ //ç»é¢ä¸ã«åEãå ´åãEæ®ãé¨åEEã¿æç»
				i=0;
				p+=-y*m;
			}
			else{
				i=y;
			}
			skip=1;
			for(;i<y+n;i++){
				if(i>=Y_RES){ //ç»é¢ä¸ã«åEãå ´åE
					p+=(y+n-i)*m;
					break;
				}
				if(*p!=0){ //ã«ã©ã¼çªå·ãEã®å ´åãEæEã¨ãã¦å¦çE
					if(skip){
						if(outflag) checkSPIfinish();
						LCD_setAddrWindow(j,i,m,n);
						skip=0;
					}
					LCD_WriteData2_notfinish(palette[*p]);
					outflag=1;
				}
				else skip=1;
				p+=m;
			}
			p-=m*n-1;
		}
		if(outflag) checkSPIfinish();
	}
}

// ç¸¦m*æ¨ªnããããEã­ã£ã©ã¯ã¿ã¼æ¶å»
// ã«ã©ã¼0ã§å¡ãã¤ã¶ãE
void g_clrbmpmn(int x,int y,unsigned char m,unsigned char n)
{
	int i,j,k,dx,dy;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //ç»é¢å¤E
	if(y<0){ //ç»é¢ä¸é¨ã«åEãå ´åE
		i=0;
		dy=n+y;
	}
	else{
 		i=y;
		dy=n;
	}
	if(x<0) { //ç»é¢å·¦ã«åEãå ´åãEæ®ãé¨åEEã¿æç»
		j=0;
		dx=m+x;
	}
	else {
		j=x;
		dx=m;
	}
	if(x+m>X_RES) { //ç»é¢å³ã«åEãå ´åE
		dx-=(x+m)-X_RES;
	}
	if(y+n>Y_RES) { //ç»é¢ä¸ã«åEãå ´åE
		dy-=(y+n)-Y_RES;
	}
	LCD_setAddrWindow(j,i,dx,dy);
	dx*=dy;
	for(i=0;i<dx;i++) LCD_WriteData2_notfinish(0);
	checkSPIfinish();
}

void g_gline(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1)-(x2,y2)ã«ã«ã©ã¼ãã¬ãEçªå·cã§ç·åEãæç»
{
	int sx,sy,dx,dy,i;
	int e;

	if(x2>x1){
		dx=x2-x1;
		sx=1;
	}
	else{
		dx=x1-x2;
		sx=-1;
	}
	if(y2>y1){
		dy=y2-y1;
		sy=1;
	}
	else{
		dy=y1-y2;
		sy=-1;
	}
	if(dx>=dy){
		e=-dx;
		for(i=0;i<=dx;i++){
			g_pset(x1,y1,c);
			x1+=sx;
			e+=dy*2;
			if(e>=0){
				y1+=sy;
				e-=dx*2;
			}
		}
	}
	else{
		e=-dy;
		for(i=0;i<=dy;i++){
			g_pset(x1,y1,c);
			y1+=sy;
			e+=dx*2;
			if(e>=0){
				x1+=sx;
				e-=dy*2;
			}
		}
	}
}
void g_hline(int x1,int x2,int y,unsigned char c)
// (x1,y)-(x2,y)ã¸ã®æ°´å¹³ã©ã¤ã³ãé«éæç»
{
	int temp;

	if(y<0 || y>=Y_RES) return;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=X_RES) return;
	if(x1<0) x1=0;
	if(x2>=X_RES) x2=X_RES-1;
	LCD_continuous_output(x1,y,palette[c],x2-x1+1);
}

void g_circle(int x0,int y0,unsigned int r,unsigned char c)
// (x0,y0)ãä¸­å¿E«ãåå¾rãã«ã©ã¼ãã¬ãEçªå·cã®åEæç»
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		g_pset(x0-x,y0-y,c);
		g_pset(x0-x,y0+y,c);
		g_pset(x0+x,y0-y,c);
		g_pset(x0+x,y0+y,c);
		g_pset(x0-y,y0-x,c);
		g_pset(x0-y,y0+x,c);
		g_pset(x0+y,y0-x,c);
		g_pset(x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}
void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1),(x2,y2)ãå¯¾è§ç·ã¨ããã«ã©ã¼ãã¬ãEçªå·cã§å¡ãããé·æ¹å½¢ãæç»
{
	int temp;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=X_RES) return;
	if(y1>y2){
		temp=y1;
		y1=y2;
		y2=temp;
	}
	if(y2<0 || y1>=Y_RES) return;
	if(y1<0) y1=0;
	if(y2>=Y_RES) y2=Y_RES-1;
	while(y1<=y2){
		g_hline(x1,x2,y1++,c);
	}
}
void g_circlefill(int x0,int y0,unsigned int r,unsigned char c)
// (x0,y0)ãä¸­å¿E«ãåå¾rãã«ã©ã¼ãã¬ãEçªå·cã§å¡ãããåEæç»
{
	int x,y,f;
	x=r;
	y=0;
	f=-2*r+3;
	while(x>=y){
		g_hline(x0-x,x0+x,y0-y,c);
		g_hline(x0-x,x0+x,y0+y,c);
		g_hline(x0-y,x0+y,y0-x,c);
		g_hline(x0-y,x0+y,y0+x,c);
		if(f>=0){
			x--;
			f-=x*4;
		}
		y++;
		f+=y*4+2;
	}
}
void g_putfont(int x,int y,unsigned char c,int bc,unsigned char n)
//8*8ããããEã¢ã«ãã¡ããããã©ã³ãè¡¨ç¤º
//åº§æ¨Ex,y)ãã«ã©ã¼ãã¬ãEçªå·c
//bc:ããã¯ã°ã©ã³ãã«ã©ã¼ãè² æ°ã®å ´åç¡è¦E
//n:æE­çªå·
{
	int i,j,dx,dy;
	unsigned char d,b;
	int skip,outflag;
	unsigned short c1;
	const unsigned char *p;
	if(x<=-8 || x>=X_RES || y<=-8 || y>=Y_RES) return; //ç»é¢å¤E
	c1=palette[c];
	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //ç»é¢ä¸é¨ã«åEãå ´åE
			i=0;
			p=fontp+n*8-y;
			dy=8+y;
		}
		else{
			i=y;
			p=fontp+n*8;
			if(y+8<=Y_RES) dy=8;
			else dy=Y_RES-y;
		}
		if(x<0){ //ç»é¢å·¦ã«åEãå ´åE
			j=0;
			dx=8+x;
		}
		else{
			j=x;
			if(x+8<=X_RES) dx=8;
			else dx=X_RES-x;
		}
		if(bc>=0) LCD_setAddrWindow(j,i,dx,dy);
		for(;i<y+8;i++){
			if(i>=Y_RES) break; //ç»é¢ä¸é¨ã«åEãå ´åE
			d=*p++;
			if(x<0){ //ç»é¢å·¦ã«åEãå ´åãEæ®ãé¨åEEã¿æç»
				j=0;
				d<<=-x;
			}
			else{
				j=x;
			}
			if(bc<0){
				skip=1;
				outflag=0;
				for(;j<x+8;j++){
					if(j>=X_RES){ //ç»é¢å³ã«åEãå ´åE
						break;
					}
					if(d&0x80){
						if(skip){
							if(outflag) checkSPIfinish();
							LCD_setAddrWindow(j,i,8,8);
							skip=0;
						}
						LCD_WriteData2_notfinish(c1);
						outflag=1;
					}
					else skip=1;
					d<<=1;
				}
				if(outflag) checkSPIfinish();
			}
			else{
				for(;j<x+8;j++){
					if(j>=X_RES){ //ç»é¢å³ã«åEãå ´åE
						break;
					}
					if(d&0x80){
						LCD_WriteData2_notfinish(c1);
					}
					else{
						LCD_WriteData2_notfinish(bc);
					}
					d<<=1;
				}
				checkSPIfinish();
			}
		}
	}
	else{
		// horizontal case
		if(x<0){ //ç»é¢å·¦ã«åEãå ´åE
			j=0;
			b=0x80>>(-x);
			dx=8+x;
		}
		else{
			j=x;
			b=0x80;
			if(x+8<=X_RES) dx=8;
			else dx=X_RES-x;
		}
		if(y<0){
			i=0;
			dy=8+y;
		}
		else{
			i=y;
			if(y+8<=Y_RES) dy=8;
			else dy=Y_RES-y;
		}
		if(bc>=0) LCD_setAddrWindow(j,i,dx,dy);
		for(;j<x+8;j++){
			if(j>=X_RES) break; //ç»é¢å³ã«åEãå ´åE
			if(y<0){ //ç»é¢ä¸é¨ã«åEãå ´åE
				i=0;
				p=fontp+n*8-y;
			}
			else{
				i=y;
				p=fontp+n*8;
			}
			if(bc<0){
				skip=1;
				outflag=0;
				for(;i<y+8;i++){
					if(i>=Y_RES){ //ç»é¢ä¸ã«åEãå ´åE
						break;
					}
					if(*p++ & b){
						if(skip){
							if(outflag) checkSPIfinish();
							LCD_setAddrWindow(j,i,8,8);
							skip=0;
						}
						LCD_WriteData2_notfinish(c1);
						outflag=1;
					}
					else skip=1;
				}
				if(outflag) checkSPIfinish();
			}
			else{
				for(;i<y+8;i++){
					if(i>=Y_RES){ //ç»é¢ä¸ã«åEãå ´åE
						break;
					}
					if(*p++ & b){
						LCD_WriteData2_notfinish(c1);
					}
					else{
						LCD_WriteData2_notfinish(bc);
					}
				}
			}
			b>>=1;
		}
		if(bc>=0) checkSPIfinish();
	}
}

void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s){
	//åº§æ¨Ex,y)ããã«ã©ã¼ãã¬ãEçªå·cã§æE­åEsãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼
	//bcãè² ã®å ´åãEç¡è¦E
	while(*s){
		g_putfont(x,y,c,bc,*s++);
		x+=8;
	}
}
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n){
	//åº§æ¨Ex,y)ã«ã«ã©ã¼çªå·cã§æ°å¤nãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼
	unsigned int d,e;
	d=10;
	e=0;
	while(n>=d){
		e++;
		if(e==9) break;
		d*=10;
	}	
	x+=e*8;
	do{
		g_putfont(x,y,c,bc,'0'+n%10);
		n/=10;
		x-=8;
	}while(n!=0);
}
void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e){
	//åº§æ¨Ex,y)ã«ã«ã©ã¼çªå·cã§æ°å¤nãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼ãeæ¡ã§è¡¨ç¤º
	if(e==0) return;
	x+=(e-1)*8;
	do{
		g_putfont(x,y,c,bc,'0'+n%10);
		e--;
		n/=10;
		x-=8;
	}while(e!=0 && n!=0);
	while(e!=0){
		g_putfont(x,y,c,bc,' ');
		x-=8;
		e--;
	}
}
unsigned int g_color(int x,int y){
//åº§æ¨Ex,y)ã®è²æE ±ãè¿ããç»é¢å¤ãE0ãè¿ã
//ãã¬ãEçªå·ã§ã¯ãªãEã¨ã«æ³¨æE
	if(x<0 || x>=X_RES || y<0 || y>=Y_RES) return 0;
	return getColor(x,y);
}

// ãE­ã¹ãç»é¢ã¯ãªã¢
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<ATTROFFSET*2/4;i++) *vp++=0;
	cursor=TVRAM;
	LCD_Clear(bgcolor);
}

// ã°ã©ãã£ãE¯ç»é¢ã¯ãªã¢
void g_clearscreen(void)
{
	LCD_Clear(0);
}

void textredraw(void){
// ãE­ã¹ãç»é¢åæç»
// ãE­ã¹ãERAMã®åE®¹ã«ãããã£ã¦æ¶²æ¶ã«åºåE
	int i,j,x,y;
	unsigned short c;
	unsigned char d,b,*p,*p2;

	LCD_setAddrWindow(0,0,X_RES,Y_RES);
	p=TVRAM;

	if(LCD_ALIGNMENT == VERTICAL){
		for(y=0;y<WIDTH_Y;y++){
			for(i=0;i<8;i++){
				for(x=0;x<WIDTH_X;x++){
					c=palette[*(p+ATTROFFSET)];
					d=*(fontp+(*p++)*8+i);
					for(j=0;j<8;j++){
						if(d & 0x80) LCD_WriteData2_notfinish(c);
						else LCD_WriteData2_notfinish(bgcolor);
						d<<=1;
					}
				}
				p-=WIDTH_X;
			}
			p+=WIDTH_X;
		}
	}
	else{
		// Horizontal case
		for(x=0;x<WIDTH_X;x++){
			b=0x80;
			for(j=0;j<8;j++){
				for(y=0;y<WIDTH_Y;y++){
					p2=fontp+(*p)*8;
					c=palette[*(p+ATTROFFSET)];
					for(i=0;i<8;i++){
						if(*p2++ & b) LCD_WriteData2_notfinish(c);
						else LCD_WriteData2_notfinish(bgcolor);
					}
					p+=WIDTH_X;
				}
				p-=ATTROFFSET;
				b>>=1;
			}
			p++;
		}
	}
	checkSPIfinish();
}

void vramscroll(void){
	unsigned short *p1,*p2;

	p1=(unsigned short *)TVRAM;
	p2=(unsigned short *)(TVRAM+WIDTH_X);
	while(p2<(unsigned short *)(TVRAM+ATTROFFSET)){
		*(p1+ATTROFFSET/2)=*(p2+ATTROFFSET/2);
		*p1++=*p2++;
	}
	while(p1<(unsigned short *)(TVRAM+ATTROFFSET)){
		*(p1+ATTROFFSET/2)=0;
		*p1++=0;
	}
	textredraw();
}
void vramscrolldown(void){
	unsigned short *p1,*p2;

	p1=(unsigned short *)(TVRAM+ATTROFFSET)-1;
	p2=(unsigned short *)(TVRAM+ATTROFFSET-WIDTH_X)-1;
	while(p2>=(unsigned short *)(TVRAM)){
		*(p1+ATTROFFSET/2)=*(p2+ATTROFFSET/2);
		*p1++=*p2++;
	}
	while(p1>=(unsigned short *)(TVRAM)){
		*(p1+ATTROFFSET/2)=0;
		*p1++=0;
	}
	textredraw();
}
void setcursor(unsigned char x,unsigned char y,unsigned char c){
	//ã«ã¼ã½ã«ãåº§æ¨Ex,y)ã«ã«ã©ã¼çªå·cã«è¨­å®E
	if(x>=WIDTH_X || y>=WIDTH_Y) return;
	cursor=TVRAM+y*WIDTH_X+x;
	cursorcolor=c;
}
void setcursorcolor(unsigned char c){
	//ã«ã¼ã½ã«ä½ç½®ããEã¾ã¾ã§ã«ã©ã¼çªå·ãcã«è¨­å®E
	cursorcolor=c;
}
void printchar(unsigned char n){
	//ã«ã¼ã½ã«ä½ç½®ã«ãE­ã¹ãã³ã¼ãnãEæE­è¡¨ç¤ºããã«ã¼ã½ã«ãEæE­é²ãã
	//ç»é¢æçµæå­è¡¨ç¤ºãã¦ãã¹ã¯ã­ã¼ã«ãããæ¬¡ã®æE­è¡¨ç¤ºæã«ã¹ã¯ã­ã¼ã«ãã
	if(cursor<TVRAM || cursor>TVRAM+ATTROFFSET) return;
	if(cursor==TVRAM+ATTROFFSET){
		vramscroll();
		cursor-=WIDTH_X;
	}
	if(n=='\n'){
		//æ¹è¡E
		cursor+=WIDTH_X-((cursor-TVRAM)%WIDTH_X);
	} else{
		*cursor=n;
		*(cursor+ATTROFFSET)=cursorcolor;
		g_putfont(((cursor-TVRAM)%WIDTH_X)*8,((cursor-TVRAM)/WIDTH_X)*8,cursorcolor,bgcolor,n);
		cursor++;
	}
}
void printstr(unsigned char *s){
	//ã«ã¼ã½ã«ä½ç½®ã«æE­åEsãè¡¨ç¤º
	while(*s){
		printchar(*s++);
	}
}
void printnum(unsigned int n){
	//ã«ã¼ã½ã«ä½ç½®ã«ç¬¦å·ãªãæ´æ°nãE0é²æ°è¡¨ç¤º
	unsigned int d,n1;
	n1=n/10;
	d=1;
	while(n1>=d){
		d*=10;
	}
	while(d!=0){
		printchar('0'+n/d);
		n%=d;
		d/=10;
	}
}
void printnum2(unsigned int n,unsigned char e){
	//ã«ã¼ã½ã«ä½ç½®ã«ç¬¦å·ãªãæ´æ°nãeæ¡ãE10é²æ°è¡¨ç¤ºEåã®ç©ºãæ¡E¨åEEã¹ããEã¹ã§åããï¼E
	unsigned int d,n1;
	if(e==0) return;
	n1=n/10;
	d=1;
	e--;
	while(e>0 && n1>=d){
		d*=10;
		e--;
	}
	if(e==0 && n1>d) n%=d*10;
	for(;e>0;e--) printchar(' ');
	while(d!=0){
		printchar('0'+n/d);
		n%=d;
		d/=10;
	}
}
void cls(void){
	//ç»é¢æ¶å»ãã«ã¼ã½ã«ãåEé ­ã«ç§»åE
	clearscreen();
}
void startPCG(unsigned char *p,int a){
// RAMãã©ã³ãï¼ECGEãEå©ç¨éå§E
// pERAMãã©ã³ããEæ ¼ç´ã¢ãã¬ã¹EE*256EE048ãã¤ãï¼E
// aEEã·ã¹ãE ãã©ã³ããããEã³ããEæE®ãEã®å ´åã³ããEãªããEä»¥å¤ã§ã³ããE
	int i;
	if(a){
		for(i=0;i<8*256;i++) *p++=FontData[i];
		fontp=p-8*256;
	}
	else fontp=p;
}
void stopPCG(void){
// RAMãã©ã³ãï¼ECGEãEå©ç¨åæ­¢
	fontp=(unsigned char *)FontData;
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	bgcolor=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
	textredraw();
}
void init_textgraph(unsigned char align){
	//ãE­ã¹ããEã°ã©ãã£ãE¯LCDã©ã¤ãã©ãªã®ä½¿ç¨éå§E
	//ãã¬ãEè¨­å®E
	//LCDç¸¦æ¨ªè¨­å®E

	int i;
	fontp=(unsigned char *)FontData;
	bgcolor=0; //ããã¯ã°ã©ã³ãã«ã©ã¼ã¯é»E
	//ã«ã©ã¼ãã¬ãEåæåE
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
	for(i=16;i<256;i++){
		set_palette(i,255,255,255);
	}
	setcursorcolor(7);
	LCD_Init();
	set_lcdalign(align);
}
void set_lcdalign(unsigned char align){
	// æ¶²æ¶ã®ç¸¦æ¨ªè¨­å®E
	LCD_ALIGNMENT=align;
	LCD_WriteComm(0x36);
	if(align==VERTICAL){
		LCD_WriteData(0x48);
		X_RES=LCD_COLUMN_RES;
		Y_RES=LCD_ROW_RES;
		WIDTH_X=LCD_COLUMN_RES/8;
		WIDTH_Y=LCD_ROW_RES/8;
	}
	else{
		LCD_WriteData(0x0C);
		X_RES=LCD_ROW_RES;
		Y_RES=LCD_COLUMN_RES;
		WIDTH_X=LCD_ROW_RES/8;
		WIDTH_Y=LCD_COLUMN_RES/8;
	}
	clearscreen();
}
