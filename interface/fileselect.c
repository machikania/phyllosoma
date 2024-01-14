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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "ff.h"
#include "../config.h"

#define MAXFILE 256

unsigned char path[256];
static FILINFO files[MAXFILE];
static unsigned int keystatus, keystatus2, keystatus3, oldkey; //最新のボタン状態と前回のボタン状態
static int keycountUP, keycountLEFT, keycountRIGHT, keycountDOWN, keycountSTART, keycountFIRE;
static int filenum, dirnum;
unsigned char show_timestamp=0; // show timestamp
unsigned char filesortby=0; // 0:A...Z 1:NEW...OLD 2:Z...A 3:OLD...NEW

void init_buttons(void){
	// ボタン用GPIO設定
	gpio_init_mask(KEYSMASK);
	gpio_set_dir_in_masked(KEYSMASK);
	gpio_pull_up(GPIO_KEYUP);
	gpio_pull_up(GPIO_KEYLEFT);
	gpio_pull_up(GPIO_KEYRIGHT);
	gpio_pull_up(GPIO_KEYDOWN);
	gpio_pull_up(GPIO_KEYSTART);
	gpio_pull_up(GPIO_KEYFIRE);
	keystatus = KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT | KEYSTART | KEYFIRE; //初期化
}
void keycheck(void){
	//ボタン状態読み取り
	//keystatus :現在押されているボタンに対応するビットを1にする
	//keystatus2:前回押されていなくて、今回押されたボタンに対応するビットを1にする
	//keystatus3:前回押されていて、今回離されたボタンに対応するビットを1にする
	//keycountXX:XXボタンが押された状態で連続して読み込まれた回数
	oldkey = keystatus;
	keystatus = ~gpio_get_all() & KEYSMASK;
	keystatus=rotate_buttons_data(keystatus);
	keystatus2 = keystatus & ~oldkey; //前回ボタンを離していたかチェック
	keystatus3 = ~keystatus & oldkey; //前回ボタンを押していたかチェック
	if (keystatus & KEYUP) keycountUP++;
	else keycountUP = 0;
	if (keystatus & KEYLEFT) keycountLEFT++;
	else keycountLEFT = 0;
	if (keystatus & KEYRIGHT) keycountRIGHT++;
	else keycountRIGHT = 0;
	if (keystatus & KEYDOWN) keycountDOWN++;
	else keycountDOWN = 0;
	if (keystatus & KEYSTART) keycountSTART++;
	else keycountSTART = 0;
	if (keystatus & KEYFIRE) keycountFIRE++;
	else keycountFIRE = 0;
}

// エラー番号を表示してい停止
void disperror(unsigned char *s, FRESULT fr){
	printstr(s);
	printstr(" FRESULT:");
	printnum(fr);
	while (1) ;
}

// テキストファイルの中身表示
void viewfile(unsigned char *fname){
	FIL Fil;
	FRESULT fr;
	unsigned char linebuf[256],ch,*p;
	unsigned char *vramend,*cursor2;

	vramend=TVRAM+WIDTH_X*WIDTH_Y;
	cls();
	fr = f_open(&Fil, fname, FA_READ);
	if (fr) disperror("File Open Error.", fr);
	p=linebuf;
	*p=0;
	setcursorcolor(4);
	printstr("[FIRE]:Exec [START]:Return\n");
	setcursorcolor(7);
	while(1){
		keycheck();
		if(keystatus2==KEYFIRE) break;
		if(keystatus3==KEYSTART) break;
		while (*p!=0 || !f_eof(&Fil)){
			while (cursor<vramend || (keystatus2==KEYDOWN || keycountDOWN>20)){
				if(*p==0 && !f_eof(&Fil)){
					if (f_gets(linebuf, sizeof(linebuf), &Fil) == NULL)
						disperror("Read Line Error.", 0);
					p=linebuf;
				}
				else if(*p==0) break;
				if(cursor>=vramend){
					windowscroll(1,WIDTH_Y-1);
					cursor-=WIDTH_X;
				}
				while(*p){
					ch=*p++;
					if(ch==13) continue;
					printchar(ch);
					if(cursor>=vramend) break;
				}
				if(cursor>=vramend) break;
			}
			if(cursor>=vramend) break;
		}
		sleep_ms(10);
	}
	f_close(&Fil);
	cls();
}

