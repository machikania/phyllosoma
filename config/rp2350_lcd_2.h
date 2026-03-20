/*
	Configuration file for Waveshare RP2350-LCD-2

	Peripheral assignment
	GP0 I/O bit0 / PWM3
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / PWM1
	GP3 I/O bit3
	GP4 I/O bit4 / UART TX
	GP5 I/O bit5 / UART RX
	GP6 I/O bit6
	GP7 I/O bit7
	GP8 I/O bit8 / SPI RX / button1 (UP)
	GP9 I/O bit9 / SPI CS / button2 (LEFT)
	GP10 I/O bit10 / SCK / button3 (RIGHT)
	GP11 I/O bit11 / SPI TX / button4 (DOWN)
	GP12 IMU_SDA / TP_SDA / I2C SDA
	GP13 IMU_SCL / TP_SCL / I2C SCL
	GP14 I/O bit14 / IMU_INT1
	GP15 LCD_BL
	GP16 LCD_DC
	GP17 LCD_CS
	GP18 LCD_CLK
	GP19 LCD_DIN(MOSI)
	GP20 LCD_RST
	GP21 SOUND OUT
	GP22 I/O bit12 / button5 (START)
	GP23 I/O bit13 / button6 (FIRE)
	GP24 SD_DO(MISO)
	GP25 SD_CS
	GP26 SD-SCLK
	GP27 SD_DI(MOSI)
	GP28 BAT_ADC / ADC2
	GP29 I/O bit15 / TP_INT
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00100000110000000100111111111111

// IO settings
#define IO_SPI_CS 9
#define IO_SPI_TX 11
#define IO_SPI_RX 8
#define IO_SPI_SCK 10
#define IO_SPI_CH spi1
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI1_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 12
#define IO_I2C_SCL 13
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
#define IO_GPIO6  6
#define IO_GPIO7  7
#define IO_GPIO8  8
#define IO_GPIO9  9
#define IO_GPIO10 10
#define IO_GPIO11 11
#define IO_GPIO12 22
#define IO_GPIO13 23
#define IO_GPIO14 14
#define IO_GPIO15 29

// Buttons settings
#define GPIO_KEYUP 8
#define GPIO_KEYLEFT 9
#define GPIO_KEYRIGHT 10
#define GPIO_KEYDOWN 11
#define GPIO_KEYSTART 22
#define GPIO_KEYFIRE 23

// Audio settings
#define AUDIO_PORT 21
#define AUDIO_SLICE 2
#define AUDIO_CHAN PWM_CHAN_B

// LCD settings
#define LCD_CS 17
#define LCD_DC 16
#define LCD_RESET 20
#define LCD_BL 15
#define LCD_SPI_TX 19
#define LCD_SPI_RX 31 // dummy
#define LCD_SPI_SCK 18
#define LCD_SPICH spi0
#define LCD_COLUMN_RES 240
#define LCD_ROW_RES 320
#define LCD_SPI_BAUDRATE (50*1000*1000)
#define LCD_SPI_BAUDRATE_R (15*1000*1000)

// File system SPI settings
#define SD_SPI_CS 25
#define SD_SPI_TX 27
#define SD_SPI_RX 24
#define SD_SPI_SCK 26
#define SD_SPICH spi1
#define SD_SPI_BAUDRATE (10*1000*1000)

// The port number conversion that is specific to this configuration
#define io_gpio_outh_conversion(a) (\
		(((a)&0x4f)<<8) |\
		(((a)&0x30)<<18) |\
		(((a)&0x80)<<22) \
	)
#define io_gpio_inh_conversion(a) (\
		(((a)>>8)&0x4f) |\
		(((a)>>18)&0x30) |\
		(((a)>>22)&0x80) \
	)
