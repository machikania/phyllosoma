//LCD繝・く繧ｹ繝医・繧ｰ繝ｩ繝輔ぅ繝・け繝ｩ繧､繝悶Λ繝ｪ

#include "graphlib.h"
#include "LCDdriver.h"

unsigned char TVRAM[ATTROFFSET*2+1] __attribute__ ((aligned (4)));
unsigned char *fontp; //繝輔か繝ｳ繝域ｼ邏阪い繝峨Ξ繧ｹ縲∝・譛溷喧譎ゅ・FontData縲ヽAM謖・ｮ壹☆繧九％縺ｨ縺ｧPCG繧貞ｮ溽樟
unsigned int bgcolor; // 繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ
unsigned char twidth; //繝・く繧ｹ繝・陦梧枚蟄玲焚
unsigned char *cursor;
unsigned char cursorcolor;
unsigned short palette[256];
int WIDTH_X; // 讓ｪ譁ｹ蜷第枚蟄玲焚
int WIDTH_Y; // 邵ｦ譁ｹ蜷第枚蟄玲焚

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g){
//繝・く繧ｹ繝茨ｼ上げ繝ｩ繝輔ぅ繝・け蜈ｱ逕ｨ繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ險ｭ螳・
	palette[n]=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

void g_pset(int x,int y,unsigned char c)
// (x,y)縺ｮ菴咲ｽｮ縺ｫ繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｧ轤ｹ繧呈緒逕ｻ
{
	if(x>=0 && x<X_RES && y>=0 && y<Y_RES)
		drawPixel(x,y,palette[c]);
}

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[])
// 讓ｪm*邵ｦn繝峨ャ繝医・繧ｭ繝｣繝ｩ繧ｯ繧ｿ繝ｼ繧貞ｺｧ讓凅,y縺ｫ陦ｨ遉ｺ
// unsigned char bmp[m*n]驟榊・縺ｫ縲∝腰邏斐↓繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷ繧剃ｸｦ縺ｹ繧・
// 繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷ縺・縺ｮ驛ｨ蛻・・騾乗・濶ｲ縺ｨ縺励※謇ｱ縺・
{
	int i,j;
	int skip,outflag;
	const unsigned char *p;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //逕ｻ髱｢螟・
	outflag=0;

	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //逕ｻ髱｢荳企Κ縺ｫ蛻・ｌ繧句ｴ蜷・
			i=0;
			p=bmp-y*m;
		}
		else{
			i=y;
			p=bmp;
		}
		for(;i<y+n;i++){
			if(i>=Y_RES) return; //逕ｻ髱｢荳矩Κ縺ｫ蛻・ｌ繧句ｴ蜷・
			if(x<0){ //逕ｻ髱｢蟾ｦ縺ｫ蛻・ｌ繧句ｴ蜷医・谿九ｋ驛ｨ蛻・・縺ｿ謠冗判
				j=0;
				p+=-x;
			}
			else{
				j=x;
			}
			skip=1;
			for(;j<x+m;j++){
				if(j>=X_RES){ //逕ｻ髱｢蜿ｳ縺ｫ蛻・ｌ繧句ｴ蜷・
					p+=x+m-j;
					break;
				}
				if(*p!=0){ //繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷ縺・縺ｮ蝣ｴ蜷医・乗・縺ｨ縺励※蜃ｦ逅・
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
		if(x<0){ //逕ｻ髱｢蟾ｦ驛ｨ縺ｫ蛻・ｌ繧句ｴ蜷・
			j=0;
			p=bmp-x;
		}
		else{
			j=x;
			p=bmp;
		}
		for(;j<x+m;j++){
			if(j>=X_RES) return; //逕ｻ髱｢蜿ｳ驛ｨ縺ｫ蛻・ｌ繧句ｴ蜷・
			if(y<0){ //逕ｻ髱｢荳翫↓蛻・ｌ繧句ｴ蜷医・谿九ｋ驛ｨ蛻・・縺ｿ謠冗判
				i=0;
				p+=-y*m;
			}
			else{
				i=y;
			}
			skip=1;
			for(;i<y+n;i++){
				if(i>=Y_RES){ //逕ｻ髱｢荳九↓蛻・ｌ繧句ｴ蜷・
					p+=(y+n-i)*m;
					break;
				}
				if(*p!=0){ //繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷ縺・縺ｮ蝣ｴ蜷医・乗・縺ｨ縺励※蜃ｦ逅・
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

// 邵ｦm*讓ｪn繝峨ャ繝医・繧ｭ繝｣繝ｩ繧ｯ繧ｿ繝ｼ豸亥悉
// 繧ｫ繝ｩ繝ｼ0縺ｧ蝪励ｊ縺､縺ｶ縺・
void g_clrbmpmn(int x,int y,unsigned char m,unsigned char n)
{
	int i,j,k,dx,dy;
	if(x<=-m || x>X_RES || y<=-n || y>=Y_RES) return; //逕ｻ髱｢螟・
	if(y<0){ //逕ｻ髱｢荳企Κ縺ｫ蛻・ｌ繧句ｴ蜷・
		i=0;
		dy=n+y;
	}
	else{
 		i=y;
		dy=n;
	}
	if(x<0) { //逕ｻ髱｢蟾ｦ縺ｫ蛻・ｌ繧句ｴ蜷医・谿九ｋ驛ｨ蛻・・縺ｿ謠冗判
		j=0;
		dx=m+x;
	}
	else {
		j=x;
		dx=m;
	}
	if(x+m>X_RES) { //逕ｻ髱｢蜿ｳ縺ｫ蛻・ｌ繧句ｴ蜷・
		dx-=(x+m)-X_RES;
	}
	if(y+n>Y_RES) { //逕ｻ髱｢荳九↓蛻・ｌ繧句ｴ蜷・
		dy-=(y+n)-Y_RES;
	}
	LCD_setAddrWindow(j,i,dx,dy);
	dx*=dy;
	for(i=0;i<dx;i++) LCD_WriteData2_notfinish(0);
	checkSPIfinish();
}

void g_gline(int x1,int y1,int x2,int y2,unsigned char c)
// (x1,y1)-(x2,y2)縺ｫ繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｧ邱壼・繧呈緒逕ｻ
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
// (x1,y)-(x2,y)縺ｸ縺ｮ豌ｴ蟷ｳ繝ｩ繧､繝ｳ繧帝ｫ倬滓緒逕ｻ
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
// (x0,y0)繧剃ｸｭ蠢・↓縲∝濠蠕в縲√き繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｮ蜀・ｒ謠冗判
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
// (x1,y1),(x2,y2)繧貞ｯｾ隗堤ｷ壹→縺吶ｋ繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｧ蝪励ｉ繧後◆髟ｷ譁ｹ蠖｢繧呈緒逕ｻ
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
// (x0,y0)繧剃ｸｭ蠢・↓縲∝濠蠕в縲√き繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｧ蝪励ｉ繧後◆蜀・ｒ謠冗判
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
//8*8繝峨ャ繝医・繧｢繝ｫ繝輔ぃ繝吶ャ繝医ヵ繧ｩ繝ｳ繝郁｡ｨ遉ｺ
//蠎ｧ讓・x,y)縲√き繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc
//bc:繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ縲∬ｲ謨ｰ縺ｮ蝣ｴ蜷育┌隕・
//n:譁・ｭ礼分蜿ｷ
{
	int i,j,dx,dy;
	unsigned char d,b;
	int skip,outflag;
	unsigned short c1;
	const unsigned char *p;
	if(x<=-8 || x>=X_RES || y<=-8 || y>=Y_RES) return; //逕ｻ髱｢螟・
	c1=palette[c];
	if(LCD_ALIGNMENT == VERTICAL){
		if(y<0){ //逕ｻ髱｢荳企Κ縺ｫ蛻・ｌ繧句ｴ蜷・
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
		if(x<0){ //逕ｻ髱｢蟾ｦ縺ｫ蛻・ｌ繧句ｴ蜷・
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
			if(i>=Y_RES) break; //逕ｻ髱｢荳矩Κ縺ｫ蛻・ｌ繧句ｴ蜷・
			d=*p++;
			if(x<0){ //逕ｻ髱｢蟾ｦ縺ｫ蛻・ｌ繧句ｴ蜷医・谿九ｋ驛ｨ蛻・・縺ｿ謠冗判
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
					if(j>=X_RES){ //逕ｻ髱｢蜿ｳ縺ｫ蛻・ｌ繧句ｴ蜷・
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
					if(j>=X_RES){ //逕ｻ髱｢蜿ｳ縺ｫ蛻・ｌ繧句ｴ蜷・
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
		if(x<0){ //逕ｻ髱｢蟾ｦ縺ｫ蛻・ｌ繧句ｴ蜷・
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
			if(j>=X_RES) break; //逕ｻ髱｢蜿ｳ縺ｫ蛻・ｌ繧句ｴ蜷・
			if(y<0){ //逕ｻ髱｢荳企Κ縺ｫ蛻・ｌ繧句ｴ蜷・
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
					if(i>=Y_RES){ //逕ｻ髱｢荳九↓蛻・ｌ繧句ｴ蜷・
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
					if(i>=Y_RES){ //逕ｻ髱｢荳九↓蛻・ｌ繧句ｴ蜷・
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
	//蠎ｧ讓・x,y)縺九ｉ繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ逡ｪ蜿ｷc縺ｧ譁・ｭ怜・s繧定｡ｨ遉ｺ縲｜c:繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ
	//bc縺瑚ｲ縺ｮ蝣ｴ蜷医・辟｡隕・
	while(*s){
		g_putfont(x,y,c,bc,*s++);
		x+=8;
	}
}
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n){
	//蠎ｧ讓・x,y)縺ｫ繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷc縺ｧ謨ｰ蛟､n繧定｡ｨ遉ｺ縲｜c:繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ
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
	//蠎ｧ讓・x,y)縺ｫ繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷc縺ｧ謨ｰ蛟､n繧定｡ｨ遉ｺ縲｜c:繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ縲‘譯√〒陦ｨ遉ｺ
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
//蠎ｧ讓・x,y)縺ｮ濶ｲ諠・ｱ繧定ｿ斐☆縲∫判髱｢螟悶・0繧定ｿ斐☆
//繝代Ξ繝・ヨ逡ｪ蜿ｷ縺ｧ縺ｯ縺ｪ縺・％縺ｨ縺ｫ豕ｨ諢・
	if(x<0 || x>=X_RES || y<0 || y>=Y_RES) return 0;
	return getColor(x,y);
}

// 繝・く繧ｹ繝育判髱｢繧ｯ繝ｪ繧｢
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<ATTROFFSET*2/4;i++) *vp++=0;
	cursor=TVRAM;
	LCD_Clear(bgcolor);
}

// 繧ｰ繝ｩ繝輔ぅ繝・け逕ｻ髱｢繧ｯ繝ｪ繧｢
void g_clearscreen(void)
{
	LCD_Clear(0);
}

void textredraw(void){
// 繝・く繧ｹ繝育判髱｢蜀肴緒逕ｻ
// 繝・く繧ｹ繝・RAM縺ｮ蜀・ｮｹ縺ｫ縺励◆縺後▲縺ｦ豸ｲ譎ｶ縺ｫ蜃ｺ蜉・
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
	//繧ｫ繝ｼ繧ｽ繝ｫ繧貞ｺｧ讓・x,y)縺ｫ繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷc縺ｫ險ｭ螳・
	if(x>=WIDTH_X || y>=WIDTH_Y) return;
	cursor=TVRAM+y*WIDTH_X+x;
	cursorcolor=c;
}
void setcursorcolor(unsigned char c){
	//繧ｫ繝ｼ繧ｽ繝ｫ菴咲ｽｮ縺昴・縺ｾ縺ｾ縺ｧ繧ｫ繝ｩ繝ｼ逡ｪ蜿ｷ繧団縺ｫ險ｭ螳・
	cursorcolor=c;
}
void printchar(unsigned char n){
	//繧ｫ繝ｼ繧ｽ繝ｫ菴咲ｽｮ縺ｫ繝・く繧ｹ繝医さ繝ｼ繝穎繧・譁・ｭ苓｡ｨ遉ｺ縺励√き繝ｼ繧ｽ繝ｫ繧・譁・ｭ鈴ｲ繧√ｋ
	//逕ｻ髱｢譛邨よ枚蟄苓｡ｨ遉ｺ縺励※繧ゅせ繧ｯ繝ｭ繝ｼ繝ｫ縺帙★縲∵ｬ｡縺ｮ譁・ｭ苓｡ｨ遉ｺ譎ゅ↓繧ｹ繧ｯ繝ｭ繝ｼ繝ｫ縺吶ｋ
	if(cursor<TVRAM || cursor>TVRAM+ATTROFFSET) return;
	if(cursor==TVRAM+ATTROFFSET){
		vramscroll();
		cursor-=WIDTH_X;
	}
	if(n=='\n'){
		//謾ｹ陦・
		cursor+=WIDTH_X-((cursor-TVRAM)%WIDTH_X);
	} else{
		*cursor=n;
		*(cursor+ATTROFFSET)=cursorcolor;
		g_putfont(((cursor-TVRAM)%WIDTH_X)*8,((cursor-TVRAM)/WIDTH_X)*8,cursorcolor,bgcolor,n);
		cursor++;
	}
}
void printstr(unsigned char *s){
	//繧ｫ繝ｼ繧ｽ繝ｫ菴咲ｽｮ縺ｫ譁・ｭ怜・s繧定｡ｨ遉ｺ
	while(*s){
		printchar(*s++);
	}
}
void printnum(unsigned int n){
	//繧ｫ繝ｼ繧ｽ繝ｫ菴咲ｽｮ縺ｫ隨ｦ蜿ｷ縺ｪ縺玲紛謨ｰn繧・0騾ｲ謨ｰ陦ｨ遉ｺ
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
	//繧ｫ繝ｼ繧ｽ繝ｫ菴咲ｽｮ縺ｫ隨ｦ蜿ｷ縺ｪ縺玲紛謨ｰn繧弾譯√・10騾ｲ謨ｰ陦ｨ遉ｺ・亥燕縺ｮ遨ｺ縺肴｡・Κ蛻・・繧ｹ繝壹・繧ｹ縺ｧ蝓九ａ繧具ｼ・
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
	//逕ｻ髱｢豸亥悉縺励き繝ｼ繧ｽ繝ｫ繧貞・鬆ｭ縺ｫ遘ｻ蜍・
	clearscreen();
}
void startPCG(unsigned char *p,int a){
// RAM繝輔か繝ｳ繝茨ｼ・CG・峨・蛻ｩ逕ｨ髢句ｧ・
// p・啌AM繝輔か繝ｳ繝医・譬ｼ邏阪い繝峨Ξ繧ｹ・・*256・・048繝舌う繝茨ｼ・
// a・・繧ｷ繧ｹ繝・Β繝輔か繝ｳ繝医°繧峨・繧ｳ繝斐・謖・ｮ壹・縺ｮ蝣ｴ蜷医さ繝斐・縺ｪ縺励・莉･螟悶〒繧ｳ繝斐・
	int i;
	if(a){
		for(i=0;i<8*256;i++) *p++=FontData[i];
		fontp=p-8*256;
	}
	else fontp=p;
}
void stopPCG(void){
// RAM繝輔か繝ｳ繝茨ｼ・CG・峨・蛻ｩ逕ｨ蛛懈ｭ｢
	fontp=(unsigned char *)FontData;
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	bgcolor=((r>>3)<<11)+((g>>2)<<5)+(b>>3);
	textredraw();
}
void init_textgraph(unsigned char align){
	//繝・く繧ｹ繝医・繧ｰ繝ｩ繝輔ぅ繝・けLCD繝ｩ繧､繝悶Λ繝ｪ縺ｮ菴ｿ逕ｨ髢句ｧ・
	//繝代Ξ繝・ヨ險ｭ螳・
	//LCD邵ｦ讓ｪ險ｭ螳・

	int i;
	fontp=(unsigned char *)FontData;
	bgcolor=0; //繝舌ャ繧ｯ繧ｰ繝ｩ繝ｳ繝峨き繝ｩ繝ｼ縺ｯ鮟・
	//繧ｫ繝ｩ繝ｼ繝代Ξ繝・ヨ蛻晄悄蛹・
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
	// 豸ｲ譎ｶ縺ｮ邵ｦ讓ｪ險ｭ螳・
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
