/*
	Configuration file for Raspberry Pi Pico + ILI9488 LCD + MMC card

	Peripheral assignment

	GP0 I/O bit0 / PWM3 / I2C SDA / button1 (UP)
	GP1 I/O bit1 / PWM2 / I2C SCL / button2 (LEFT)
	GP2 I/O bit2 / PWM1 / button3 (RIGHT)
	GP3 I/O bit3 / SPI CS / button4 (DOWN)
	GP4 I/O bit4 / UART TX / button5 (START)
	GP5 I/O bit5 / UART RX / button6 (FIRE)
	GP6 I2C1_SDA (Keyboard)
	GP7 I2C1_SCL (Keyboard)
	GP8 I/O bit6 / I/O bit8
	GP9 I/O bit7 / I/O bit9
	GP10 SPI1_SCK（LCD）
	GP11 SPI1_TX（LCD）
	GP12 SPI1_RX（LCD）
	GP13 SPI1_CS（LCD）
	GP14 LCD_DC（LCD）
	GP15 LCD_RST（LCD）
	GP16 SPI0_RX（MMC）/ SPI RX (pulled up by a 10k ohm resistor)
	GP17 SPI0_CS（MMC）
	GP18 SPI0_SCK（MMC）
	GP19 SPI0_TX（MMC）
	GP20 I/O bit10
	GP21 I/O bit11
	GP22 I/O bit12
	GP26 I/O bit13 / ADC0
	GP27 I/O bit14 / SOUND OUT / ADC1
	GP28 I/O bit15 / ADC2
	GP29 ADC3
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00011100011100000000001100111111

// IO settings
#define IO_SPI_CS 3
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI0_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 0
#define IO_I2C_SCL 1
#define IO_I2C_CH i2c0
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
#define IO_GPIO6  8
#define IO_GPIO7  9
#define IO_GPIO8  8
#define IO_GPIO9  9
#define IO_GPIO10 20
#define IO_GPIO11 21
#define IO_GPIO12 22
#define IO_GPIO13 26
#define IO_GPIO14 27
#define IO_GPIO15 28

// Buttons settings
#define GPIO_KEYUP 0
#define GPIO_KEYLEFT 1
#define GPIO_KEYRIGHT 2
#define GPIO_KEYDOWN 3
#define GPIO_KEYSTART 4
#define GPIO_KEYFIRE 5

// Audio settings
#define AUDIO_PORT 27
#define AUDIO_SLICE 5
#define AUDIO_CHAN PWM_CHAN_B

// LCD settings
#define LCD_CS 13
#define LCD_DC 14
#define LCD_RESET 15
#define LCD_SPI_TX 11
#define LCD_SPI_RX 12
#define LCD_SPI_SCK 10
#define LCD_SPICH spi1
#define LCD_COLUMN_RES 320
#define LCD_ROW_RES 320
#define LCD_SPI_BAUDRATE (32*1000*1000)
#define LCD_SPI_BAUDRATE_R (15*1000*1000)

// File system SPI settings
#define SD_SPI_CS 17
#define SD_SPI_TX 19
#define SD_SPI_RX 16
#define SD_SPI_SCK 18
#define SD_SPICH spi0
#define SD_SPI_BAUDRATE (10*1000*1000)
