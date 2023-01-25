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

#define BLINKTIME 15
#define CURSORCHAR 0x87
#define CURSORCHAR2 0x80
#define CURSORCOLOR 7

// 60分のn秒ウェイト
void wait60thsec(unsigned short n);

// カーソル点滅用に元の文字コードを退避
void getcursorchar();

// カーソルを元の文字に戻す
void resetcursorchar();

// 定期的に呼び出すことでカーソルを点滅表示させる
// BLINKTIMEで点滅間隔を設定
// 事前にgetcursorchar()を呼び出しておく
void blinkcursorchar();

// キー入力して文字列配列sに格納
// sに初期文字列を入れておくと最初に表示して文字列の最後にカーソル移動する
// 初期文字列を使用しない場合は*s=0としておく
// カーソル位置はsetcursor関数で指定しておく
// 最大文字数n、最後に0を格納するのでn+1バイトの領域必要、ただしnの最大値は255
// 戻り値　Enterで終了した場合0、ESCで終了時は-1（sは壊さない）
int lineinput(char *s,int n);

// キーボードから1キー入力待ち
// 戻り値 通常文字の場合ASCIIコード、その他は0、グローバル変数vkeyに仮想キーコード
unsigned char inputchar(void);

// カーソル表示しながらキーボードから通常文字キー入力待ちし、入力された文字を表示
// 戻り値 入力された文字のASCIIコード、グローバル変数vkeyに最後に押されたキーの仮想キーコード
unsigned char printinputchar(void);

// カーソル表示しながらキーボードから1キー入力待ち
// 戻り値 通常文字の場合ASCIIコード、その他は0、グローバル変数vkeyに仮想キーコード
unsigned char cursorinputchar(void);

extern int insertmode; //挿入モード：1、上書きモード：0
