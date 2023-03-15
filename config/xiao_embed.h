/*
	Configuration file for Seeed XIAO RP2040

	Peripheral assignment

	GP0 I/O bit0 / UART TX / PWM3
	GP1 I/O bit1 / UART RX / SPI CS / PWM2
	GP2 I/O bit2 / SPI SCK / PWM1
	GP3 I/O bit3 / SPI MOSI
	GP4 I/O bit4 / SPI MISO
	GP6 I/O bit6 / I2C SDA
	GP7 I/O bit7 / I2C SCL
	GP26 I/O bit13 / button6 (FIRE) / ADC0
	GP27 I/O bit14 / ADC1
	GP28 I/O bit15 / SOUND OUT / ADC2
	GP29 ADC3
*/

#ifdef GPIO_ALL_MASK
#error Including multiple congigure headers.
#endif
#ifdef KMBASIC_COMPILER_H
#ifndef DEBUG_MODE
#error "xiao_embed.h" must be used in DEBUG_MODE.
#endif
#endif

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00011100000000000000000011011111

// IO settings
#define IO_SPI_CS 1
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI0_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 6
#define IO_I2C_SCL 7
#define IO_I2C_CH i2c1
#define IO_PWM1 2
#define IO_PWM2 1
#define IO_PWM3 0
#define IO_PWM1_SLICE 1
#define IO_PWM2_SLICE 0
#define IO_PWM3_SLICE 0
#define IO_PWM1_CHANNEL PWM_CHAN_A
#define IO_PWM2_CHANNEL PWM_CHAN_B
#define IO_PWM3_CHANNEL PWM_CHAN_A
#define IO_UART_TX 0
#define IO_UART_RX 1
#define IO_UART_CH uart0
#define IO_UART_IRQ UART0_IRQ
#define IO_ADC0 26
#define IO_ADC1 27
#define IO_ADC2 28
#define IO_ADC3 29
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

// Buttons settings
#define GPIO_KEYUP 8
#define GPIO_KEYLEFT 9
#define GPIO_KEYRIGHT 20
#define GPIO_KEYDOWN 21
#define GPIO_KEYSTART 22
#define GPIO_KEYFIRE 26

// Audio settings
#define AUDIO_PORT 28
#define AUDIO_SLICE 6
#define AUDIO_CHAN PWM_CHAN_A

// LCD settings
#define LCD_CS 13
#define LCD_DC 10
#define LCD_RESET 11
#define LCD_SPI_TX 15
#define LCD_SPI_RX 12
#define LCD_SPI_SCK 14
#define LCD_SPICH spi1
#define LCD_COLUMN_RES 240
#define LCD_ROW_RES 320
#define LCD_SPI_BAUDRATE (32*1000*1000)
#define LCD_SPI_BAUDRATE_R (15*1000*1000)

// File system SPI settings
#define SD_SPI_CS 17
#define SD_SPI_TX 3
#define SD_SPI_RX 4
#define SD_SPI_SCK 2
#define SD_SPICH spi0
#define SD_SPI_BAUDRATE (10*1000*1000)
