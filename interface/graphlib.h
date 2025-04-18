/*----------------------------------------------------------------------------

Copyright (C) 2022, KenKen, all right reserved.

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
void clearscreen(void); //テキスト画面クリア
void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g); //テキストパレット設定
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); //バックグランドカラー設定
void init_textgraph(unsigned char align); //LCDテキスト・グラフィック機能利用準備
void init_palette(void); //カラーパレット初期化

void putcursorchar(void);
	// カーソル位置の文字をテキストVRAMにしたがって液晶に出力
void textredraw(void);
	// テキスト画面再描画、テキストVRAMの内容にしたがって液晶に出力
void vramscroll(void);
	//1行スクロール
void vramscrolldown(void);
	//1行逆スクロール
void windowscroll(int y1,int y2);
	//行y1からy2の間を1行スクロール
void setcursor(unsigned char x,unsigned char y,unsigned char c);
	//カーソル位置とカラーを設定
void setcursorcolor(unsigned char c);
	//カーソル位置そのままでカラー番号をcに設定
void printchar(unsigned char n);
	//カーソル位置にテキストコードnを1文字表示し、カーソルを1文字進める
void printstr(unsigned char *s);
	//カーソル位置に文字列sを表示
void printnum(unsigned int n);
	//カーソル位置に符号なし整数nを10進数表示
void printnum2(unsigned int n,unsigned char e);
	//カーソル位置に符号なし整数nをe桁の10進数表示（前の空き桁部分はスペースで埋める）
void cls(void);
	//テキスト画面消去し、カーソルを先頭に移動
void startPCG(unsigned char *p,int a);
	// RAMフォント（PCG）の利用開始、pがフォント格納場所、aが0以外でシステムフォントをコピー
void stopPCG(void);
	// RAMフォント（PCG）の利用停止


void g_pset(int x,int y,unsigned char c);
// (x,y)の位置にカラーcで点を描画

void g_putbmpmn(int x,int y,unsigned short m,unsigned short n,const unsigned char bmp[]);
// 横m*縦nドットのキャラクターを座標x,yに表示
// unsigned char bmp[m*n]配列に、単純にカラー番号を並べる
// カラー番号が0の部分は透明色として扱う

void g_clrbmpmn(int x,int y,unsigned short m,unsigned short n);
// 縦m*横nドットのキャラクター消去
// カラー0で塗りつぶし

void g_gline(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1)-(x2,y2)にカラーcで線分を描画

void g_hline(int x1,int x2,int y,unsigned char c);
// (x1,y)-(x2,y)への水平ラインを高速描画

void g_circle(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)を中心に、半径r、カラーcの円を描画

void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画

void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)を中心に、半径r、カラーcで塗られた円を描画

void g_putfont(int x,int y,unsigned char c,int bc,unsigned char n);
//8*8ドットのアルファベットフォント表示
//座標（x,y)、カラー番号c
//bc:バックグランドカラー、負数の場合無視
//n:文字番号

void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s);
//座標(x,y)からカラー番号cで文字列sを表示、bc:バックグランドカラー

void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n);
//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー

void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e);
//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示

unsigned int g_color(int x,int y);
//座標(x,y)の色を返す（RGB565形式）

void g_clearscreen(void);
// グラフィック画面クリア

void set_lcdalign(unsigned char align);
// 液晶の縦横設定

extern unsigned short palette[];
//パレット用配列

extern const unsigned char FontData[];
//フォントデータ

extern unsigned char TVRAM[];
//テキストVRAM

void video_init(void);
// Initialize video for running BASIC code

void lcd_spi_init(void);
// Set SPI baudrate again

void set_gvram(unsigned char *gvram_draw,unsigned char *gvram_disp);
// Set drawing and displaying graphic-VRAM
