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

void LCD_WriteDataColor(unsigned short color)
{
// Write Color Data (RGB565)
    unsigned int d;
	lcd_dc_hi();
	lcd_cs_lo();
	d=(color>>8) | (color<<8);
	spi_write_blocking(LCD_SPICH, (unsigned char *)&d, 2);
	lcd_cs_hi();
}

void LCD_WriteDataColor_notfinish(unsigned short color)
{
// Write Color Data (RGB565) without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
    unsigned int d;
	lcd_dc_hi();
	lcd_cs_lo();
	d=(color>>8) | (color<<8);
	spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
}

/*
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

void LCD_WriteDataN_notfinish(unsigned char *b,int n)
{
// Write Data N bytes, without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking_notfinish(LCD_SPICH, b,n);
}
*/

void LCD_Init()
{
	lcd_cs_hi();
	lcd_dc_hi();

	// Reset controller
	lcd_reset_hi();
	sleep_ms(1);
	lcd_reset_lo();
	sleep_ms(10);
	lcd_reset_hi();
	sleep_ms(120);

//************* Start Initial Sequence **********//
	LCD_WriteComm(0x21); //Inversion ON
	LCD_WriteComm(0xC2);
	LCD_WriteData(0x33);
	LCD_WriteComm(0XC5);
	LCD_WriteData(0x00);
	LCD_WriteData(0x1e);
	LCD_WriteData(0x80);
	LCD_WriteComm(0xB1);
	LCD_WriteData(0xB0);
	LCD_WriteComm(0x48); //Vertical
	LCD_WriteData(0x28);
	LCD_WriteComm(0XE0);
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x18);
	LCD_WriteData(0x04);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x06);
	LCD_WriteData(0x3a);
	LCD_WriteData(0x56);
	LCD_WriteData(0x4d);
	LCD_WriteData(0x03);
	LCD_WriteData(0x0a);
	LCD_WriteData(0x06);
	LCD_WriteData(0x30);
	LCD_WriteData(0x3e);
	LCD_WriteData(0x0f);		
	LCD_WriteComm(0XE1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x18);
	LCD_WriteData(0x01);
	LCD_WriteData(0x11);
	LCD_WriteData(0x06);
	LCD_WriteData(0x38);
	LCD_WriteData(0x34);
	LCD_WriteData(0x4d);
	LCD_WriteData(0x06);
	LCD_WriteData(0x0d);
	LCD_WriteData(0x0b);
	LCD_WriteData(0x31);
	LCD_WriteData(0x37);
	LCD_WriteData(0x0f);
	LCD_WriteComm(0X3A);	//Set Interface Pixel Format
	LCD_WriteData(0x55);
	LCD_WriteComm(0x11);
	sleep_ms(120);
	LCD_WriteComm(0x29);
	X_RES=LCD_COLUMN_RES;
	Y_RES=LCD_ROW_RES;
}

void LCD_WriteData2(unsigned short d)
{
	LCD_WriteData(d>>8);
	LCD_WriteData((unsigned char)d);
}

void LCD_setAddrWindow(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
	if(!(LCD_ALIGNMENT&HORIZONTAL)){
		LCD_WriteComm(0x2a);
		LCD_WriteData2(x);
		LCD_WriteData2(x+w-1);
		LCD_WriteComm(0x2b);
		LCD_WriteData2(y);
		LCD_WriteData2(y+h-1);
	}
	else{
		LCD_WriteComm(0x2a);
		LCD_WriteData2(y);
		LCD_WriteData2(y+h-1);
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
    unsigned int d;
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
    unsigned int d;
	lcd_spi_init_highspeed();
	LCD_setAddrWindow(0,0,X_RES,Y_RES);
	lcd_dc_hi();
	lcd_cs_lo();
	d=(color>>8) | (color<<8);
	for (i=0; i < X_RES*Y_RES ; i++){
		spi_write_blocking_notfinish(LCD_SPICH, (unsigned char *)&d, 2);
	}
	checkSPIfinish();
	lcd_spi_init_normalspeed();
}

void drawPixel(unsigned short x, unsigned short y, unsigned short color)
{
	LCD_SetCursor(x,y);
	LCD_WriteDataColor(color);
}

unsigned short getColor(unsigned short x, unsigned short y)
{
	// Can not read LCD pixel data
	return 0;
}

int attroffset; // TVRAMのカラー情報エリア位置

void lcd_spi_init(void){
	spi_init(LCD_SPICH, LCD_SPI_BAUDRATE);
}

inline void lcd_spi_init_normalspeed(void){
	spi_set_baudrate(LCD_SPICH, LCD_SPI_BAUDRATE);
}

inline void lcd_spi_init_highspeed(void){
	spi_set_baudrate(LCD_SPICH, LCD_SPI_BAUDRATE2);
}

void lcd_display_init(void){
	// Enable SPI and connect to GPIOs
	spi_init(LCD_SPICH, LCD_SPI_BAUDRATE);
	gpio_set_function(LCD_SPI_RX, GPIO_FUNC_SPI);
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
	
	init_textgraph(HORIZONTAL);
	
	// attroffset is always equal to ATTROFFSET
	attroffset=ATTROFFSET;
}
