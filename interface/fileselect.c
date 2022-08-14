#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "ff.h"
#include "../config.h"

#define MAXFILE 256

unsigned char path[256];
unsigned char filenames[MAXFILE][13];
unsigned int keystatus, keystatus2, keystatus3, oldkey; //最新のボタン状態と前回のボタン状態
int keycountUP, keycountLEFT, keycountRIGHT, keycountDOWN, keycountSTART, keycountFIRE;
int filenum, dirnum;

void wait60thsec(unsigned short n){
	// 60分のn秒ウェイト
	uint64_t t = to_us_since_boot(get_absolute_time()) % 16667;
	sleep_us(16667 * n - t);
}
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
	setcursorcolor(4);
	printstr("[FIRE]:EXECUTE [START]:RETURN\n");
	setcursorcolor(7);
	while (!f_eof(&Fil) && cursor<vramend){
		if (f_gets(linebuf, sizeof(linebuf), &Fil) == NULL)
			disperror("Read Line Error.", 0);
		p=linebuf;
		while(*p && cursor<vramend){
			ch=*p++;
			if(ch==13) continue;
			printchar(ch);
		}
	}
	//FIREキーを離すまで待つ
	while(keystatus3!=KEYFIRE){
		keycheck();
		wait60thsec(1);
	}
	while(1){
		keycheck();
		if(keystatus3==KEYFIRE) break;
		if(keystatus3==KEYSTART) break;
		if(cursor>=vramend && (keystatus2==KEYDOWN || keycountDOWN>20)){
			while (1){
				if(*p==0 && !f_eof(&Fil)){
					if (f_gets(linebuf, sizeof(linebuf), &Fil) == NULL)
						disperror("Read Line Error.", 0);
					p=linebuf;
				}
				else if(*p==0) break;
				while(*p){
					ch=*p++;
					if(ch==13) continue;
					printchar(ch);
					if(cursor>=vramend) break;
				}
				if(cursor>=vramend) break;
			}
			cursor2=cursor;
			setcursor(0,0,4);
			printstr("[FIRE]:EXECUTE [START]:RETURN");
			while(cursor<TVRAM+WIDTH_X) printchar(' ');
			cursor=cursor2;
			setcursorcolor(7);
		}
		wait60thsec(1);
	}
	f_close(&Fil);
	cls();
}

// filenames配列のn番目のファイルから一覧表示
void dispfiles(int n){
	int i, j;
	int mx,my;

	mx=WIDTH_X/13;
	my=WIDTH_Y-1;
	setcursor(0, 0, 4);
	printstr("Select file & Push FIRE\n");
	for (i = 0; i < my * mx; i++){
		if (i % mx == 0) printchar(' ');
		if (i + n < dirnum){
			// ディレクトリ
			setcursorcolor(6);
			printchar('[');
			printstr(filenames[i + n]);
			printchar(']');
			//13文字まで空白で埋める
			for (j = 11 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else if (i + n < filenum){
			// ファイル
			setcursorcolor(7);
			printstr(filenames[i + n]);
			//13文字まで空白で埋める
			for (j = 13 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else for (j = 0; j < 13; j++) printchar(' '); //画面最後まで空白で埋める
		if((i+1)%mx==0 && 13*mx+1<WIDTH_X) printchar('\n');
	}
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

	mx=WIDTH_X/13;
	my=WIDTH_Y-1;
	cls();
	while (1){
		filenum = 0;
		dirnum = 0;
		fr = f_opendir(&dj, path);
		if (fr) disperror("Open directory Error.", fr);
		if (path[1]){ // not root directory
			// 親ディレクトリ
			strcpy(filenames[filenum], "..");
			filenum++;
			dirnum++;
		}
		while (1){
			fr = f_readdir(&dj, &fno); // Read a directory item
			if (fr) disperror("Read directory Error.", fr);
			if (fno.fname[0] == 0) break;
			if (fno.fattrib & AM_DIR){ // It is a directory
				strcpy(filenames[filenum], fno.fname);
				filenum++;
				dirnum++;
			}
		}
		f_closedir(&dj);

		fr = f_findfirst(&dj, &fno, path, "*.*"); // 全てのファイル
//		fr = f_findfirst(&dj, &fno, path, "*.BAS"); // BASICソースファイル
		if (fr) disperror("Findfirst Error.", fr);
		while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
			strcpy(filenames[filenum], fno.fname);
			filenum++;
			if (filenum >= MAXFILE) break;
			fr = f_findnext(&dj, &fno); // Search for next item
			if (fr) disperror("Findnext Error.", fr);
		}
		f_closedir(&dj);
		if (filenum == 0) return NULL;
		n = 0;
		top = 0;
		x = 0;
		y = 0;
		dispfiles(top); //ファイル番号topから一覧を画面表示
		do{
			setcursor(x * 13, y + 1, 5);
			printchar(0x1c); // right arrow
			wait60thsec(2);
			setcursor(x * 13, y + 1, 5);
			printchar(' ');
			keycheck();
			key = keystatus2;
			// 20回以上同じボタンを押し続けていればリピートさせる
			if (keycountUP > 20) key |= KEYUP;
			if (keycountLEFT > 20) key |= KEYLEFT;
			if (keycountRIGHT > 20) key |= KEYRIGHT;
			if (keycountDOWN > 20) key |= KEYDOWN;
			if (keycountFIRE > 30) key |= KEYFIRE;
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
				break;
			case KEYRIGHT:
				if (n + 1 >= filenum) break;
				n++;
				x++;
				if (x >= mx){
					x = 0;
					y++;
					if (y >= my){
						y--;
						top += mx;
						dispfiles(top);
					}
				}
				break;
			case KEYSTART:
				set_lcdalign(LCD_ALIGNMENT^HORIZONTAL);
				mx=WIDTH_X/13;
				my=WIDTH_Y-1;
				n = 0;
				top = 0;
				x = 0;
				y = 0;
				dispfiles(top); //ファイル番号topから一覧を画面表示
				break;
			case KEYFIRE:
				if (n >= dirnum && keycountFIRE>30){
					viewfile(filenames[n]);
					if(keystatus3 != KEYFIRE) dispfiles(top);
				}
				break;
			}
		} while (keystatus3 != KEYFIRE);
		if (n < dirnum){
			// ディレクトリの場合
			if (filenames[n][0] == '.'){
				// 親ディレクトリの場合、pathから現ディレクトリを削除
				for (p = path; *p; p++) ;
				for (p -= 2; *p != '/'; p--) ;
				*(p + 1) = 0;
			}
			else{
				// pathにディレクトリ名を結合して最後に'/'を付加
				for (p = path; *p; p++) ;
				for (p2 = filenames[n]; *p2;) *p++ = *p2++;
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
	return filenames[n]; //選択したファイル名へのポインタを返す
}
