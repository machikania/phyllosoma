/*----------------------------------------------------------------------------

Copyright (C) 2024, KenKen, all right reserved.

This program supplied herewith by KenKen is free software; you can
redistribute it and/or modify it under the terms of the same license written
here and only for non-commercial purpose.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of FITNESS FOR A PARTICULAR
PURPOSE. The copyright owner and contributors are NOT LIABLE for any damages
caused by using this program.

----------------------------------------------------------------------------*/

//　テキスト＋グラフィックビデオ出力用ライブラリ

#include "rp2040_pwm_ntsc_textgraph.h"
#include "../compiler.h"

// x,yにカラー番号cのドットを描画
void g_pset(int x, int y, unsigned char c)
{
	if((unsigned int)x>=X_RES) return;
	if((unsigned int)y>=Y_RES) return;
	GVRAM[y*X_RES+x]=c;
}

// 横m*縦nドットのキャラクターを座標x,yに表示
// unsigned char bmp[m*n]配列に、単純にカラー番号を並べる
// カラー番号が0の部分は透明色として扱う
void g_putbmpmn(int x,int y,unsigned short m,unsigned short n,const unsigned char bmp[]){
	int i,j,k;
	unsigned char *vp;
	const unsigned char *p;
	unsigned short *vph;

	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //画面外
	if(y<0){ //画面上部に切れる場合
		i=0;
		p=bmp-y*m;
	}
	else{
		i=y;
		p=bmp;
	}
	for(;i<y+n;i++){
		if(i>=Y_RES) return; //画面下部に切れる場合
		if(x<0){ //画面左に切れる場合は残る部分のみ描画
			j=0;
			p+=-x;
			vp=GVRAM+i*X_RES;
		}
		else{
			j=x;
			vp=GVRAM+i*X_RES+x;
		}
		for(;j<x+m;j++){
			if(j>=X_RES){ //画面右に切れる場合
				p+=x+m-j;
				break;
			}
			if(*p!=0){ //カラー番号が0の場合、透明として処理
				*vp=*p;
			}
			p++;
			vp++;
		}
	}
}

// 縦m*横nドットのキャラクター消去
// カラー0で塗りつぶし
void g_clrbmpmn(int x,int y,unsigned short m,unsigned short n)
{
	int i,j,k;
	unsigned char *vp;
	unsigned short mask,*vph;

	if(x<=-m || x>=X_RES || y<=-n || y>=Y_RES) return; //画面外
	if(y<0){ //画面上部に切れる場合
		i=0;
	}
	else{
		i=y;
	}
	for(;i<y+n;i++){
		if(i>=Y_RES) return; //画面下部に切れる場合
		if(x<0){ //画面左に切れる場合は残る部分のみ描画
			j=0;
			vp=GVRAM+i*X_RES;
		}
		else{
			j=x;
			vp=GVRAM+i*X_RES+x;
		}
		for(;j<x+m;j++){
			if(j>=X_RES){ //画面右に切れる場合
				break;
			}
			*vp++=0;
		}
	}
}

// (x1,y1)-(x2,y2)にカラーcで線分を描画
void g_gline(int x1,int y1,int x2,int y2,unsigned char c)
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

// (x0,y0)を中心に、半径r、カラーcの円を描画
void g_circle(int x0,int y0,unsigned int r,unsigned char c)
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

// (x1,y)-(x2,y)の水平ラインをカラーcで高速描画
void g_hline(int x1,int x2,int y,unsigned char c)
{
	int temp;
	unsigned int d,*ad;
	unsigned short dh,*adh;

	if(y<0 || y>=Y_RES) return;
	if(x1>x2){
		temp=x1;
		x1=x2;
		x2=temp;
	}
	if(x2<0 || x1>=X_RES) return;
	if(x1<0) x1=0;
	if(x2>=X_RES) x2=X_RES-1;
	while(x1&3){
		g_pset(x1++,y,c);
		if(x1>x2) return;
	}
	d=c|(c<<8)|(c<<16)|(c<<24);
	ad=(unsigned int *)(GVRAM+y*X_RES+x1);
	while(x1+3<=x2){
		*ad++=d;
		x1+=4;
	}
	while(x1<=x2) g_pset(x1++,y,c);
}

// (x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画
void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c)
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

// (x0,y0)を中心に、半径r、カラーcで塗られた円を描画
void g_circlefill(int x0,int y0,unsigned int r,unsigned char c)
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

//8*8ドットのアルファベットフォント表示
//座標（x,y)、カラー番号c
//bc:バックグランドカラー、負数の場合無視
//n:文字番号
void g_putfont(int x,int y,unsigned char c,int bc,unsigned char n)
{
	int i,j,k;
	unsigned char d;
	const unsigned char *p;
	unsigned int d1,mask;
	unsigned short *ad;

	p=fontp+n*8;
	for(i=0;i<8;i++){
		d=*p++;
		for(j=0;j<8;j++){
			if(d&0x80) g_pset(x+j,y+i,c);
			else if(bc>=0) g_pset(x+j,y+i,bc);
			d<<=1;
		}
	}
}

//座標(x,y)からカラー番号cで文字列sを表示、bc:バックグランドカラー
void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s){
	while(*s){
		g_putfont(x,y,c,bc,*s++);
		x+=8;
	}
}

