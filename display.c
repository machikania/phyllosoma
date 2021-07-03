/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "./interface/graphlib.h"
#include "./interface/LCDdriver.h"
#include "./api.h"
#include "./compiler.h"
#include "./display.h"

/*

CLEAR
	すべての文字列型変数と整数型配列を破棄し、整数値を０とする。また、
	PCGの使用をやめ、表示キャラクターをリセットする。


void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); //バックグランドカラー設定
BGCOLOR r,g,b
	背景色指定。

void cls(void);
CLS
	スクリーン消去。

void setcursorcolor(unsigned char c);
COLOR x
	テキスト色指定。

void setcursor(unsigned char x,unsigned char y,unsigned char c);
CURSOR x,y
	カーソル位置指定。

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g);
PALETTE n,r,g,b
	パレット指定。

void startPCG(unsigned char *p,int a);
	// RAMフォント（PCG）の利用開始、pがフォント格納場所、aが0以外でシステムフォントをコピー
void stopPCG(void);
	// RAMフォント（PCG）の利用停止
PCG x,y,z
	ASCIIコードがxの文字の表示キャラクターを変更する。y,zは、キャラク
	ターデーター。詳細は、下記<PCG>の項を参照。
USEPCG [x]
	PCGを使用、もしくは使用停止する。x=0で使用停止、x=1で使用、x=2で
	キャラクターをリセットして使用。xを省略した場合は、x=1と同じ。

SCROLL x,y
	画面を横方向、もしくは縦方向(斜めも可)に動かす。動かす方向と大きさ
	は、x, yでそれぞれ、横方向の移動度、縦方向の移動度として指定する。
WIDTH x
	キャラクターディスプレイの横幅を文字数で指定。xは30、36、40、48、
	もしくは80。

TVRAM([x])
	ビデオRAMのx番目の内容を、バイト値で返す。xを省略した場合、ビデオ
	RAMの開始位置の物理アドレスを返す。

void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c);
BOXFILL [x1,y1],x2,y2[,c]
	座標(x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画。

void g_circle(int x0,int y0,unsigned int r,unsigned char c);
CIRCLE [x,y],r[,c]
	座標(x,y)を中心に、半径r、カラーcの円を描画。

void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
CIRCLEFILL [x,y],r[,c]
	座標(x,y)を中心に、半径r、カラーcで塗られた円を描画。
GCLS
	画面クリアー。

GCOLOR c
	それぞれの命令で、cを省略した場合の色を指定。
GPALETTE n,r,g,b
	パレット指定。

void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s);
GPRINT [x,y],c,bc,s$
	座標(x,y)にカラーcで文字列s$を表示、bc:背景色（負数の場合背景色指定なし）。

void g_gline(int x1,int y1,int x2,int y2,unsigned char c);
void g_hline(int x1,int x2,int y,unsigned char c);
LINE [x1,y1],x2,y2[,c]
	座標(x1,y1)から(x2,y2)にカラーcで線分を描画。
POINT x,y
	グラフィック現在位置を、設定する。

void g_pset(int x,int y,unsigned char c);
PSET [x,y][,c]
	座標(x,y)の位置にカラーcで点を描画。

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[]);
PUTBMP [x,y],m,n,bbb
	横m*縦nドットのキャラクター(bbbで指定)を座標(x,y)に表示。
	サイズm*nの配列bmpに、単純にカラー番号を並べる。
	ただし、カラーが0の部分は透明色として扱う。ただし、bbbはラベル名もし
	くは配列へのポインター。

USEGRAPHIC [x]
Type Mの場合
	グラフィックディスプレイを使用、もしくは使用停止する。x=0で使用停止、
	x=1, 5, 9で使用、x=2, 6, 10で画面とパレットをクリアーして使用、x=3,7, 
	11でグラフィック領域を確保するが表示はキャラクターディスプレイのまま。
	ただし、グラフィックディスプレイ未使用の状態でx=0, 4, 8の場合は、領域
	を確保する。xを省略した場合は、x=1と同じ。
	ただし、xの値が0-3の場合はType-Z互換グラフィック、4-7の場合は標準グラ
	フィック、8-11の場合はワイドグラフィック。
Type Zの場合
	グラフィックディスプレイを使用、もしくは使用停止する。x=0で使用停止、
	x=1で使用、x=2で画面とパレットをクリアーして使用、x=3でグラフィック領
	域を確保するが表示はキャラクターディスプレイのまま。ただし、グラフィッ
	クディスプレイ未使用の状態でx=0の場合は、領域を確保する。xを省略した場
	合は、x=1と同じ。


GCOLOR(x,y)
	グラフィック座標(x,y)の表示中パレット番号を返す。

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
	return r0;
}
