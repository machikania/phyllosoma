/*----------------------------------------------------------------------------

Copyright (C) 2023, KenKen, all right reserved.

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
// キー入力、カーソル表示関連機能 by K.Tanaka
// USBキーボード入力システム、カラーテキスト出力システム利用

#include "pico/stdlib.h"
#include "bsp/board.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "usbkeyboard.h"
#include "keyinput.h"

unsigned char lineinputbuf[256]; //lineinput関数用一時バッファ
unsigned char blinkchar,blinkcolor;
int insertmode; //挿入モード：1、上書きモード：0

// 60分のn秒ウェイト
void wait60thsec(unsigned short n){
	uint64_t t=to_us_since_boot(get_absolute_time())%16667;
	sleep_us(16667*n-t);
}

// カーソル点滅用に元の文字コードを退避
void getcursorchar(){
	blinkchar=*cursor;
	blinkcolor=*(cursor+ATTROFFSET);
}

// カーソルを元の文字に戻す
void resetcursorchar(){
	*cursor=blinkchar;
	*(cursor+ATTROFFSET)=blinkcolor;
	putcursorchar();
}

// 定期的に呼び出すことでカーソルを点滅表示させる
// BLINKTIMEで点滅間隔を設定
// 事前にgetcursorchar()を呼び出しておく
void blinkcursorchar(){
	static int blinktimer=0;
	static uint8_t *oldcursorpos=0;
	if(cursor!=oldcursorpos){
		oldcursorpos=cursor;
		blinktimer=0;
	}
	else blinktimer++;
	if(blinktimer>=BLINKTIME*2) blinktimer=0;
	if(blinktimer<BLINKTIME){
		if(insertmode) *cursor=CURSORCHAR;
		else *cursor=CURSORCHAR2;
		*(cursor+ATTROFFSET)=CURSORCOLOR;
		putcursorchar();
	}
	else{
		*cursor=blinkchar;
		*(cursor+ATTROFFSET)=blinkcolor;
		putcursorchar();
	}
}

// キーボードから1キー入力待ち
// 戻り値 通常文字の場合ASCIIコード、その他は0、グローバル変数vkeyに仮想キーコード
unsigned char inputchar(void){
	unsigned char k;
	while(1){
		wait60thsec(1);
		k=usbkb_readkey();  //キーバッファから読み込み、k:通常文字入力の場合ASCIIコード
		if(vkey) return k;
	}
}

// カーソル表示しながらキーボードから1キー入力待ち
// 戻り値 通常文字の場合ASCIIコード、その他は0、グローバル変数vkeyに仮想キーコード
unsigned char cursorinputchar(void){
	unsigned char k;
	getcursorchar(); //カーソル位置の文字を退避（カーソル点滅用）
	while(1){
		wait60thsec(1);
		blinkcursorchar(); //カーソル点滅させる
		k=usbkb_readkey();  //キーバッファから読み込み、k:通常文字入力の場合ASCIIコード
		if(vkey) break;  //キーが押された場合ループから抜ける
	}
	resetcursorchar(); //カーソルを元の文字表示に戻す
	return k;
}

// カーソル表示しながらキーボードから通常文字キー入力待ちし、入力された文字を表示
// 戻り値 入力された文字のASCIIコード、グローバル変数vkeyに最後に押されたキーの仮想キーコード
unsigned char printinputchar(void){
	unsigned char k;
	while(1){
		k=cursorinputchar();
		if(k) break;
	}
	printchar(k);
	return k;
}

// キー入力して文字列配列sに格納
// sに初期文字列を入れておくと最初に表示して文字列の最後にカーソル移動する
// 初期文字列を使用しない場合は*s=0としておく
// カーソル位置はsetcursor関数で指定しておく
// 最大文字数n、最後に0を格納するのでn+1バイトの領域必要、ただしnの最大値は255
// 戻り値　Enterで終了した場合0、ESCで終了時は-1（sは壊さない）
int lineinput(char *s,int n){
	unsigned char *ps,*pd,*pc;
	unsigned char k1,k2;
	int i;

	if(n>255) return -1;
	ps=s;
	pd=lineinputbuf;
	i=0;
	//初期文字列をlineinputbufにコピーし、文字数をiに入れる
	while(*ps!=0 && i<n){
		*pd++=*ps++;
		i++;
	}
	*pd=0;//バッファ内の文字列最後に0
	pc=pd;//現在の文字入力位置ポインタ（最後尾）
	if(i>0) printstr(lineinputbuf); //初期文字列表示
	while(1){
		k1=cursorinputchar(); //カーソル表示しながら1キー入力待ち
		k2=(unsigned char)vkey; //k2:仮想キーコード
		if(k1){
			//通常文字の場合
			if(insertmode || *pc==0){
				//挿入モードまたは最後尾の場合
				if(i==n) continue; //入力文字数最大値の場合無視
				for(pd=lineinputbuf+i;pd>=pc;pd--) *(pd+1)=*pd; //1文字分挿入
				i++;
			}
			*pc=k1; //入力文字を追加
			printstr(pc); //入力文字以降を表示
			pc++;
			for(ps=lineinputbuf+i;ps>pc;ps--) cursor--; //カーソル位置戻し
		}
		else switch(k2){
			//制御文字の場合
			case VK_LEFT:
			case VK_NUMPAD4:
				//左矢印キー
				if(pc>lineinputbuf){
					pc--;
					cursor--;
				}
				break;
			case VK_RIGHT:
			case VK_NUMPAD6:
				//右矢印キー
				if(*pc!=0){
					pc++;
					cursor++;
				}
				break;
			case VK_RETURN: //Enterキー
			case VK_SEPARATOR: //テンキーのEnter
				//入力用バッファから呼び出し元のバッファにコピーして終了
				printchar('\n');
				ps=lineinputbuf;
				pd=s;
				while(*ps!=0) *pd++=*ps++;
				*pd=0;
				return 0;
			case VK_HOME:
			case VK_NUMPAD7:
				//Homeキー、文字列先頭にカーソル移動
				while(pc>lineinputbuf){
					pc--;
					cursor--;
				}
				break;
			case VK_END:
			case VK_NUMPAD1:
				//Endキー、文字列最後尾にカーソル移動
				while(*pc!=0){
					pc++;
					cursor++;
				}
				break;
			case VK_BACK:
				//Back Spaceキー、1文字左に移動しDelete処理
				if(pc==lineinputbuf) break;//カーソルが先頭の場合、無視
				pc--;
				cursor--;
			case VK_DELETE:
			case VK_DECIMAL:
				//Deleteキー、カーソル位置の1文字削除
				if(*pc==0) break;//カーソルが最後尾の場合、無視
				for(pd=pc;*(pd+1)!=0;pd++) *pd=*(pd+1);
				*pd=0;
				i--;
				printstr(pc);
				printchar(0);//NULL文字表示
				for(ps=lineinputbuf+i+1;ps>pc;ps--) cursor--;
				break;
			case VK_INSERT:
			case VK_NUMPAD0:
				//Insertキー、挿入モードトグル動作
				insertmode^=1;
				break;
			case VK_ESCAPE:
			case VK_CANCEL:
				//ESCキーまたはCTRL+Breakキー、-1で終了
				return -1;
		}
	}
}
