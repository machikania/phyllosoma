/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "./compiler.h"
#include "./api.h"
#include "./io.h"

void io_init(void){
	// Clear all GPIO settings and let all ports to be input
	gpio_init_mask(GPIO_ALL_MASK);
}

/*
	KEYS() function
*/

int lib_keys(int r0, int r1, int r2){
	static char init=0;
	unsigned int res,k;
	if (!init) {
		init=1;
		gpio_init_mask(KEYSMASK);
		gpio_set_dir_in_masked(KEYSMASK);
		gpio_pull_up(GPIO_KEYUP);
		gpio_pull_up(GPIO_KEYLEFT);
		gpio_pull_up(GPIO_KEYRIGHT);
		gpio_pull_up(GPIO_KEYDOWN);
		gpio_pull_up(GPIO_KEYSTART);
		gpio_pull_up(GPIO_KEYFIRE);
	}
	k=~gpio_get_all() & KEYSMASK;
	res =(k&KEYUP)    ?  1:0;
	res|=(k&KEYDOWN)  ?  2:0;
	res|=(k&KEYLEFT)  ?  4:0;
	res|=(k&KEYRIGHT) ?  8:0;
	res|=(k&KEYSTART) ? 16:0;
	res|=(k&KEYFIRE)  ? 32:0;
	return res&r0;
}

/*
	PWM statement
*/

int pwm_statement(void){
	g_default_args[2]=1000;
	g_default_args[3]=1;
	return argn_function(LIB_PWM,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		ARG_INTEGER_OPTIONAL<<ARG3);
}

int lib_pwm(int r0, int r1, int r2){
	// r2: duty (0-1000)
	// r1: pulse frequency (Hz); r1=sysclock/((wrap+1)xclkdiv); clkdiv=sysclock/(wrap+1)/r1; clkdiv=125000/r1
	// r0: port to use (1-3)
	unsigned int port,slice,channel;
	
	switch(r0){
		case 1: // Use PWM1
			port=IO_PWM1;
			slice=IO_PWM1_SLICE;
			channel=IO_PWM1_CHANNEL;
			break;
		case 2: // Use PWM2
			port=IO_PWM2;
			slice=IO_PWM2_SLICE;
			channel=IO_PWM2_CHANNEL;
			break;
		case 3: // Use PWM3
			port=IO_PWM3;
			slice=IO_PWM3_SLICE;
			channel=IO_PWM3_CHANNEL;
			break;
		default:
			// Invalid
			return r0;
	}
	// Allocate GPIO to the PWM
    gpio_set_function(port, GPIO_FUNC_PWM);
    // Set clock divier for frequency
    pwm_set_clkdiv(slice,125000.0/((float)r1));
    // 1000 cycles PWM
    pwm_set_wrap(slice, 1000);
	// Set duty
	pwm_set_chan_level(slice, channel, r2);
	// Enable
	pwm_set_enabled(slice, true);
	// All done
	return r0;
}

/*
	ANALOG function
*/
 
int analog_function(void){
	return argn_function(LIB_ANALOG,
		ARG_INTEGER<<ARG1);
}

int lib_analog(int r0, int r1, int r2){
	static char init=0;
	int port,input;
	switch(r0){
		case 13:
		case 26:
			// ADC0
			port=26;
			input=0;
			break;
		case 14:
		case 27:
			// ADC1
			port=27;
			input=1;
			break;
		case 15:
		case 28:
			// ADC2
			port=28;
			input=2;
			break;
		default:
			// Invalid
			return -1;
	}
	if (!init) {
		init=1;
		adc_init();
	}
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(port);
    // Select ADC input 0
    adc_select_input(input);
	// Read
	return adc_read();	
}

/*
	SPI
*/

int spi_statement(void){
	// LIB_SPI_SPI 1
	return 0;
}

int spiwrite_statement(void){
	// LIB_SPI_SPIWRITE 3
	return 0;
}

int spiwritedata_statement(void){
	// LIB_SPI_SPIWRITEDATA 5
	return 0;
}

int spiread_function(void){
	// LIB_SPI_SPIREAD 2
	return 0;
}

int spireaddata_statement(void){
	// LIB_SPI_SPIREADDATA 4
	return 0;
}

int spiswapdata_statement(void){
	// LIB_SPI_SPISWAPDATA 6
	return 0;
}

int lib_spi(int r0, int r1, int r2){
	switch(r2){
		case LIB_SPI_SPI:
		case LIB_SPI_SPIREAD:
		case LIB_SPI_SPIWRITE:
		default:
			// Invalid
			return 0;
	}
}

/*
	I2C
*/

int i2c_statement(void){
	// LIB_I2C_I2C 1
	return 0;
}

int i2cwrite_statement(void){
	// LIB_I2C_I2CWRITE 3
	return 0;
}

int i2cread_function(void){
	// LIB_I2C_I2CREAD 2
	return 0;
}

