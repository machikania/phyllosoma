/*----------------------------------------------------------------------------

Copyright (C) 2025, KenKen, all right reserved.

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
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"
#include "graphlib.h"
#include "../config.h"

int LCD_ALIGNMENT; // VERTICAL, HORIZONTAL, VERTICAL&LCD180TURN, or HORIZONTAL&LCD180TURN
int X_RES; // 横方向解像度
int Y_RES; // 縦方向解像度

static inline void lcd_cs_lo() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_CS, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void lcd_cs_hi() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_CS, 1);
    asm volatile("nop \n nop \n nop");
}

static inline void lcd_dc_lo() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_DC, 0);
    asm volatile("nop \n nop \n nop");
}
static inline void lcd_dc_hi() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_DC, 1);
    asm volatile("nop \n nop \n nop");
}

static inline void lcd_reset_lo() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_RESET, 0);
    asm volatile("nop \n nop \n nop");
}
static inline void lcd_reset_hi() {
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_RESET, 1);
    asm volatile("nop \n nop \n nop");
}

int __not_in_flash_func(spi_write_blocking_notfinish)(spi_inst_t *spi, const uint8_t *src, size_t len) {
//    invalid_params_if(SPI, 0 > (int)len);
    // Write to TX FIFO whilst ignoring RX, then clean up afterward. When RX
    // is full, PL022 inhibits RX pushes, and sets a sticky flag on
    // push-on-full, but continues shifting. Safe if SSPIMSC_RORIM is not set.
    for (size_t i = 0; i < len; ++i) {
        while (!spi_is_writable(spi))
            tight_loop_contents();
        spi_get_hw(spi)->dr = (uint32_t)src[i];
    }
    return (int)len;
}

void __not_in_flash_func(checkSPIfinish)(void) {
	// Drain RX FIFO, then wait for shifting to finish (which may be *after*
	// TX FIFO drains), then drain RX FIFO again
	while (spi_is_readable(LCD_SPICH))
		(void)spi_get_hw(LCD_SPICH)->dr;
	while (spi_get_hw(LCD_SPICH)->sr & SPI_SSPSR_BSY_BITS)
		tight_loop_contents();
	while (spi_is_readable(LCD_SPICH))
		(void)spi_get_hw(LCD_SPICH)->dr;

	// Don't leave overrun flag set
	spi_get_hw(LCD_SPICH)->icr = SPI_SSPICR_RORIC_BITS;
	lcd_cs_hi();
}

void LCD_WriteComm(unsigned char comm){
// Write Command
	lcd_dc_lo();
	lcd_cs_lo();
	spi_write_blocking(LCD_SPICH, &comm , 1);
	lcd_cs_hi();
}

void LCD_WriteData(unsigned char data)
{
// Write Data
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking(LCD_SPICH, &data , 1);
	lcd_cs_hi();
}

void LCD_WriteData2(unsigned short data)
{
// Write Data 2 bytes
    unsigned short d;
	lcd_dc_hi();
	lcd_cs_lo();
    d=(data>>8) | (data<<8);
	spi_write_blocking(LCD_SPICH, (unsigned char *)&d, 2);
	lcd_cs_hi();
}

void LCD_WriteDataColor(unsigned short data)
{
// Write Color Data  (Equal to LCD_WriteData2)
    unsigned short d;
	lcd_dc_hi();
	lcd_cs_lo();
    d=(data>>8) | (data<<8);
	spi_write_blocking(LCD_SPICH, (unsigned char *)&d, 2);
	lcd_cs_hi();
}

void LCD_WriteDataN(unsigned char *b,int n)
{
// Write Data N bytes
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking(LCD_SPICH, b,n);
	lcd_cs_hi();
}

void LCD_WriteData_notfinish(unsigned char data)
{
// Write Data, without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking_notfinish(LCD_SPICH, &data , 1);
}

void LCD_WriteData2_notfinish(unsigned short data)
{
// Write Data 2 bytes, without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
    unsigned short d;
	lcd_dc_hi();
	lcd_cs_lo();
	d=(data>>8) | (data<<8);
	spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
}

void LCD_WriteDataColor_notfinish(unsigned short data)
{
// Write Color Data , without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
// Equal to LCD_WriteData2_notfinish
    unsigned short d;
	lcd_dc_hi();
	lcd_cs_lo();
	d=(data>>8) | (data<<8);
	spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
}

void LCD_WriteDataN_notfinish(unsigned char *b,int n)
{
// Write Data N bytes, without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking_notfinish(LCD_SPICH, b,n);
}

void sw_read_blocking(unsigned char *p,int n)
// Read Data N bytes from 'SPI_TX' pin by software SPI
{
	for(;n>0;n--){
		for(int i=0;i<8;i++){
		    gpio_put(LCD_SPI_SCK, 0);
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
		    gpio_put(LCD_SPI_SCK, 1);
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
		    asm volatile("nop \n nop \n nop");
			*p<<=1;
			if(gpio_get(LCD_SPI_TX)) *p+=1;
		}
		p++;
	}
}

void LCD_Read(unsigned char com,unsigned char *b,int n){
	lcd_cs_lo();
// Write Command
	lcd_dc_lo();
	spi_write_blocking(LCD_SPICH, &com , 1);
// Read Data
	lcd_dc_hi();

// Temporarily disable the hardware SPI
// and initialize SCK/TX port for software SPI
	gpio_init(LCD_SPI_SCK);
	gpio_put(LCD_SPI_SCK, 0);
	gpio_set_dir(LCD_SPI_SCK, GPIO_OUT);
	gpio_init(LCD_SPI_TX);
	gpio_set_dir(LCD_SPI_TX, GPIO_IN);

	sw_read_blocking(b, 1); // dummy read
	sw_read_blocking(b, n);

	lcd_cs_hi();

	// Reconfigure the hardware SPI.
	gpio_set_function(LCD_SPI_TX, GPIO_FUNC_SPI);
	gpio_set_function(LCD_SPI_SCK, GPIO_FUNC_SPI);
}

void LCD_Init()
{
	// Reset controller
	lcd_reset_hi();
	sleep_ms(100);
	lcd_reset_lo();
	sleep_ms(100);
	lcd_reset_hi();
	sleep_ms(1);

//************* Start Initial Sequence **********//
	LCD_WriteComm(0x11);
	sleep_ms(120);
