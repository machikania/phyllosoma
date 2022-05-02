/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

/*
	Periferal assignment

	GP0 I/O bit0 / PWM3
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / PWM1
	GP3 I/O bit3 / SPI CS
	GP4 I/O bit4 / I2C SDA
	GP5 I/O bit5 / I2C SCL
	GP6 I/O bit6 / UART TX
	GP7 I/O bit7 / UART RX
	GP8 I/O bit8 / button1 (UP)
	GP9 I/O bit9 / button2 (LEFT)
	GP10 LCD-DC
	GP11 LCD-RESET
	GP12 LCD-MISO
	GP13 LCD-CS
	GP14 LCD-SCK
	GP15 LCD-MOSI
	GP16 SD-DO / SPI RX
	GP17 SD-CS
	GP18 SD-SCLK / SCK
	GP19 SD-DI / SPI TX
	GP20 I/O bit10 / button3 (RIGHT)
	GP21 I/O bit11 / button4 (DOWN)
	GP22 I/O bit12 / button5 (START)
	GP26 I/O bit13 / button6 (FIRE) / ADC0
	GP27 I/O bit14 / ADC1
	GP28 I/O bit15 / SOUND OUT / ADC2
*/

#define GPIO_ALL_MASK 0b00011100011100000000001111111111

// Button settings
#include "./interface/buttons.h"

// SD card settings (copy from pico-sdmm.c)
#define SD_SPI_CS 17
#define SD_SPI_TX 19
#define SD_SPI_RX 16
#define SD_SPI_SCK 18
#define SD_SPICH spi0
#define SD_SPI_BAUDRATE 10*1000*1000

// SPI settings
#define IO_SPI_CS 3
#define IO_SPI_TX SD_SPI_TX
#define IO_SPI_RX SD_SPI_RX
#define IO_SPI_SCK SD_SPI_SCK
#define IO_SPI_CH SD_SPICH

// I2C settings
#define IO_I2C_SDA 4
#define IO_I2C_SCL 5
#define IO_I2C_CH i2c0

// PWM settings
#define IO_PWM1 2
#define IO_PWM2 1
#define IO_PWM3 0
#define IO_PWM1_SLICE 1
#define IO_PWM2_SLICE 0
#define IO_PWM3_SLICE 0
#define IO_PWM1_CHANNEL PWM_CHAN_A
#define IO_PWM2_CHANNEL PWM_CHAN_B
#define IO_PWM3_CHANNEL PWM_CHAN_A

// Serial settings
#define IO_UART_TX 6
#define IO_UART_RX 7
#define IO_UART_CH 1

// ADC settings
#define IO_ADC0 26
#define IO_ADC1 27
#define IO_ADC2 28

// GPIO settings
#define IO_GPIO0  0
#define IO_GPIO1  1
#define IO_GPIO2  2
#define IO_GPIO3  3
#define IO_GPIO4  4
#define IO_GPIO5  5
#define IO_GPIO6  6
#define IO_GPIO7  7
#define IO_GPIO8  8
#define IO_GPIO9  9
#define IO_GPIO10 20
#define IO_GPIO11 21
#define IO_GPIO12 22
#define IO_GPIO13 26
#define IO_GPIO14 27
#define IO_GPIO15 28

#define IO_GPIO_8L_MASK (\
		(1<<IO_GPIO0) | (1<<IO_GPIO1) | (1<<IO_GPIO2) | (1<<IO_GPIO3) | \
		(1<<IO_GPIO4) | (1<<IO_GPIO5) | (1<<IO_GPIO6) | (1<<IO_GPIO7) \
	)
#define IO_GPIO_8H_MASK (\
		(1<<IO_GPIO8) | (1<<IO_GPIO9) | (1<<IO_GPIO10) | (1<<IO_GPIO11) | \
		(1<<IO_GPIO12) | (1<<IO_GPIO13) | (1<<IO_GPIO14) | (1<<IO_GPIO15) \
	)
#define IO_GPIO_16_MASK (IO_GPIO_8L_MASK | IO_GPIO_8H_MASK)
#define io_gpio_outl_conversion(a) (a)
#define io_gpio_inl_conversion(a) (a)
#define io_gpio_outh_conversion(a) (\
		(((a)&0x03)<<8) |\
		(((a)&0x1c)<<18) |\
		(((a)&0xe0)<<21) \
	)
#define io_gpio_inh_conversion(a) (\
		(((a)>>8)&0x03) |\
		(((a)>>18)&0x1c) |\
		(((a)>>21)&0xe0) \
	)
#define io_gpio_out16_conversion(a) (\
		io_gpio_outh_conversion((a)>>8) |\
		io_gpio_outl_conversion((a)&0xff) \
	)
#define io_gpio_in16_conversion(a) (\
		io_gpio_inh_conversion(a)<<8 |\
		io_gpio_inl_conversion(a)&0xff \
	)
	
