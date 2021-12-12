#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "LCDdriver.h"

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
    invalid_params_if(SPI, 0 > (int)len);
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

void LCD_WriteDataN_notfinish(unsigned char *b,int n)
{
// Write Data N bytes, without SPI transfer finished check
// After final data write, you should call checkSPIfinish()
	lcd_dc_hi();
	lcd_cs_lo();
	spi_write_blocking_notfinish(LCD_SPICH, b,n);
}

void LCD_Read(unsigned char com,unsigned char *b,int n){
	lcd_cs_lo();
// Write Command
	lcd_dc_lo();
	spi_write_blocking(LCD_SPICH, &com , 1);
// Read Data
	lcd_dc_hi();
	spi_read_blocking(LCD_SPICH, 0, b, 1); // dummy read
	spi_read_blocking(LCD_SPICH, 0, b, n);
	lcd_cs_hi();
}

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
	LCD_WriteComm(0xCB);
	LCD_WriteData(0x39);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x02);
	LCD_WriteComm(0xCF);
	LCD_WriteData(0x00);
	LCD_WriteData(0XC1);
	LCD_WriteData(0X30);
	LCD_WriteComm(0xE8);
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x78);
	LCD_WriteComm(0xEA);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteComm(0xED);
	LCD_WriteData(0x64);
	LCD_WriteData(0x03);
	LCD_WriteData(0X12);
	LCD_WriteData(0X81);
	LCD_WriteComm(0xF7);
	LCD_WriteData(0x20);
	LCD_WriteComm(0xC0);
	LCD_WriteData(0x23);
	LCD_WriteComm(0xC1);
	LCD_WriteData(0x10);
	LCD_WriteComm(0xC5);
	LCD_WriteData(0x3e);
	LCD_WriteData(0x28);
	LCD_WriteComm(0xC7);
	LCD_WriteData(0x86);
	LCD_WriteComm(0x36);
#if LCD_ALIGNMENT == VERTICAL
	LCD_WriteData(0x48);
#elif LCD_ALIGNMENT == HORIZONTAL
	LCD_WriteData(0x0C);
#endif
	LCD_WriteComm(0x37);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteComm(0x3A);
	LCD_WriteData(0x55);
	LCD_WriteComm(0xB1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x18);
	LCD_WriteComm(0xB6);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x82);
	LCD_WriteData(0x27);
	LCD_WriteData(0x00);
//	LCD_WriteComm(0xF2);
//	LCD_WriteData(0x00);
	LCD_WriteComm(0x26);
	LCD_WriteData(0x01);
	LCD_WriteComm(0xE0);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x3a);
	LCD_WriteData(0x36);
	LCD_WriteData(0x0b);
	LCD_WriteData(0x0d);
	LCD_WriteData(0x06);
	LCD_WriteData(0x4c);
	LCD_WriteData(0x91);
	LCD_WriteData(0x31);
	LCD_WriteData(0x08);
	LCD_WriteData(0x10);
	LCD_WriteData(0x04);
	LCD_WriteData(0x11);
	LCD_WriteData(0x0c);
	LCD_WriteData(0x00);
	LCD_WriteComm(0XE1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x06);
	LCD_WriteData(0x0a);
	LCD_WriteData(0x05);
	LCD_WriteData(0x12);
	LCD_WriteData(0x09);
	LCD_WriteData(0x2c);
	LCD_WriteData(0x92);
	LCD_WriteData(0x3f);
	LCD_WriteData(0x08);
	LCD_WriteData(0x0e);
	LCD_WriteData(0x0b);
	LCD_WriteData(0x2e);
	LCD_WriteData(0x33);
	LCD_WriteData(0x0F);
	LCD_WriteComm(0x11);
	sleep_ms(120);
	LCD_WriteComm(0x29);
}

void LCD_setAddrWindow(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{
#if LCD_ALIGNMENT == VERTICAL
	LCD_WriteComm(0x2a);
	LCD_WriteData2(x);
	LCD_WriteData2(x+w-1);
	LCD_WriteComm(0x2b);
	LCD_WriteData2(y);
	LCD_WriteData2(y+h-1);
#elif LCD_ALIGNMENT == HORIZONTAL
	LCD_WriteComm(0x2a);
	LCD_WriteData2(y);
	LCD_WriteData2(y+h-1);
	LCD_WriteComm(0x2b);
	LCD_WriteData2(x);
	LCD_WriteData2(x+w-1);
#endif
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