// タイムスタンプ表示
static void disptimestamp(FILINFO *finfo){
	int k;
	printnum(1980+(finfo->fdate>>9));
	printchar('/');
	k=(finfo->fdate>>5) & 0x0f;
	if(k<10) printchar('0');
	printnum(k);
	printchar('/');
	k=finfo->fdate & 0x1f;
	if(k<10) printchar('0');
	printnum(k);
	printchar(' ');
	k=finfo->ftime>>11;
	if(k<10) printchar('0');
	printnum(k);
	printchar(':');
	k=(finfo->ftime>>5) & 0x3f;
	if(k<10) printchar('0');
	printnum(k);
}
// filenames配列のn番目のファイルから一覧表示
void dispfiles(int n){
	int i, j,k;
	int mx,my;

	if(show_timestamp) mx=1; else mx=WIDTH_X/13;
	my=WIDTH_Y-1;
	setcursor(0, 0, 4);
	printstr("[FIRE]:Exec [START]:View\n");
	for (i = 0; i < my * mx; i++){
		if (i % mx == 0) printchar(' ');
		if (i + n < dirnum){
			// ディレクトリ
			setcursorcolor(6);
			printchar('[');
			printstr(files[i + n].fname);
			printchar(']');
			//13文字まで空白で埋める
			for (j = 11 - strlen(files[i + n].fname); j > 0; j--)
				printchar(' ');
			if(show_timestamp){
				disptimestamp(&files[i+n]);
			}
		}
		else if (i + n < filenum){
			// ファイル
			setcursorcolor(7);
			printstr(files[i + n].fname);
			//13文字まで空白で埋める
			for (j = 13 - strlen(files[i + n].fname); j > 0; j--)
				printchar(' ');
			if(show_timestamp){
				disptimestamp(&files[i+n]);
			}
		}
		else if(show_timestamp){
			for(j = 0; j < 30; j++) printchar(' '); //画面最後まで空白で埋める
		}
		else{
			for(j = 0; j < 13; j++) printchar(' '); //画面最後まで空白で埋める
		}
		if((i+1)%mx==0 && (cursor-TVRAM)%WIDTH_X) printchar('\n');
	}
}

// ファイルの並べ替え比較関数
static int fnamecmp(const void *s1,const void *s2){
	uint32_t t1,t2;
	t1=(((FILINFO *)s1)->fdate <<16)+((FILINFO *)s1)->ftime;
	t2=(((FILINFO *)s2)->fdate <<16)+((FILINFO *)s2)->ftime;
	switch (filesortby)
	{
	case 0: // A..Z
		return strncmp(((FILINFO *)s1)->fname,((FILINFO *)s2)->fname,12);
	case 1: // NEW..OLD
		return (int)(t2-t1);
	case 2: // Z..A
		return strncmp(((FILINFO *)s2)->fname,((FILINFO *)s1)->fname,12);
	case 3: // OLD..NEW
		return (int)(t1-t2);
	}
	return 0;
}

