/*
	Configuration file for Waveshare RP2350-LCD-1.47

	Peripheral assignment

	GP0 I/O bit0 / PWM3
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / PWM1
	GP3 I/O bit3 / SPI CS
	GP4 I/O bit4 / UART TX
	GP5 I/O bit5 / UART RX
	GP6 I/O bit6 / I2C SDA
	GP7 I/O bit7 / I2C SCL
	GP8 I/O bit8 / button1 (UP) / SPI RX
	GP9 I/O bit9 / button2 (LEFT)
	GP10 SD-SCLK
	GP11 SD_DI(MOSI)
	GP12 SD_DO(MISO)
	GP13 SD_D1
	GP14 SD_D2
	GP15 SD_CS
	GP16 LCD_DC
	GP17 LCD_CS
	GP18 LCD_CLK
	GP19 LCD_DIN(MOSI)
	GP20 LCD_RST
	GP21 LCD_BL
	GP22 I/O bit15 / RGB_IO(WS2812B)
	GP23 (NC)
	GP24 (NC)
	GP25 I/O bit10 / button3 (RIGHT)
	GP26 I/O bit11 / button4 (DOWN) / ADC0 / SCK
	GP27 I/O bit12 / button5 (START) / ADC1 / SPI TX
	GP28 I/O bit13 / SOUND OUT / ADC2
	GP29 I/O bit14 / button6 (FIRE) / ADC3
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00111110010000000000001111111111

// IO settings
#define IO_SPI_CS 3
#define IO_SPI_TX 27
#define IO_SPI_RX 8
#define IO_SPI_SCK 26
#define IO_SPI_CH spi1
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI1_BASE + SPI_SSPCR0_OFFSET))
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
#define IO_GPIO10 25
#define IO_GPIO11 26
#define IO_GPIO12 27
#define IO_GPIO13 28
#define IO_GPIO14 29
#define IO_GPIO15 22

// Buttons settings
#define GPIO_KEYUP 8
#define GPIO_KEYLEFT 9
#define GPIO_KEYRIGHT 25
#define GPIO_KEYDOWN 26
#define GPIO_KEYSTART 27
#define GPIO_KEYFIRE 29

// Audio settings
#define AUDIO_PORT 28
#define AUDIO_SLICE 6
#define AUDIO_CHAN PWM_CHAN_A

// LCD settings
#define LCD_CS 17
#define LCD_DC 16
#define LCD_RESET 20
#define LCD_BL 21
#define LCD_SPI_TX 19
#define LCD_SPI_RX 31 // dummy
#define LCD_SPI_SCK 18
#define LCD_SPICH spi0
#define LCD_COLUMN_RES 172
#define LCD_ROW_RES 320
#define LCD_SPI_BAUDRATE (50*1000*1000)
#define LCD_SPI_BAUDRATE_R (15*1000*1000)

// File system SPI settings
#define SD_SPI_CS 15
#define SD_SPI_TX 11
#define SD_SPI_RX 12
#define SD_SPI_SCK 10
#define SD_SPICH spi1
#define SD_SPI_BAUDRATE (10*1000*1000)
