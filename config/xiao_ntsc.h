/*
	Configuration file for Seeed XIAO RP2040

	Peripheral assignment

	GP0 I/O bit0 / UART TX / PWM1
	GP1 I/O bit1 / button5 (START) / UART RX / PWM2
	GP2 SD-SCLK / SCK
	GP3 SD-DI(MOSI) / SPI TX
	GP4 SD-DO(MISO) / SPI RX (pulled up by a 10k ohm resistor)
	GP5 I/O bit5 / button6 (FIRE)
	GP6 SD-CS
	GP7 NTSC Video out
	GP8 I/O bit8 / PWM3
	GP9 I/O bit9
	GP10 I/O bit10
	GP11 I/O bit11
	GP12 I/O bit12 / button1 (UP)
	GP13 I/O bit13 / button2 (LEFT)
	GP14 I/O bit14 / button3 (RIGHT)
	GP15 I/O bit15 / button4 (DOWN)
	GP16 I/O bit16
	GP17 I/O bit17
	GP18 I/O bit18
	GP19 I/O bit19
	GP20 I/O bit20
	GP21 I/O bit21
	GP22 I/O bit22
	GP23 I/O bit23
	GP24 I/O bit24
	GP25 I/O bit25
	GP26 I/O bit26 / ADC0 / I2C SDA
	GP27 I/O bit27 / ADC1 / I2C SCL
	GP28 I/O bit28 / SOUND OUT / ADC2
	GP29 I/O bit29 / ADC3 / SPI CS (default)
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00111111111111111111111100100011

// IO settings
#define IO_SPI_CS 29
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI0_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 26
#define IO_I2C_SCL 27
#define IO_I2C_CH i2c1
#define IO_PWM1 0
#define IO_PWM2 1
#define IO_PWM3 8
#define IO_PWM1_SLICE 0
#define IO_PWM2_SLICE 0
#define IO_PWM3_SLICE 4
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
#define IO_GPIO2  21
#define IO_GPIO3  21
#define IO_GPIO4  21
#define IO_GPIO5  5
#define IO_GPIO6  21
#define IO_GPIO7  21
#define IO_GPIO8  8
#define IO_GPIO9  9
#define IO_GPIO10 10
#define IO_GPIO11 11
#define IO_GPIO12 12
#define IO_GPIO13 13
#define IO_GPIO14 14
#define IO_GPIO15 15
#define IO_GPIO16 16
#define IO_GPIO17 17
#define IO_GPIO18 18
#define IO_GPIO19 19
#define IO_GPIO20 20
#define IO_GPIO21 21
#define IO_GPIO22 22
#define IO_GPIO23 23
#define IO_GPIO24 24
#define IO_GPIO25 25
#define IO_GPIO26 26
#define IO_GPIO27 27
#define IO_GPIO28 28
#define IO_GPIO29 29

// Buttons settings
#define GPIO_KEYUP 12
#define GPIO_KEYLEFT 13
#define GPIO_KEYRIGHT 14
#define GPIO_KEYDOWN 15
#define GPIO_KEYSTART 1
#define GPIO_KEYFIRE 5

// Audio settings
#define AUDIO_PORT 28
#define AUDIO_SLICE 6
#define AUDIO_CHAN PWM_CHAN_A

#define NTSC_VIDEO_OUT 7

// File system SPI settings
#define SD_SPI_CS 6
#define SD_SPI_TX 3
#define SD_SPI_RX 4
#define SD_SPI_SCK 2
#define SD_SPICH spi0
#define SD_SPI_BAUDRATE (10*1000*1000)

#define KMBASIC_OBJECT_KBYTES 192

// Disable all port number conversion
#undef io_gpio_outh_conversion
#undef io_gpio_inh_conversion
#undef io_gpio_out16_conversion
#undef io_gpio_in16_conversion
#define io_gpio_outh_conversion(a) (((a)&0xff)<<8)
#define io_gpio_inh_conversion(a) (((a)>>8)&0xff)
#define io_gpio_out16_conversion(a) (a)
#define io_gpio_in16_conversion(a) (a)