//	LCD_WriteComm(0x36);

	LCD_WriteComm(0x3A);
	LCD_WriteData(0x05);

	LCD_WriteComm(0xB2);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x33);
	LCD_WriteData(0x33);

	LCD_WriteComm(0xB7);
	LCD_WriteData(0x35);

	LCD_WriteComm(0xBB);
	LCD_WriteData(0x35);

	LCD_WriteComm(0xC0);
	LCD_WriteData(0x00);
//	LCD_WriteData(0x2C);

	LCD_WriteComm(0xC2);
	LCD_WriteData(0x01);

	LCD_WriteComm(0xC3);
	LCD_WriteData(0x13);

	LCD_WriteComm(0xC4);
	LCD_WriteData(0x20);

	LCD_WriteComm(0xC6);
	LCD_WriteData(0x0F);

	LCD_WriteComm(0xD0);
	LCD_WriteData(0xA4);
	LCD_WriteData(0xA1);

	LCD_WriteComm(0xD6);
	LCD_WriteData(0xA1);

	LCD_WriteComm(0xE0);
	LCD_WriteData(0xF0);
	LCD_WriteData(0x00);
	LCD_WriteData(0x04);
	LCD_WriteData(0x04);
	LCD_WriteData(0x04);
	LCD_WriteData(0x05);
	LCD_WriteData(0x29);
	LCD_WriteData(0x33);
	LCD_WriteData(0x3E);
	LCD_WriteData(0x38);
	LCD_WriteData(0x12);
	LCD_WriteData(0x12);
	LCD_WriteData(0x28);
	LCD_WriteData(0x30);

	LCD_WriteComm(0xE1);
	LCD_WriteData(0xF0);
	LCD_WriteData(0x07);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x0B);
	LCD_WriteData(0x07);
	LCD_WriteData(0x28);
	LCD_WriteData(0x33);
	LCD_WriteData(0x3E);
	LCD_WriteData(0x36);
	LCD_WriteData(0x14);
	LCD_WriteData(0x14);
	LCD_WriteData(0x29);
	LCD_WriteData(0x32);

	LCD_WriteComm(0x21); // invert

	LCD_WriteComm(0x11);
	sleep_ms(120);
	LCD_WriteComm(0x29);
	X_RES=LCD_COLUMN_RES;
	Y_RES=LCD_ROW_RES;
}

