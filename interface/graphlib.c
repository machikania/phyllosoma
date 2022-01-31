//LCDチE��スト�EグラフィチE��ライブラリ

#include "graphlib.h"
#include "LCDdriver.h"

unsigned char TVRAM[ATTROFFSET*2+1] __attribute__ ((aligned (4)));
unsigned char *fontp; //フォント格納アドレス、�E期化時�EFontData、RAM持E��することでPCGを実現
unsigned int bgcolor; // バックグランドカラー
unsigned char twidth; //チE��スチE行文字数
unsigned char *cursor;
unsigned char cursorcolor;
unsigned short palette[256];
int WIDTH_X; // 横方向文字数
int WIDTH_Y; // 縦方向文字数

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g){
//チE��スト／グラフィチE��共用カラーパレチE��設宁E
	palette[n]=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

void g_pset(int x,int y,unsigned char c)
// (x,y)の位置にカラーパレチE��番号cで点を描画
{
	if(x>=0 && x<X_RES && y>=0 && y<Y_RES)
		drawPixel(x,y,palette[c]);
}

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[])
// 横m*縦nドット�Eキャラクターを座標x,yに表示
// unsigned char bmp[m*n]配�Eに、単純にカラー番号を並べめE
// カラー番号ぁEの部刁E�E透�E色として扱ぁE
{
	int i,j;
	int skip,outflag;
	const unsigned char *p;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //画面夁E
	outflag=0;

	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //画面上部に刁E��る場吁E
			i=0;
			p=bmp-y*m;
		}
		else{
			i=y;
			p=bmp;
		}
		for(;i<y+n;i++){
			if(i>=Y_RES) return; //画面下部に刁E��る場吁E
			if(x<0){ //画面左に刁E��る場合�E残る部刁E�Eみ描画
				j=0;
				p+=-x;
			}
			else{
				j=x;
			}
			skip=1;
			for(;j<x+m;j++){
				if(j>=X_RES){ //画面右に刁E��る場吁E
					p+=x+m-j;
					break;
				}
				if(*p!=0){ //カラー番号ぁEの場合、E���Eとして処琁E
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
		if(x<0){ //画面左部に刁E��る場吁E
			j=0;
			p=bmp-x;
		}
		else{
			j=x;
			p=bmp;
		}
		for(;j<x+m;j++){
			if(j>=X_RES) return; //画面右部に刁E��る場吁E
			if(y<0){ //画面上に刁E��る場合�E残る部刁E�Eみ描画
				i=0;
				p+=-y*m;
			}
			else{
				i=y;
			}
			skip=1;
			for(;i<y+n;i++){
				if(i>=Y_RES){ //画面下に刁E��る場吁E
					p+=(y+n-i)*m;
					break;
				}
				if(*p!=0){ //カラー番号ぁEの場合、E���Eとして処琁E
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

// 縦m*横nドット�Eキャラクター消去
// カラー0で塗りつぶぁE
void g_clrbmpmn(int x,int y,unsigned char m,unsigned char n)
{
	int i,j,k,dx,dy;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //画面夁E
	if(y<0){ //画面上部に刁E��る場吁E
		i=0;
		dy=n+y;
	}
	else{
 		i=y;
		dy=n;
	}
	if(x<0) { //画面左に刁E��る場合�E残る部刁E�Eみ描画
		j=0;
		dx=m+x;
	}
	else {
		j=x;
		dx=m;
	}
	if(x+m>X_RES) { //画面右に刁E��る場吁E
		dx-=(x+m)-X_RES;
	}
	if(y+n>Y_RES) { //画面下に刁E��る場吁E
		dy-=(y+n)-Y_RES;
	}
	LCD_setAddrWindow(j,i,dx,dy);
	dx*=dy;
	for(i=0;i<dx;i++) LCD_WriteData2_notfinish(0);
	checkSPIfinish();
}

void g_gline(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1)-(x2,y2)にカラーパレチE��番号cで線�Eを描画
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
// (x1,y)-(x2,y)への水平ラインを高速描画
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
// (x0,y0)を中忁E��、半径r、カラーパレチE��番号cの冁E��描画
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
// (x1,y1),(x2,y2)を対角線とするカラーパレチE��番号cで塗られた長方形を描画
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
// (x0,y0)を中忁E��、半径r、カラーパレチE��番号cで塗られた冁E��描画
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
//8*8ドット�Eアルファベットフォント表示
//座樁Ex,y)、カラーパレチE��番号c
//bc:バックグランドカラー、負数の場合無要E
//n:斁E��番号
{
	int i,j,dx,dy;
	unsigned char d,b;
	int skip,outflag;
	unsigned short c1;
	const unsigned char *p;
	if(x<=-8 || x>=X_RES || y<=-8 || y>=Y_RES) return; //画面夁E
	c1=palette[c];
	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //画面上部に刁E��る場吁E
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
		if(x<0){ //画面左に刁E��る場吁E
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
			if(i>=Y_RES) break; //画面下部に刁E��る場吁E
			d=*p++;
			if(x<0){ //画面左に刁E��る場合�E残る部刁E�Eみ描画
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
					if(j>=X_RES){ //画面右に刁E��る場吁E
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
					if(j>=X_RES){ //画面右に刁E��る場吁E
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
		if(x<0){ //画面左に刁E��る場吁E
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
			if(j>=X_RES) break; //画面右に刁E��る場吁E
			if(y<0){ //画面上部に刁E��る場吁E
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
					if(i>=Y_RES){ //画面下に刁E��る場吁E
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
					if(i>=Y_RES){ //画面下に刁E��る場吁E
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
	//座樁Ex,y)からカラーパレチE��番号cで斁E���Esを表示、bc:バックグランドカラー
	//bcが負の場合�E無要E
	while(*s){
		g_putfont(x,y,c,bc,*s++);
		x+=8;
	}
}
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n){
	//座樁Ex,y)にカラー番号cで数値nを表示、bc:バックグランドカラー
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
	//座樁Ex,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示
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
//座樁Ex,y)の色惁E��を返す、画面外�E0を返す
//パレチE��番号ではなぁE��とに注愁E
	if(x<0 || x>=X_RES || y<0 || y>=Y_RES) return 0;
	return getColor(x,y);
}

// チE��スト画面クリア
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<ATTROFFSET*2/4;i++) *vp++=0;
	cursor=TVRAM;
	LCD_Clear(bgcolor);
}

// グラフィチE��画面クリア
void g_clearscreen(void)
{
	LCD_Clear(0);
}

void textredraw(void){
// チE��スト画面再描画
// チE��スチERAMの冁E��にしたがって液晶に出劁E
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
	//カーソルを座樁Ex,y)にカラー番号cに設宁E
	if(x>=WIDTH_X || y>=WIDTH_Y) return;
	cursor=TVRAM+y*WIDTH_X+x;
	cursorcolor=c;
}
void setcursorcolor(unsigned char c){
	//カーソル位置そ�Eままでカラー番号をcに設宁E
	cursorcolor=c;
}
void printchar(unsigned char n){
	//カーソル位置にチE��ストコードnめE斁E��表示し、カーソルめE斁E��進める
	//画面最終文字表示してもスクロールせず、次の斁E��表示時にスクロールする
	if(cursor<TVRAM || cursor>TVRAM+ATTROFFSET) return;
	if(cursor==TVRAM+ATTROFFSET){
		vramscroll();
		cursor-=WIDTH_X;
	}
	if(n=='\n'){
		//改衁E
		cursor+=WIDTH_X-((cursor-TVRAM)%WIDTH_X);
	} else{
		*cursor=n;
		*(cursor+ATTROFFSET)=cursorcolor;
		g_putfont(((cursor-TVRAM)%WIDTH_X)*8,((cursor-TVRAM)/WIDTH_X)*8,cursorcolor,bgcolor,n);
		cursor++;
	}
}
void printstr(unsigned char *s){
	//カーソル位置に斁E���Esを表示
	while(*s){
		printchar(*s++);
	}
}
void printnum(unsigned int n){
	//カーソル位置に符号なし整数nめE0進数表示
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
	//カーソル位置に符号なし整数nをe桁�E10進数表示�E�前の空き桁E��刁E�Eスペ�Eスで埋める！E
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
	//画面消去しカーソルを�E頭に移勁E
	clearscreen();
}
void startPCG(unsigned char *p,int a){
// RAMフォント！ECG�E��E利用開姁E
// p�E�RAMフォント�E格納アドレス�E�E*256�E�E048バイト！E
// a�E�EシスチE��フォントから�Eコピ�E持E��、Eの場合コピ�Eなし、E以外でコピ�E
	int i;
	if(a){
		for(i=0;i<8*256;i++) *p++=FontData[i];
		fontp=p-8*256;
	}
	else fontp=p;
}
void stopPCG(void){
// RAMフォント！ECG�E��E利用停止
	fontp=(unsigned char *)FontData;
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	bgcolor=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
	textredraw();
}
void init_textgraph(unsigned char align){
	//チE��スト�EグラフィチE��LCDライブラリの使用開姁E
	//パレチE��設宁E
	//LCD縦横設宁E

	int i;
	fontp=(unsigned char *)FontData;
	bgcolor=0; //バックグランドカラーは黁E
	//カラーパレチE��初期匁E
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
	// 液晶の縦横設宁E
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