//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n){
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

//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示
void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e){
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

//座標(x,y)のVRAM上の現在のパレット番号を返す、画面外は0を返す
unsigned int g_color(int x,int y){
	unsigned short *ad;

	if((unsigned int)x>=(unsigned int)X_RES) return 0;
	if((unsigned int)y>=(unsigned int)Y_RES) return 0;
	return *(GVRAM+y*X_RES+x);
}

void windowscroll(int y1,int y2){
	// scroll up text bitween line y1 and y2
	unsigned char *p1,*p2,*vramend;

	vramend=TVRAM+WIDTH_X*(y2+1);
	p1=TVRAM+WIDTH_X*y1;
	p2=p1+WIDTH_X;
	while(p2<vramend){
		*(p1+attroffset)=*(p2+attroffset);
		*p1++=*p2++;
	}
	while(p1<vramend){
		*(p1+attroffset)=0;
		*p1++=0;
	}
}
void vramscroll(void){
	unsigned int *p1,*p2;
	unsigned short *hp1,*hp2;

	if(WIDTH_X%4==0){
		p1=(unsigned int *)TVRAM;
		p2=(unsigned int *)(TVRAM+WIDTH_X);
		while(p2<(unsigned int *)(TVRAM+attroffset)){
			*(p1+attroffset/4)=*(p2+attroffset/4);
			*p1++=*p2++;
		}
		while(p1<(unsigned int *)(TVRAM+attroffset)){
			*(p1+attroffset/4)=0;
			*p1++=0;
		}
	}
	else{
		hp1=(unsigned short *)TVRAM;
		hp2=(unsigned short *)(TVRAM+WIDTH_X);
		while(hp2<(unsigned short *)(TVRAM+attroffset)){
			*(hp1+attroffset/2)=*(hp2+attroffset/2);
			*hp1++=*hp2++;
		}
		while(hp1<(unsigned short *)(TVRAM+attroffset)){
			*(hp1+attroffset/2)=0;
			*hp1++=0;
		}
	}
}

//カーソルを座標(x,y)にカラー番号cに設定
void setcursor(unsigned char x,unsigned char y,unsigned char c){
	//カーソルを座標(x,y)にカラー番号cに設定
	if(x>=WIDTH_X || y>=WIDTH_Y) return;
	cursor=TVRAM+y*WIDTH_X+x;
	cursorcolor=c;
}

//カーソル位置そのままでカラー番号をcに設定
void setcursorcolor(unsigned char c){
	cursorcolor=c;
}

//カーソル位置にテキストコードnを1文字表示し、カーソルを1文字進める
//画面最終文字表示してもスクロールせず、次の文字表示時にスクロールする
void printchar(unsigned char n){
	if(cursor<TVRAM || cursor>TVRAM+WIDTH_X*WIDTH_Y) return;
	if(cursor==TVRAM+WIDTH_X*WIDTH_Y){
		vramscroll();
		cursor=TVRAM+WIDTH_X*(WIDTH_Y-1);
	}
	if(n=='\n'){
		//改行
		cursor+=WIDTH_X-((cursor-TVRAM)%WIDTH_X);
	} else if(n==0x08){
		//BS
		if (TVRAM<cursor) cursor--;
	} else{
		*cursor=n;
		*(cursor+attroffset)=cursorcolor;
		cursor++;
	}
}

//カーソル位置に文字列sを表示
void printstr(unsigned char *s){
	while(*s){
		printchar(*s++);
	}
}

//カーソル位置に符号なし整数nを10進数表示
void printnum(unsigned int n){
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

//カーソル位置に符号なし整数nをe桁の10進数表示（前の空き桁部分はスペースで埋める）
void printnum2(unsigned int n,unsigned char e){
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

//テキスト画面を0でクリアし、カーソルを画面先頭に移動
void cls(void){
	clearscreen();
}

// RAMフォント（PCG）の利用開始
// p：RAMフォントの格納アドレス（8*256＝2048バイト）
// a： システムフォントからのコピー指定。0の場合コピーなし、0以外でコピー
void startPCG(unsigned char *p,int a){
	int i;
	if(a){
		for(i=0;i<8*256;i++) *p++=FontData[i];
		fontp=p-8*256;
	}
	else fontp=p;
}
void stopPCG(void){
// RAMフォント（PCG）の利用停止
	fontp=(uint8_t*)FontData;
}

// General function for video

void video_init(void){
	set_videomode(VMODE_WIDETEXT,0);
	init_palette();
}

void lcd_spi_init(void){
	// SPI is not used for NTSC video
	// Instead, change some settings for 157.5 and 315 MHz CPU speeds.
	switch(g_clock_hz){
		case 157500000:
			// Normal mode NTSC video
			ntsc_changeclock(1);
			break;
		case 315000000:
			// Double speed mode NTSC video
			ntsc_changeclock(2);
			break;
		default:
			break;
	}
}

// Dummy functions (not required for NTSC)
void putcursorchar(void){}
void textredraw(void){}
void set_lcdalign(unsigned char align){}
int LCD_ALIGNMENT=HORIZONTAL; // VERTICAL, HORIZONTAL, VERTICAL&LCD180TURN, or HORIZONTAL&LCD180TURN