void LCD_setAddrWindow(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
	if(!(LCD_ALIGNMENT&HORIZONTAL)){
		LCD_WriteComm(0x2a);
		LCD_WriteData2(x+34);
		LCD_WriteData2(x+34+w-1);
		LCD_WriteComm(0x2b);
		LCD_WriteData2(y);
		LCD_WriteData2(y+h-1);
	}
	else{
		LCD_WriteComm(0x2a);
		LCD_WriteData2(y+34);
		LCD_WriteData2(y+34+h-1);
		LCD_WriteComm(0x2b);
		LCD_WriteData2(x);
		LCD_WriteData2(x+w-1);
	}
	LCD_WriteComm(0x2c);
}

void LCD_SetCursor(unsigned short x, unsigned short y)
{
	LCD_setAddrWindow(x,y,X_RES-x,1);
}

void LCD_continuous_output(unsigned short x,unsigned short y,unsigned short color,int n)
{
	//High speed continuous output
	int i;
    unsigned short d;
	LCD_setAddrWindow(x,y,n,1);
	lcd_dc_hi();
	lcd_cs_lo();
	d=(color>>8) | (color<<8);
	for (i=0; i < n ; i++){
		spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
	}
	checkSPIfinish();
}
void LCD_Clear(unsigned short color)
{
	int i;
    unsigned short d;
	LCD_setAddrWindow(0,0,X_RES,Y_RES);
	lcd_dc_hi();
	lcd_cs_lo();
	d=(color>>8) | (color<<8);
	for (i=0; i < X_RES*Y_RES ; i++){
		spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
	}
	checkSPIfinish();
}

void drawPixel(unsigned short x, unsigned short y, unsigned short color)
{
	LCD_SetCursor(x,y);
	LCD_WriteData2(color);
}

unsigned short getColor(unsigned short x, unsigned short y)
{
	unsigned int d=0;
	LCD_SetCursor(x,y);
	LCD_Read(0x2e, (unsigned char *)&d, 3);
	return ((d&0xf8)<<8)|((d&0xfc00)>>5)|((d&0xf80000)>>19); //RGB565 format
}

int attroffset; // TVRAMのカラー情報エリア位置

void lcd_spi_init(void){
	spi_init(LCD_SPICH, LCD_SPI_BAUDRATE);
	gpio_init(LCD_BL);
	gpio_put(LCD_BL, 1);
	gpio_set_dir(LCD_BL, GPIO_OUT);
}

void lcd_display_init(void){
	// Enable SPI and connect to GPIOs
	spi_init(LCD_SPICH, LCD_SPI_BAUDRATE);
//	gpio_set_function(LCD_SPI_RX, GPIO_FUNC_SPI);
	gpio_set_function(LCD_SPI_TX, GPIO_FUNC_SPI);
	gpio_set_function(LCD_SPI_SCK, GPIO_FUNC_SPI);
	
	gpio_init(LCD_CS);
	gpio_put(LCD_CS, 1);
	gpio_set_dir(LCD_CS, GPIO_OUT);
	gpio_init(LCD_DC);
	gpio_put(LCD_DC, 1);
	gpio_set_dir(LCD_DC, GPIO_OUT);
	gpio_init(LCD_RESET);
	gpio_put(LCD_RESET, 1);
	gpio_set_dir(LCD_RESET, GPIO_OUT);
	gpio_init(LCD_BL);
	gpio_put(LCD_BL, 1);
	gpio_set_dir(LCD_BL, GPIO_OUT);
	
	init_textgraph(HORIZONTAL);

	// attroffset is always equal to ATTROFFSET
	attroffset=ATTROFFSET;
}
