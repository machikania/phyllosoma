/*
	Configuration file for Raspberry Pi Pico + Pico-ResTouch-LCD-3.5 + MMC card

	Peripheral assignment

	GP0 I/O bit0 / PWM3
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / SRAM CS
	GP3 I/O bit3 / SPI CS
	GP4 I/O bit4 / UART TX
	GP5 I/O bit5 / UART RX
	GP6 I/O bit6 / I2C SDA
	GP7 I/O bit7 / I2C SCL
	GP8 LCD-DC
	GP9 LCD-CS
	GP10 LCD-SCK / SD-SCLK
	GP11 LCD-MOSI / SD-DI(MOSI)
	GP12 LCD-MISO / SD-DO(MISO)
	GP13 I/O bit8 / LCD-BackLight / PWM1
	GP14 
	GP15 LCD-RESET
	GP16 I/O bit9 / TP-CS
	GP17 I/O bit10 / TP-IRQ / button1 (UP)
	GP18 I/O bit11 / button2 (LEFT)
	GP19 I/O bit12 / button3 (RIGHT)
	GP20 I/O bit13 / button4 (DOWN)
	GP21 I/O bit14 / button5 (START)
	GP22 SD-CS
	GP26 I/O bit15 / button6 (FIRE) / ADC0
	GP27 SOUND OUT / ADC1
	GP28 ADC2
	GP29 ADC3
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00111100001111110110000011111111

// IO settings
#define IO_SPI_CS 3
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI0_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 6
#define IO_I2C_SCL 7
#define IO_I2C_CH i2c1
#define IO_PWM1 13
#define IO_PWM2 1
#define IO_PWM3 0
#define IO_PWM1_SLICE 6
#define IO_PWM2_SLICE 0
#define IO_PWM3_SLICE 0
#define IO_PWM1_CHANNEL PWM_CHAN_B
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
#define IO_GPIO8  13
#define IO_GPIO9  16
#define IO_GPIO10 17
#define IO_GPIO11 18
#define IO_GPIO12 19
#define IO_GPIO13 20
#define IO_GPIO14 21
#define IO_GPIO15 26

// Buttons settings
#define GPIO_KEYUP 17
#define GPIO_KEYLEFT 18
#define GPIO_KEYRIGHT 19
#define GPIO_KEYDOWN 20
#define GPIO_KEYSTART 21
#define GPIO_KEYFIRE 26

// Audio settings
#define AUDIO_PORT 27
#define AUDIO_SLICE 5
#define AUDIO_CHAN PWM_CHAN_B

// LCD settings
#define LCD_CS 9
#define LCD_DC 8
#define LCD_RESET 15
#define LCD_SPI_TX 11
#define LCD_SPI_RX 12
#define LCD_SPI_SCK 10
#define LCD_SPICH spi1
#define LCD_COLUMN_RES 320
#define LCD_ROW_RES 480
#define LCD_SPI_BAUDRATE (16*1000*1000)
#define LCD_SPI_BAUDRATE2 (50*1000*1000)
#define LCD_SPI_BAUDRATE_R (10*1000*1000)

// File system SPI settings
#define SD_SPI_CS 22
#define SD_SPI_TX 11
#define SD_SPI_RX 12
#define SD_SPI_SCK 10
#define SD_SPICH spi1
#define SD_SPI_BAUDRATE (16*1000*1000)

// The port number conversion that is specific to this configuration
#define io_gpio_outh_conversion(a) (\
		(((a)&0x01)<<13) |\
		(((a)&0x7e)<<15) |\
		(((a)&0x80)<<19) \
	)
#define io_gpio_inh_conversion(a) (\
		(((a)>>13)&0x01) |\
		(((a)>>15)&0x7e) |\
		(((a)>>19)&0x80) \
	)
