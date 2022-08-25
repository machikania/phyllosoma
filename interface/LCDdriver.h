/*----------------------------------------------------------------------------

Copyright (C) 2022, KenKen, all right reserved.

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
#define VERTICAL 0
#define HORIZONTAL 1
#define LCD0TURN 0
#define LCD180TURN 2

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
