/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

extern unsigned char g_io_spi_rx;
extern unsigned char g_io_spi_tx;
extern unsigned char g_io_spi_sck;

#define IO_GPIO_8L_MASK (\
		(1<<IO_GPIO0) | (1<<IO_GPIO1) | (1<<IO_GPIO2) | (1<<IO_GPIO3) | \
		(1<<IO_GPIO4) | (1<<IO_GPIO5) | (1<<IO_GPIO6) | (1<<IO_GPIO7) \
	)
#define IO_GPIO_8H_MASK (\
		(1<<IO_GPIO8) | (1<<IO_GPIO9) | (1<<IO_GPIO10) | (1<<IO_GPIO11) | \
		(1<<IO_GPIO12) | (1<<IO_GPIO13) | (1<<IO_GPIO14) | (1<<IO_GPIO15) \
	)
#define IO_GPIO_16_MASK (IO_GPIO_8L_MASK | IO_GPIO_8H_MASK)
#define io_gpio_outl_conversion(a) (a)
#define io_gpio_inl_conversion(a) (a)
#define io_gpio_outh_conversion(a) (\
		(((a)&0x03)<<8) |\
		(((a)&0x1c)<<18) |\
		(((a)&0xe0)<<21) \
	)
#define io_gpio_inh_conversion(a) (\
		(((a)>>8)&0x03) |\
		(((a)>>18)&0x1c) |\
		(((a)>>21)&0xe0) \
	)
#define io_gpio_out16_conversion(a) (\
		io_gpio_outh_conversion((a)>>8) |\
		io_gpio_outl_conversion((a)&0xff) \
	)
#define io_gpio_in16_conversion(a) (\
		io_gpio_inh_conversion(a)<<8 |\
		io_gpio_inl_conversion(a)&0xff \
	)
