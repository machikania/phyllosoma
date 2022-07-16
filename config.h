// Select environment-specific header below

// Raspberry Pi Pico + ILI9341 LCD
// YD-RP2040 + ILI9341 LCD
#include "./config/pico_ili9341.h"

// Environment non-specific settings follow

#define PHYLLOSOMA
#define SYSVER1 "Phyllosoma"
#define SYSVER2 "0.9.0"
#define BASVER "KM-1500"

#define IO_SPI_TX SD_SPI_TX
#define IO_SPI_RX SD_SPI_RX
#define IO_SPI_SCK SD_SPI_SCK
#define IO_SPI_CH SD_SPICH

#define KEYUP (1 << GPIO_KEYUP)
#define KEYLEFT (1 << GPIO_KEYLEFT)
#define KEYRIGHT (1 << GPIO_KEYRIGHT)
#define KEYDOWN (1 << GPIO_KEYDOWN)
#define KEYSTART (1 << GPIO_KEYSTART)
#define KEYFIRE (1 << GPIO_KEYFIRE)
#define KEYSMASK (KEYUP | KEYLEFT | KEYRIGHT | KEYDOWN | KEYSTART | KEYFIRE)