int i2creaddata_statement(void){
	// LIB_I2C_I2CREADDATA 5
	return 0;
}

int i2cwritedata_statement(void){
	// LIB_I2C_I2CWRITEDATA 6
	return 0;
}

int i2cerror_function(void){
	// LIB_I2C_I2CERROR 4
	return 0;
}

int lib_i2c(int r0, int r1, int r2){
	switch(r2){
		case LIB_I2C_I2C:
		case LIB_I2C_I2CREAD:
		case LIB_I2C_I2CWRITE:
		case LIB_I2C_I2CERROR:
		default:
			// Invalid
			return 0;
	}
}

/*
	SERIAL
*/

int serial_statement(void){
	// LIB_SERIAL_SERIAL 1
	return 0;
}

int serialin_function(void){
	// LIB_SERIAL_SERIALIN 2
	return 0;
}

int serialout_statement(void){
	// LIB_SERIAL_SERIALOUT 3
	return 0;
}

int lib_serial(int r0, int r1, int r2){
	switch(r2){
		case LIB_SERIAL_SERIAL:
		case LIB_SERIAL_SERIALIN:
		case LIB_SERIAL_SERIALOUT:
		default:
			// Invalid
			return 0;
	}
}

/*
	GPIO
*/

int in_function(void){
	return argn_function(LIB_GPIO,
		ARG_INTEGER<<ARG1 |
		LIB_GPIO_IN<<LIBOPTION);
}

int in8h_function(void){
	return argn_function(LIB_GPIO,
		ARG_NONE |
		LIB_GPIO_IN8H<<LIBOPTION);
}

int in8l_function(void){
	return argn_function(LIB_GPIO,
		ARG_NONE |
		LIB_GPIO_IN8L<<LIBOPTION);
}

int in16_function(void){
	return argn_function(LIB_GPIO,
		ARG_NONE |
		LIB_GPIO_IN16<<LIBOPTION);
}

int out_statement(void){
	return argn_function(LIB_GPIO,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		LIB_GPIO_OUT<<LIBOPTION);
}

int out8h_statement(void){
	return argn_function(LIB_GPIO,
		ARG_INTEGER<<ARG1 |
		LIB_GPIO_OUT8H<<LIBOPTION);
}

int out8l_statement(void){
	return argn_function(LIB_GPIO,
		ARG_INTEGER<<ARG1 |
		LIB_GPIO_OUT8L<<LIBOPTION);
}

int out16_statement(void){
	return argn_function(LIB_GPIO,
		ARG_INTEGER<<ARG1 |
		LIB_GPIO_OUT16<<LIBOPTION);
}

int lib_gpio(int r0, int r1, int r2){
	switch(r2){
		case LIB_GPIO_IN:
		case LIB_GPIO_IN8H:
		case LIB_GPIO_IN8L:
		case LIB_GPIO_IN16:
		case LIB_GPIO_OUT:
		case LIB_GPIO_OUT8H:
		case LIB_GPIO_OUT8L:
		case LIB_GPIO_OUT16:
		default:
			// Invalid
			return -1;
	}
}

int io_statements(void){
	if (instruction_is("PWM")) return pwm_statement();
	if (instruction_is("SPI")) return spi_statement();
	if (instruction_is("SPIWRITE")) return spiwrite_statement();
	if (instruction_is("SPIWRITEDATA")) return spiwritedata_statement();
	if (instruction_is("SPIREADDATA")) return spireaddata_statement();
	if (instruction_is("SPISWAPDATA")) return spiswapdata_statement();
	if (instruction_is("I2C")) return i2c_statement();
	if (instruction_is("I2CWRITE")) return i2cwrite_statement();
	if (instruction_is("I2CREADDATA")) return i2creaddata_statement();
	if (instruction_is("SERIAL")) return serial_statement();
	if (instruction_is("SERIALOUT")) return serialout_statement();
	if (instruction_is("OUT")) return out_statement();
	if (instruction_is("OUT8H")) return out8h_statement();
	if (instruction_is("OUT8L")) return out8l_statement();
	if (instruction_is("OUT16")) return out16_statement();
	return ERROR_STATEMENT_NOT_DETECTED;
}

int io_functions(void){
	if (instruction_is("ANALOG(")) return analog_function();
	if (instruction_is("SPIREAD(")) return spiread_function();
	if (instruction_is("I2CREAD(")) return i2cread_function();
	if (instruction_is("I2CERROR(")) return i2cerror_function();
	if (instruction_is("SERIALIN(")) return serialin_function();
	if (instruction_is("IN(")) return in_function();
	if (instruction_is("IN8H(")) return in8h_function();
	if (instruction_is("IN8L(")) return in8l_function();
	if (instruction_is("IN16(")) return in16_function();
	return ERROR_STATEMENT_NOT_DETECTED;
}
