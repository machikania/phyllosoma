#define LCD_CS 13
#define LCD_DC 10
#define LCD_RESET 11
#define LCD_SPI_TX 15
#define LCD_SPI_RX 12
#define LCD_SPI_SCK 14
#define LCD_SPICH spi1

#define VERTICAL 0
#define HORIZONTAL 1

#define LCD_COLUMN_RES 240
#define LCD_ROW_RES 320

extern int LCD_ALIGNMENT;
extern int X_RES; // 横方向解像度
extern int Y_RES; // 縦方向解像度

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
