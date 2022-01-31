#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "ff.h"

#define MAXFILE 256

// 蜈･蜉帙・繧ｿ繝ｳ縺ｮ繝薙ャ繝亥ｮ夂ｾｩ
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
unsigned short keystatus, keystatus2, keystatus3, oldkey; //譛譁ｰ縺ｮ繝懊ち繝ｳ迥ｶ諷九→蜑榊屓縺ｮ繝懊ち繝ｳ迥ｶ諷・
int keycountUP, keycountLEFT, keycountRIGHT, keycountDOWN, keycountSTART, keycountFIRE;
int filenum, dirnum;

void wait60thsec(unsigned short n){
	// 60蛻・・n遘偵え繧ｧ繧､繝・
	uint64_t t = to_us_since_boot(get_absolute_time()) % 16667;
	sleep_us(16667 * n - t);
}
void init_buttons(void){
	// 繝懊ち繝ｳ逕ｨGPIO險ｭ螳・
	gpio_init_mask(KEYSMASK);
	gpio_set_dir_in_masked(KEYSMASK);
	gpio_pull_up(GPIO_KEYUP);
	gpio_pull_up(GPIO_KEYLEFT);
	gpio_pull_up(GPIO_KEYRIGHT);
	gpio_pull_up(GPIO_KEYDOWN);
	gpio_pull_up(GPIO_KEYSTART);
	gpio_pull_up(GPIO_KEYFIRE);
	keystatus = KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT | KEYSTART | KEYFIRE; //蛻晄悄蛹・
}
void keycheck(void){
	//繝懊ち繝ｳ迥ｶ諷玖ｪｭ縺ｿ蜿悶ｊ
	//keystatus :迴ｾ蝨ｨ謚ｼ縺輔ｌ縺ｦ縺・ｋ繝懊ち繝ｳ縺ｫ蟇ｾ蠢懊☆繧九ン繝・ヨ繧・縺ｫ縺吶ｋ
	//keystatus2:蜑榊屓謚ｼ縺輔ｌ縺ｦ縺・↑縺上※縲∽ｻ雁屓謚ｼ縺輔ｌ縺溘・繧ｿ繝ｳ縺ｫ蟇ｾ蠢懊☆繧九ン繝・ヨ繧・縺ｫ縺吶ｋ
	//keystatus3:蜑榊屓謚ｼ縺輔ｌ縺ｦ縺・※縲∽ｻ雁屓髮｢縺輔ｌ縺溘・繧ｿ繝ｳ縺ｫ蟇ｾ蠢懊☆繧九ン繝・ヨ繧・縺ｫ縺吶ｋ
	//keycountXX:XX繝懊ち繝ｳ縺梧款縺輔ｌ縺溽憾諷九〒騾｣邯壹＠縺ｦ隱ｭ縺ｿ霎ｼ縺ｾ繧後◆蝗樊焚
	oldkey = keystatus;
	keystatus = ~gpio_get_all() & KEYSMASK;
	keystatus2 = keystatus & ~oldkey; //蜑榊屓繝懊ち繝ｳ繧帝屬縺励※縺・◆縺九メ繧ｧ繝・け
	keystatus3 = ~keystatus & oldkey; //蜑榊屓繝懊ち繝ｳ繧呈款縺励※縺・◆縺九メ繧ｧ繝・け
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

// 繧ｨ繝ｩ繝ｼ逡ｪ蜿ｷ繧定｡ｨ遉ｺ縺励※縺・●豁｢
void disperror(unsigned char *s, FRESULT fr){
	printstr(s);
	printstr(" FRESULT:");
	printnum(fr);
	while (1) ;
}

// filenames驟榊・縺ｮn逡ｪ逶ｮ縺ｮ繝輔ぃ繧､繝ｫ縺九ｉ荳隕ｧ陦ｨ遉ｺ
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
			// 繝・ぅ繝ｬ繧ｯ繝医Μ
			setcursorcolor(6);
			printchar('[');
			printstr(filenames[i + n]);
			printchar(']');
			//13譁・ｭ励∪縺ｧ遨ｺ逋ｽ縺ｧ蝓九ａ繧・
			for (j = 11 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else if (i + n < filenum){
			// 繝輔ぃ繧､繝ｫ
			setcursorcolor(7);
			printstr(filenames[i + n]);
			//13譁・ｭ励∪縺ｧ遨ｺ逋ｽ縺ｧ蝓九ａ繧・
			for (j = 13 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else for (j = 0; j < 13; j++) printchar(' '); //逕ｻ髱｢譛蠕後∪縺ｧ遨ｺ逋ｽ縺ｧ蝓九ａ繧・
		if((i+1)%mx==0 && 13*mx+1<WIDTH_X) printchar('\n');
	}
}

// SD繧ｫ繝ｼ繝牙・縺ｮBASIC繧ｽ繝ｼ繧ｹ繝励Ο繧ｰ繝ｩ繝荳隕ｧ繧定｡ｨ遉ｺ縲・∈謚槭＠
// 繝輔ぃ繧､繝ｫ蜷阪ｒ霑斐☆
unsigned char *fileselect(void){
	FRESULT fr;
	DIR dj;		 // Directory object
	FILINFO fno; // File information
	int n;		 // file number
	int top;	 // 逕ｻ髱｢蜈磯ｭ縺ｮ繝輔ぃ繧､繝ｫ逡ｪ蜿ｷ
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
			// 隕ｪ繝・ぅ繝ｬ繧ｯ繝医Μ
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

		fr = f_findfirst(&dj, &fno, path, "*.*"); // 蜈ｨ縺ｦ縺ｮ繝輔ぃ繧､繝ｫ
//		fr = f_findfirst(&dj, &fno, path, "*.BAS"); // BASIC繧ｽ繝ｼ繧ｹ繝輔ぃ繧､繝ｫ
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
		dispfiles(top); //繝輔ぃ繧､繝ｫ逡ｪ蜿ｷtop縺九ｉ荳隕ｧ繧堤判髱｢陦ｨ遉ｺ
		do{
			setcursor(x * 13, y + 1, 5);
			printchar(0x1c); // right arrow
			wait60thsec(2);
			setcursor(x * 13, y + 1, 5);
			printchar(' ');
			keycheck();
			key = keystatus2;
			// 30蝗樔ｻ･荳雁酔縺倥・繧ｿ繝ｳ繧呈款縺礼ｶ壹￠縺ｦ縺・ｌ縺ｰ繝ｪ繝斐・繝医＆縺帙ｋ
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
				dispfiles(top); //繝輔ぃ繧､繝ｫ逡ｪ蜿ｷtop縺九ｉ荳隕ｧ繧堤判髱｢陦ｨ遉ｺ
				break;
			}
		} while (keystatus2 != KEYFIRE);
		if (n < dirnum){
			// 繝・ぅ繝ｬ繧ｯ繝医Μ縺ｮ蝣ｴ蜷・
			if (filenames[n][0] == '.'){
				// 隕ｪ繝・ぅ繝ｬ繧ｯ繝医Μ縺ｮ蝣ｴ蜷医｝ath縺九ｉ迴ｾ繝・ぅ繝ｬ繧ｯ繝医Μ繧貞炎髯､
				for (p = path; *p; p++) ;
				for (p -= 2; *p != '/'; p--) ;
				*(p + 1) = 0;
			}
			else{
				// path縺ｫ繝・ぅ繝ｬ繧ｯ繝医Μ蜷阪ｒ邨仙粋縺励※譛蠕後↓'/'繧剃ｻ伜刈
				for (p = path; *p; p++) ;
				for (p2 = filenames[n]; *p2;) *p++ = *p2++;
				*p++ = '/';
				*p = 0;
			}
			fr = f_chdir(path); //Change Directory
			if (fr) disperror("Change directory Error.", fr);
		}
		else break; // 繝輔ぃ繧､繝ｫ驕ｸ謚・
	}
	cls();
	setcursorcolor(7);
	return filenames[n]; //驕ｸ謚槭＠縺溘ヵ繧｡繧､繝ｫ蜷阪∈縺ｮ繝昴う繝ｳ繧ｿ繧定ｿ斐☆
}
