#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "ff.h"

#define MAXFILE 256

// 入力�Eタンのビット定義
#define GPIO_KEYUP 0
#define GPIO_KEYLEFT 1
#define GPIO_KEYRIGHT 2
#define GPIO_KEYDOWN 3
#define GPIO_KEYSTART 4
#define GPIO_KEYFIRE 5
#define KEYUP (1 << GPIO_KEYUP)
#define KEYLEFT (1 << GPIO_KEYLEFT)
#define KEYRIGHT (1 << GPIO_KEYRIGHT)
#define KEYDOWN (1 << GPIO_KEYDOWN)
#define KEYSTART (1 << GPIO_KEYSTART)
#define KEYFIRE (1 << GPIO_KEYFIRE)
#define KEYSMASK (KEYUP | KEYLEFT | KEYRIGHT | KEYDOWN | KEYSTART | KEYFIRE)

unsigned char path[256];
unsigned char filenames[MAXFILE][13];
unsigned short keystatus, keystatus2, keystatus3, oldkey; //最新のボタン状態と前回のボタン状慁E
int keycountUP, keycountLEFT, keycountRIGHT, keycountDOWN, keycountSTART, keycountFIRE;
int filenum, dirnum;

void wait60thsec(unsigned short n){
	// 60刁E�En秒ウェイチE
	uint64_t t = to_us_since_boot(get_absolute_time()) % 16667;
	sleep_us(16667 * n - t);
}
void init_buttons(void){
	// ボタン用GPIO設宁E
	gpio_init_mask(KEYSMASK);
	gpio_set_dir_in_masked(KEYSMASK);
	gpio_pull_up(GPIO_KEYUP);
	gpio_pull_up(GPIO_KEYLEFT);
	gpio_pull_up(GPIO_KEYRIGHT);
	gpio_pull_up(GPIO_KEYDOWN);
	gpio_pull_up(GPIO_KEYSTART);
	gpio_pull_up(GPIO_KEYFIRE);
	keystatus = KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT | KEYSTART | KEYFIRE; //初期匁E
}
void keycheck(void){
	//ボタン状態読み取り
	//keystatus :現在押されてぁE��ボタンに対応するビチE��めEにする
	//keystatus2:前回押されてぁE��くて、今回押された�Eタンに対応するビチE��めEにする
	//keystatus3:前回押されてぁE��、今回離された�Eタンに対応するビチE��めEにする
	//keycountXX:XXボタンが押された状態で連続して読み込まれた回数
	oldkey = keystatus;
	keystatus = ~gpio_get_all() & KEYSMASK;
	keystatus2 = keystatus & ~oldkey; //前回ボタンを離してぁE��かチェチE��
	keystatus3 = ~keystatus & oldkey; //前回ボタンを押してぁE��かチェチE��
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

// エラー番号を表示してぁE��止
void disperror(unsigned char *s, FRESULT fr){
	printstr(s);
	printstr(" FRESULT:");
	printnum(fr);
	while (1) ;
}

// filenames配�Eのn番目のファイルから一覧表示
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
			// チE��レクトリ
			setcursorcolor(6);
			printchar('[');
			printstr(filenames[i + n]);
			printchar(']');
			//13斁E��まで空白で埋めめE
			for (j = 11 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else if (i + n < filenum){
			// ファイル
			setcursorcolor(7);
			printstr(filenames[i + n]);
			//13斁E��まで空白で埋めめE
			for (j = 13 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else for (j = 0; j < 13; j++) printchar(' '); //画面最後まで空白で埋めめE
		if((i+1)%mx==0 && 13*mx+1<WIDTH_X) printchar('\n');
	}
}

// SDカード�EのBASICソースプログラム一覧を表示、E��択し
// ファイル名を返す
unsigned char *fileselect(void){
	FRESULT fr;
	DIR dj;		 // Directory object
	FILINFO fno; // File information
	int n;		 // file number
	int top;	 // 画面先頭のファイル番号
	int x, y;
	unsigned char *p, *p2;
	unsigned short key;
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
			// 親チE��レクトリ
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
			// 30回以上同じ�Eタンを押し続けてぁE��ばリピ�Eトさせる
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
				if (n - x + mx < filenum){
					n += mx;
					if (n >= filenum){
						n -= x;
						x = 0;
					}
					if (y < my-1){
						y++;
					}
					else{
						top += mx;
						dispfiles(top);
					}
				}
				break;
			case KEYLEFT:
				if (x > 0){
					n--;
					x--;
				}
				break;
			case KEYRIGHT:
				if (x < mx-1 && n + 1 < filenum){
					n++;
					x++;
				}
				break;
			case KEYSTART:
				set_lcdalign(!LCD_ALIGNMENT);
				mx=WIDTH_X/13;
				my=WIDTH_Y-1;
				n = 0;
				top = 0;
				x = 0;
				y = 0;
				dispfiles(top); //ファイル番号topから一覧を画面表示
				break;
			}
		} while (keystatus2 != KEYFIRE);
		if (n < dirnum){
			// チE��レクトリの場吁E
			if (filenames[n][0] == '.'){
				// 親チE��レクトリの場合、pathから現チE��レクトリを削除
				for (p = path; *p; p++) ;
				for (p -= 2; *p != '/'; p--) ;
				*(p + 1) = 0;
			}
			else{
				// pathにチE��レクトリ名を結合して最後に'/'を付加
				for (p = path; *p; p++) ;
				for (p2 = filenames[n]; *p2;) *p++ = *p2++;
				*p++ = '/';
				*p = 0;
			}
			fr = f_chdir(path); //Change Directory
			if (fr) disperror("Change directory Error.", fr);
		}
		else break; // ファイル選抁E
	}
	cls();
	setcursorcolor(7);
	return filenames[n]; //選択したファイル名へのポインタを返す
}
