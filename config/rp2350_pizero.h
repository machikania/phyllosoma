/*
	Configuration file for Waveshare RP2350-PiZero

	Peripheral assignment

	GP0 I/O bit0 / PWM1
	GP1 I/O bit1 / PWM2
	GP2 I/O bit2 / I2C SDA...I2C1
	GP3 I/O bit3 / I2C SCL...I2C1
	GP4 I/O bit4 / UART TX...UART1
	GP5 I/O bit5 / UART RX...UART1
	GP6 I/O bit6 / button4 (DOWN)
	GP7 I/O bit7 / PWM3
	GP8 I/O bit8 / SPI CS (default)
	GP9 I/O bit9
	GP10 I/O bit10 / SPI SCK...SPI1
	GP11 I/O bit11 / SPI TX(MOSI)...SPI1
	GP12 I/O bit12 / SPI RX(MISO)...SPI1
	GP13 I/O bit13 / button2 (LEFT)
	GP14 I/O bit14 / button6 (FIRE)
	GP15 I/O bit15 / button1 (UP)
	GP16 I/O bit16
	GP17 I/O bit17
	GP18 I/O bit18
	GP19 I/O bit19 / button3 (RIGHT)
	GP20 I/O bit20
	GP21 I/O bit21 / button5 (START)
	GP22 I/O bit22
	GP23 I/O bit23
	GP24 I/O bit24
	GP25 I/O bit25
	GP26 I/O bit26
	GP27 I/O bit27 / SOUND OUT

	GP28 PIO-USB-P
	GP29 PIO-USB-N
	GP30 SD-SCLK....SPI1
	GP31 SD-DI(MOSI)...SPI1
	GP32 DVI-D2-P
	GP33 DVI-D2-N
	GP34 DVI-D1-P
	GP35 DVI-D1-N
	GP36 DVI-D0-P
	GP37 DVI-D0-N
	GP38 DVI-CLK-P
	GP39 DVI-CLK-N
	GP40 SD-DO(MISO)...SPI1 / (ADC0)
	GP41 (SD-D1) / (ADC1)
	GP42 (SD-D2) / (ADC2)
	GP43 SD-CS / (ADC3)
	GP44 DVI-SDA / (ADC4)
	GP45 DVI-SCL / (ADC5)
	GP46 DVI-CEC / (ADC6)
	GP47 (PSRAM-CS) / (ADC7)
*/

//                     31     24      16       8       0
//                      |      |       |       |       |
#define GPIO_ALL_MASK 0b00001111111111111111111111111111

// IO settings
#define IO_SPI_CS 8
#define IO_SPI_SSPCR0 ((volatile unsigned int*)(SPI1_BASE + SPI_SSPCR0_OFFSET))
#define IO_I2C_SDA 2
#define IO_I2C_SCL 3
#define IO_I2C_CH i2c1
#define IO_PWM1 0
#define IO_PWM2 1
#define IO_PWM3 7
#define IO_PWM1_SLICE 0
#define IO_PWM2_SLICE 0
#define IO_PWM3_SLICE 3
#define IO_PWM1_CHANNEL PWM_CHAN_A
#define IO_PWM2_CHANNEL PWM_CHAN_B
#define IO_PWM3_CHANNEL PWM_CHAN_B
#define IO_UART_TX 4
#define IO_UART_RX 5
#define IO_UART_CH uart1
#define IO_UART_IRQ UART1_IRQ
#define IO_ADC0 40
#define IO_ADC1 41
#define IO_ADC2 42
#define IO_ADC3 43
#define IO_ADC4 44
#define IO_ADC5 45
#define IO_ADC6 46
#define IO_ADC7 47
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
#define IO_GPIO28 27
#define IO_GPIO29 27

// Buttons settings
#define GPIO_KEYUP 15
#define GPIO_KEYLEFT 13
#define GPIO_KEYRIGHT 19
#define GPIO_KEYDOWN 6
#define GPIO_KEYSTART 21
#define GPIO_KEYFIRE 14

// Audio settings
#define AUDIO_PORT 27
#define AUDIO_SLICE 5
#define AUDIO_CHAN PWM_CHAN_B

// Null LCD SPI setting
#define LCD_SPICH 0
#define LCD_SPI_RX 0

// File system SPI settings
#define SD_SPI_CS 43
#define SD_SPI_TX 31
#define SD_SPI_RX 40
#define SD_SPI_SCK 30
#define SD_SPICH spi1
#define SD_SPI_BAUDRATE (10*1000*1000)

#define KMBASIC_OBJECT_KBYTES 192

// The port number conversion that is specific to this configuration
#define io_gpio_outl_conversion(a) (a)
#define io_gpio_inl_conversion(a) (a)
#define io_gpio_outh_conversion(a) (((a)&0xff)<<8)
#define io_gpio_inh_conversion(a) (((a)>>8)&0xff)
