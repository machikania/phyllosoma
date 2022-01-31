#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "ff.h"

#define MAXFILE 256

// å¥åãEã¿ã³ã®ãããå®ç¾©
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
unsigned short keystatus, keystatus2, keystatus3, oldkey; //ææ°ã®ãã¿ã³ç¶æã¨ååã®ãã¿ã³ç¶æE
int keycountUP, keycountLEFT, keycountRIGHT, keycountDOWN, keycountSTART, keycountFIRE;
int filenum, dirnum;

void wait60thsec(unsigned short n){
	// 60åEEnç§ã¦ã§ã¤ãE
	uint64_t t = to_us_since_boot(get_absolute_time()) % 16667;
	sleep_us(16667 * n - t);
}
void init_buttons(void){
	// ãã¿ã³ç¨GPIOè¨­å®E
	gpio_init_mask(KEYSMASK);
	gpio_set_dir_in_masked(KEYSMASK);
	gpio_pull_up(GPIO_KEYUP);
	gpio_pull_up(GPIO_KEYLEFT);
	gpio_pull_up(GPIO_KEYRIGHT);
	gpio_pull_up(GPIO_KEYDOWN);
	gpio_pull_up(GPIO_KEYSTART);
	gpio_pull_up(GPIO_KEYFIRE);
	keystatus = KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT | KEYSTART | KEYFIRE; //åæåE
}
void keycheck(void){
	//ãã¿ã³ç¶æèª­ã¿åã
	//keystatus :ç¾å¨æ¼ããã¦ãEãã¿ã³ã«å¯¾å¿ããããEãEã«ãã
	//keystatus2:ååæ¼ããã¦ãEªãã¦ãä»åæ¼ããããEã¿ã³ã«å¯¾å¿ããããEãEã«ãã
	//keystatus3:ååæ¼ããã¦ãE¦ãä»åé¢ããããEã¿ã³ã«å¯¾å¿ããããEãEã«ãã
	//keycountXX:XXãã¿ã³ãæ¼ãããç¶æã§é£ç¶ãã¦èª­ã¿è¾¼ã¾ããåæ°
	oldkey = keystatus;
	keystatus = ~gpio_get_all() & KEYSMASK;
	keystatus2 = keystatus & ~oldkey; //ååãã¿ã³ãé¢ãã¦ãEããã§ãE¯
	keystatus3 = ~keystatus & oldkey; //ååãã¿ã³ãæ¼ãã¦ãEããã§ãE¯
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

// ã¨ã©ã¼çªå·ãè¡¨ç¤ºãã¦ãEæ­¢
void disperror(unsigned char *s, FRESULT fr){
	printstr(s);
	printstr(" FRESULT:");
	printnum(fr);
	while (1) ;
}

// filenameséåEã®nçªç®ã®ãã¡ã¤ã«ããä¸è¦§è¡¨ç¤º
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
			// ãE£ã¬ã¯ããª
			setcursorcolor(6);
			printchar('[');
			printstr(filenames[i + n]);
			printchar(']');
			//13æE­ã¾ã§ç©ºç½ã§åããE
			for (j = 11 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else if (i + n < filenum){
			// ãã¡ã¤ã«
			setcursorcolor(7);
			printstr(filenames[i + n]);
			//13æE­ã¾ã§ç©ºç½ã§åããE
			for (j = 13 - strlen(filenames[i + n]); j > 0; j--)
				printchar(' ');
		}
		else for (j = 0; j < 13; j++) printchar(' '); //ç»é¢æå¾ã¾ã§ç©ºç½ã§åããE
		if((i+1)%mx==0 && 13*mx+1<WIDTH_X) printchar('\n');
	}
}

// SDã«ã¼ãåEã®BASICã½ã¼ã¹ãã­ã°ã©ã ä¸è¦§ãè¡¨ç¤ºãE¸æã
// ãã¡ã¤ã«åãè¿ã
unsigned char *fileselect(void){
	FRESULT fr;
	DIR dj;		 // Directory object
	FILINFO fno; // File information
	int n;		 // file number
	int top;	 // ç»é¢åé ­ã®ãã¡ã¤ã«çªå·
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
			// è¦ªãE£ã¬ã¯ããª
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

		fr = f_findfirst(&dj, &fno, path, "*.*"); // å¨ã¦ã®ãã¡ã¤ã«
//		fr = f_findfirst(&dj, &fno, path, "*.BAS"); // BASICã½ã¼ã¹ãã¡ã¤ã«
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
		dispfiles(top); //ãã¡ã¤ã«çªå·topããä¸è¦§ãç»é¢è¡¨ç¤º
		do{
			setcursor(x * 13, y + 1, 5);
			printchar(0x1c); // right arrow
			wait60thsec(2);
			setcursor(x * 13, y + 1, 5);
			printchar(' ');
			keycheck();
			key = keystatus2;
			// 30åä»¥ä¸åããEã¿ã³ãæ¼ãç¶ãã¦ãEã°ãªããEãããã
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
				dispfiles(top); //ãã¡ã¤ã«çªå·topããä¸è¦§ãç»é¢è¡¨ç¤º
				break;
			}
		} while (keystatus2 != KEYFIRE);
		if (n < dirnum){
			// ãE£ã¬ã¯ããªã®å ´åE
			if (filenames[n][0] == '.'){
				// è¦ªãE£ã¬ã¯ããªã®å ´åãpathããç¾ãE£ã¬ã¯ããªãåé¤
				for (p = path; *p; p++) ;
				for (p -= 2; *p != '/'; p--) ;
				*(p + 1) = 0;
			}
			else{
				// pathã«ãE£ã¬ã¯ããªåãçµåãã¦æå¾ã«'/'ãä»å 
				for (p = path; *p; p++) ;
				for (p2 = filenames[n]; *p2;) *p++ = *p2++;
				*p++ = '/';
				*p = 0;
			}
			fr = f_chdir(path); //Change Directory
			if (fr) disperror("Change directory Error.", fr);
		}
		else break; // ãã¡ã¤ã«é¸æE
	}
	cls();
	setcursorcolor(7);
	return filenames[n]; //é¸æãããã¡ã¤ã«åã¸ã®ãã¤ã³ã¿ãè¿ã
}
