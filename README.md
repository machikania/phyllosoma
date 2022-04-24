# phyllosoma
MachiKania Phyllosoma

## MachiKania Phyllosoma
MachiKania Phyllosoma is a BASIC compiler for ARMv6-M, especially for Raspberry Pi Pico.

## how to compile
cmake and make.

## how to use
Copy "kmbasic.uf2" to the RPI-RP2 drive of Raspberry Pi Pico. Immediately connect to COMx port (com number depends on environment) by serial console at 115200 baud rate.

## License
Most of codes (written in C) are provided with LGPL 2.1 license, but some codes are provided with the other licenses. See the comment of each file.

## Connection

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
