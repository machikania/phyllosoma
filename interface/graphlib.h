void clearscreen(void); //ãE­ã¹ãç»é¢ã¯ãªã¢
void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g); //ãE­ã¹ããã¬ãEè¨­å®E
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); //ããã¯ã°ã©ã³ãã«ã©ã¼è¨­å®E
void init_textgraph(unsigned char align); //LCDãE­ã¹ããEã°ã©ãã£ãE¯æ©èEå©ç¨æºå

void textredraw(void);
	// ãE­ã¹ãç»é¢åæç»ããã­ã¹ãERAMã®åE®¹ã«ãããã£ã¦æ¶²æ¶ã«åºåE
void vramscroll(void);
	//1è¡ã¹ã¯ã­ã¼ã«
void vramscrolldown(void);
	//1è¡éE¹ã¯ã­ã¼ã«
void setcursor(unsigned char x,unsigned char y,unsigned char c);
	//ã«ã¼ã½ã«ä½ç½®ã¨ã«ã©ã¼ãè¨­å®E
void setcursorcolor(unsigned char c);
	//ã«ã¼ã½ã«ä½ç½®ããEã¾ã¾ã§ã«ã©ã¼çªå·ãcã«è¨­å®E
void printchar(unsigned char n);
	//ã«ã¼ã½ã«ä½ç½®ã«ãE­ã¹ãã³ã¼ãnãEæE­è¡¨ç¤ºããã«ã¼ã½ã«ãEæE­é²ãã
void printstr(unsigned char *s);
	//ã«ã¼ã½ã«ä½ç½®ã«æE­åEsãè¡¨ç¤º
void printnum(unsigned int n);
	//ã«ã¼ã½ã«ä½ç½®ã«ç¬¦å·ãªãæ´æ°nãE0é²æ°è¡¨ç¤º
void printnum2(unsigned int n,unsigned char e);
	//ã«ã¼ã½ã«ä½ç½®ã«ç¬¦å·ãªãæ´æ°nãeæ¡ãE10é²æ°è¡¨ç¤ºEåã®ç©ºãæ¡E¨åEEã¹ããEã¹ã§åããï¼E
void cls(void);
	//ãE­ã¹ãç»é¢æ¶å»ããã«ã¼ã½ã«ãåEé ­ã«ç§»åE
void startPCG(unsigned char *p,int a);
	// RAMãã©ã³ãï¼ECGEãEå©ç¨éå§ãpããã©ã³ãæ ¼ç´å ´æãaãEä»¥å¤ã§ã·ã¹ãE ãã©ã³ããã³ããE
void stopPCG(void);
	// RAMãã©ã³ãï¼ECGEãEå©ç¨åæ­¢


void g_pset(int x,int y,unsigned char c);
// (x,y)ã®ä½ç½®ã«ã«ã©ã¼cã§ç¹ãæç»

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[]);
// æ¨ªm*ç¸¦nããããEã­ã£ã©ã¯ã¿ã¼ãéÂWx,yã«è¡¨ç¤º
// unsigned char bmp[m*n]éåEã«ãåç´ã«ã«ã©ã¼çªå·ãé«ÂEã
// ã«ã©ã¼çªå·ãEã®é¨åEEéæEè²ã¨ãã¦æ±ãE

void g_clrbmpmn(int x,int y,unsigned char m,unsigned char n);
// ç¸¦m*æ¨ªnããããEã­ã£ã©ã¯ã¿ã¼ç¿éÂ
// ã«ã©ã¼0ã§å¡ãã¤ã¶ãE

void g_gline(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1)-(x2,y2)ã«ã«ã©ã¼cã§ç·åEãæç»

void g_hline(int x1,int x2,int y,unsigned char c);
// (x1,y)-(x2,y)ã¸ã®æ°´å¹³ã©ã¤ã³ãé«éæç»

void g_circle(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)ãä¸­å¿E«ãåå¾rãã«ã©ã¼cã®åEæç»

void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1),(x2,y2)ãå¯¾è§ç·ã¨ããã«ã©ã¼cã§å¡ãããé·æ¹å½¢ãæç»

void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)ãä¸­å¿E«ãåå¾rãã«ã©ã¼cã§å¡ãããåEæç»

void g_putfont(int x,int y,unsigned char c,int bc,unsigned char n);
//8*8ããããEã¢ã«ãã¡ããããã©ã³ãè¡¨ç¤º
//åº§æ¨ï¼E,y)ãã«ã©ã¼çªå·c
//bc:ããã¯ã°ã©ã³ãã«ã©ã¼ãè² æ°ã®å ´åç¡è¦E
//n:æE­çªå·

void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s);
//åº§æ¨Ex,y)ããã«ã©ã¼çªå·cã§æE­åEsãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼

void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n);
//åº§æ¨Ex,y)ã«ã«ã©ã¼çªå·cã§æ°å¤nãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼

void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e);
//åº§æ¨Ex,y)ã«ã«ã©ã¼çªå·cã§æ°å¤nãè¡¨ç¤ºãbc:ããã¯ã°ã©ã³ãã«ã©ã¼ãeæ¡ã§è¡¨ç¤º

unsigned int g_color(int x,int y);
//åº§æ¨Ex,y)ã®è²ãè¿ãEEGB565å½¢å¼ï¼E

void g_clearscreen(void);
// ã°ã©ãã£ãE¯ç»é¢ã¯ãªã¢

void set_lcdalign(unsigned char align);
// æ¶²æ¶ã®ç¸¦æ¨ªè¨­å®E

extern unsigned short palette[];
//ãã¬ãEç¨éåE

extern const unsigned char FontData[];
//ãã©ã³ããã¼ã¿

extern unsigned char TVRAM[];
//ãE­ã¹ãERAM

extern int WIDTH_X; // æ¨ªæ¹åæå­æ°
extern int WIDTH_Y; // ç¸¦æ¹åæå­æ°
#define ATTROFFSET (LCD_COLUMN_RES*LCD_ROW_RES/64) // VRAMä¸ãEã«ã©ã¼ãã¬ãEæ ¼ç´ä½ç½®