// SDカード内のBASICソースプログラム一覧を表示、選択し
// ファイル名を返す
unsigned char *fileselect(void){
	FRESULT fr;
	DIR dj;		 // Directory object
	FILINFO fno; // File information
	int n;		 // file number
	int top;	 // 画面先頭のファイル番号
	int x, y;
	unsigned char *p, *p2;
	unsigned int key;
	int mx,my;

	if(show_timestamp) mx=1; else mx=WIDTH_X/13;
	my=WIDTH_Y-1;
	cls();
	while (1){
		filenum = 0;
		dirnum = 0;
		fr = f_opendir(&dj, path);
		if (fr) disperror("Open directory Error.", fr);
		if (path[1]){ // not root directory
			// 親ディレクトリ
			strcpy(files[filenum].fname, "..");
			files[filenum].fdate=20513; // 2020/01/01
			files[filenum].ftime=0;
			files[filenum].fattrib=AM_DIR;
			filenum++;
			dirnum++;
		}
		while (1){
			fr = f_readdir(&dj, &fno); // Read a directory item
			if (fr) disperror("Read directory Error.", fr);
			if (fno.fname[0] == 0) break;
			if ((fno.fattrib & AM_DIR) && !(fno.fattrib & AM_SYS)){ // It is a directory
				files[filenum]=fno;
				filenum++;
				dirnum++;
				if (filenum >= MAXFILE) break;
			}
		}
		f_closedir(&dj);
		if(dirnum>1){
			qsort(files,dirnum,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
		}

		if(filenum < MAXFILE){
			fr = f_findfirst(&dj, &fno, path, "*.*"); // 全てのファイル
//			fr = f_findfirst(&dj, &fno, path, "*.BAS"); // BASICソースファイル
			if (fr) disperror("Findfirst Error.", fr);
			while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
				files[filenum]=fno;
				filenum++;
				if (filenum >= MAXFILE) break;
				fr = f_findnext(&dj, &fno); // Search for next item
				if (fr) disperror("Findnext Error.", fr);
			}
			f_closedir(&dj);
		}
		if (filenum == 0) return NULL;
		if(filenum-dirnum>1){
			qsort(&(files[dirnum]),filenum-dirnum,sizeof(FILINFO),fnamecmp); //ファイル名順に並べ替え
		}
		n = 0;
		top = 0;
		x = 0;
		y = 0;
		dispfiles(top); //ファイル番号topから一覧を画面表示
		do{
			setcursor(x * 13, y + 1, 5);
			printchar(0x1c); // right arrow
			sleep_ms(25);
			setcursor(x * 13, y + 1, 5);
			printchar(' ');
			keycheck();
			key = keystatus2;
			// 30回以上同じボタンを押し続けていればリピートさせる
			if (keycountUP > 30) key |= KEYUP;
			if (keycountLEFT > 30) key |= KEYLEFT;
			if (keycountRIGHT > 30) key |= KEYRIGHT;
			if (keycountDOWN > 30) key |= KEYDOWN;
			switch (key){
			case KEYUP:
				if (y > 0){
					n -= mx;
					y--;
				}
				else if (top >= mx){
					n -= mx;
					top -= mx;
					dispfiles(top);
				}
				break;
			case KEYDOWN:
				if (y < my-1){
					if (n + mx < filenum){
						n += mx;
						y++;
					}
				}
				else if(n - x + mx < filenum){
					n += mx;
					if (n >= filenum){
						n -= x;
						x = 0;
					}
					top += mx;
					dispfiles(top);
				}
				break;
			case KEYLEFT:
				if (x > 0){
					n--;
					x--;
				}
/*
				else if (y > 0){
						n--;
						x = mx - 1;
						y--;
				}
				else if (top >= mx){
					n--;
					x = mx - 1;
					top -= mx;
					dispfiles(top);
				}
*/
				break;
			case KEYRIGHT:
				if (n + 1 >= filenum) break;
				if (x + 1 < mx)
				{
					n++;
					x++;
				}
/*
				if (x >= mx){
					x = 0;
					y++;
					if (y >= my){
						y--;
						top += mx;
						dispfiles(top);
					}
				}
*/
				break;
			}
			if (keycountSTART>20){
				if(WIDTH_X>=30 && show_timestamp==0){
					//タイムスタンプ表示
					show_timestamp=1;
					cls();
				}
				else{
					//画面の縦横変更、タイムスタンプ非表示
					set_lcdalign(LCD_ALIGNMENT^HORIZONTAL);
					show_timestamp=0;
				}
				if(show_timestamp) mx=1; else mx=WIDTH_X/13;
				my = WIDTH_Y-1;
				n = 0;
				top = 0;
				x = 0;
				y = 0;
				dispfiles(top); //ファイル番号topから一覧を画面表示
				//STARTキーを離すまで待つ
				while(keycountSTART){
					keycheck();
					sleep_ms(16);
				}
			}
			else if((keystatus==(KEYSTART | KEYDOWN)) && (keystatus2 & KEYDOWN)){
				filesortby=(filesortby+1)&3;
				if(dirnum>1){
					qsort(files,dirnum,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
				}
				if(filenum-dirnum>1){
					qsort(&(files[dirnum]),filenum-dirnum,sizeof(FILINFO),fnamecmp); //ファイル名順に並べ替え
				}
				n = 0;
				top = 0;
				x = 0;
				y = 0;
				dispfiles(top); //ファイル番号topから一覧を画面表示
				//キーを離すまで待つ
				while(keystatus){
					keycheck();
					sleep_ms(16);
				}
			}
			else if(keystatus3==KEYSTART && n >= dirnum){
				//プログラムソース表示
				viewfile(files[n].fname);
				if(keystatus2 == KEYFIRE) break;
				dispfiles(top);
			}
		} while (keystatus2 != KEYFIRE);
		if (n < dirnum){
			// ディレクトリの場合
			if ((files[n].fname)[0] == '.'){
				// 親ディレクトリの場合、pathから現ディレクトリを削除
				for (p = path; *p; p++) ;
				for (p -= 2; *p != '/'; p--) ;
				*(p + 1) = 0;
			}
			else{
				// pathにディレクトリ名を結合して最後に'/'を付加
				for (p = path; *p; p++) ;
				for (p2 = files[n].fname; *p2;) *p++ = *p2++;
				*p++ = '/';
				*p = 0;
			}
			fr = f_chdir(path); //Change Directory
			if (fr) disperror("Change directory Error.", fr);
		}
		else break; // ファイル選択
	}
	cls();
	setcursorcolor(7);
	return files[n].fname; //選択したファイル名へのポインタを返す
}
