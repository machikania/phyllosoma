#ifndef MACHIKANIA_CONFIG_H
#define MACHIKANIA_CONFIG_H

// Selection of environment-specific header must be done in config.cmake
// One of the header files in config directry is selected by following include directive.
#ifdef MACHIKANIA_CONFIG
#include MACHIKANIA_CONFIG
#else
#error MACHIKANIA_CONFIG not defined
#endif

// Environment non-specific settings follow

#define PHYLLOSOMA
#define SYSVER1 "Phyllosoma"
#define SYSVER2 "1.2.2.4"
#define BASVER "KM-1502"

#define INTRODUCE_MACHIKANIA \
	"MachiKania BASIC System\n"\
	" Ver "SYSVER1" "SYSVER2"\n"\
	"BASIC Compiler "BASVER" by Katsumi\n"\
	"LCD, File, & Keyboard systems by KENKEN\n"\
	"\n"

#ifndef KMBASIC_OBJECT_KBYTES
#define KMBASIC_OBJECT_KBYTES 192
#endif

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

extern char g_enable_button_rotation;
extern int LCD_ALIGNMENT;
#define rotate_buttons_data(a) (((~LCD_ALIGNMENT)&g_enable_button_rotation) ? (\
		((a)&KEYUP ? KEYRIGHT:0)|\
		((a)&KEYLEFT ? KEYUP:0)|\
		((a)&KEYRIGHT ? KEYDOWN:0)|\
		((a)&KEYDOWN ? KEYLEFT:0)|\
		((a)&KEYSTART)|\
		((a)&KEYFIRE)\
	):(a))

#endif //MACHIKANIA_CONFIG_H
