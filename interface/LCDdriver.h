#define LCD_CS 13
#define LCD_DC 10
#define LCD_RESET 11
#define LCD_SPI_TX 15
#define LCD_SPI_RX 12
#define LCD_SPI_SCK 14
#define LCD_SPICH spi1

#define VERTICAL 0
#define HORIZONTAL 1
//#define LCD_ALIGNMENT VERTICAL
#define LCD_ALIGNMENT HORIZONTAL

#if LCD_ALIGNMENT == VERTICAL
	#define X_RES 240 // 横方向解像度
	#define Y_RES 320 // 縦方向解像度
	#define WIDTH_X1 30 // 横方向文字数1
	#define WIDTH_X2 40 // 横方向文字数2(6ドットフォント利用時)
	#define WIDTH_Y 40 // 縦方向文字数
	#define ATTROFFSET1 (WIDTH_X1*WIDTH_Y) // VRAM上のカラーパレット格納位置1
	#define ATTROFFSET2 (WIDTH_X2*WIDTH_Y) // VRAM上のカラーパレット格納位置2
	#define G_X_RES 240 // 横方向解像度
	#define G_Y_RES 320 // 縦方向解像度
#else
	#define X_RES 320 // 横方向解像度
	#define Y_RES 240 // 縦方向解像度
	#define WIDTH_X1 40 // 横方向文字数1
	#define WIDTH_X2 52 // 横方向文字数2(6ドットフォント利用時)
	#define WIDTH_Y 30 // 縦方向文字数
	#define ATTROFFSET1 (WIDTH_X1*WIDTH_Y) // VRAM上のカラーパレット格納位置1
	#define ATTROFFSET2 (WIDTH_X2*WIDTH_Y) // VRAM上のカラーパレット格納位置2
	#define G_X_RES 320 // 横方向解像度
	#define G_Y_RES 240 // 縦方向解像度
#endif

void LCD_WriteComm(unsigned char comm);
void LCD_WriteData(unsigned char data);
void LCD_WriteData2(unsigned short data);
void LCD_WriteDataN(unsigned char *b,int n);
void LCD_WriteData_notfinish(unsigned char data);
void LCD_WriteData2_notfinish(unsigned short data);
void LCD_WriteDataN_notfinish(unsigned char *b,int n);
void checkSPIfinish(void);
void LCD_Init(void);
void LCD_setAddrWindow(unsigned short x,unsigned short y,unsigned short w,unsigned short h);
void LCD_SetCursor(unsigned short x, unsigned short y);
void LCD_Clear(unsigned short color);
void LCD_continuous_output(unsigned short x,unsigned short y,unsigned short color,int n);
void drawPixel(unsigned short x, unsigned short y, unsigned short color);
unsigned short getColor(unsigned short x, unsigned short y);
