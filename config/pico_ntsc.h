/*
	Configuration file for Raspberry Pi Pico + NTSC Video + MMC card

	Peripheral assignment

	GP0 I/O bit0 / PWM3
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / PWM1
	GP3 I/O bit3 / SPI CS
	GP4 I/O bit4 / UART TX
	GP5 I/O bit5 / UART RX
	GP6 I/O bit6 / I2C SDA
	GP7 I/O bit7 / I2C SCL
	GP8 I/O bit8 / button1 (UP)
	GP9 I/O bit9 / button2 (LEFT)
	GP10 
	GP11 
	GP12 
	GP13 
	GP14 
	GP15 NTSC Video out
	GP16 SD-DO(MISO) / SPI RX (pulled up by a 10k ohm resistor)
	GP17 SD-CS
	GP18 SD-SCLK / SCK
	GP19 SD-DI(MOSI) / SPI TX
	GP20 I/O bit10 / button3 (RIGHT)
	GP21 I/O bit11 / button4 (DOWN)
	GP22 I/O bit12 / button5 (START)
	GP26 I/O bit13 / button6 (FIRE) / ADC0
	GP27 I/O bit14 / ADC1
	GP28 I/O bit15 / SOUND OUT / ADC2
	GP29 ADC3
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00111100011100000111111111111111

// IO settings
#define IO_SPI_CS 3
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
#define IO_UART_TX 4
#define IO_UART_RX 5
#define IO_UART_CH uart1
#define IO_UART_IRQ UART1_IRQ
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
#define IO_GPIO16 10
#define IO_GPIO17 11
#define IO_GPIO18 12
#define IO_GPIO19 13
#define IO_GPIO20 14
#define IO_GPIO21 15
#define IO_GPIO22 16
#define IO_GPIO23 17
#define IO_GPIO24 18
#define IO_GPIO25 19
#define IO_GPIO26 23
#define IO_GPIO27 24
#define IO_GPIO28 25
#define IO_GPIO29 29

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

#define NTSC_VIDEO_OUT 15

// File system SPI settings
#define SD_SPI_CS 17
#define SD_SPI_TX 19
#define SD_SPI_RX 16
#define SD_SPI_SCK 18
#define SD_SPICH spi0
#define SD_SPI_BAUDRATE (10*1000*1000)
