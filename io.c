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
#include "hardware/i2c.h"
#include "./compiler.h"
#include "./api.h"
#include "./io.h"

void io_init(void){
	// Clear all GPIO settings and let all ports to be input
	gpio_init_mask(GPIO_ALL_MASK);
	// Disable PWMs
	pwm_set_enabled(IO_PWM1_SLICE, false);
	pwm_set_enabled(IO_PWM2_SLICE, false);
	pwm_set_enabled(IO_PWM3_SLICE, false);
	// Disable I2C
	i2c_deinit(IO_I2C_CH);
}

int io_prepare_stack(int min_args){
	// Stack is set as r1
	// Number of argument(s) is r0
	// This function will return the number of arguments. If error, return error code (<0).
	int e,n;
	short* sub_sp=object;
	skip_blank();
	if (')'==source[0] && 0==min_args) return 0;
	// Prepare stack
	check_object(1);
	(object++)[0]=0xb080;         // sub	sp, #xx (this will be updated; see below)
	n=0;
	do {
		e=get_integer();
		if (e) return e;
		check_object(1);
		(object++)[0]=0x9000 | n; // str	r0, [sp, #xx]
		n++;
		skip_blank();
	} while (','==(source++)[0]);
	source --;
	if (n<min_args) return ERROR_SYNTAX;
	sub_sp[0]|=n;                 // Update sub sp,#xx assembly
	// Prepare r1 as pointer to the stack
	check_object(1);
	(object++)[0]=0x4669;         // mov	r1, sp
	// R0 is the number of arguments
	e=set_value_in_register(0,n);
	if (e) return e;
	// All done. Return number of arguments	
	return n;
}

int io_call_lib_with_stack(int lib, int option, int min_args){
	int n,e;
	n=io_prepare_stack(min_args);
	if (n<0) return n; // Error
	if (option) {
		// If there is option, set to r2
		e=set_value_in_register(2,option);
		if (e) return e;
	}
	// Call library
	e=call_lib_code(lib);
	if (e) return e;
	// Delete stack
	if (n) {
		check_object(1);
		(object++)[0]=0xb000 | n; // add	sp, #xx
	}
	return 0;
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
	g_default_args[1]=100;
	return argn_function(LIB_I2C,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		LIB_I2C_I2C<<LIBOPTION);
}

int i2cwrite_statement(void){
	return io_call_lib_with_stack(LIB_I2C,LIB_I2C_I2CWRITE,1);
}

int i2cread_function(void){
	return io_call_lib_with_stack(LIB_I2C,LIB_I2C_I2CREAD,1);
}

int i2creaddata_statement(void){
	return io_call_lib_with_stack(LIB_I2C,LIB_I2C_I2CREADDATA,3);
}

int i2cwritedata_statement(void){
	return io_call_lib_with_stack(LIB_I2C,LIB_I2C_I2CWRITEDATA,3);
}

int i2cerror_function(void){
	return argn_function(LIB_I2C,
		ARG_NONE |
		LIB_I2C_I2CERROR<<LIBOPTION);
}

int lib_i2c(int r0, int r1, int r2){
	int i;
	static int s_err=0;
	int* sp;
	unsigned char* i2cdat;
	unsigned char* i2cdat2;
	int num,numdat2;
	unsigned char addr;
	// Preparations
	switch(r2){
		case LIB_I2C_I2CWRITE:
		case LIB_I2C_I2CREAD:
			// Prepare values
			sp=(int*)r1;
			i2cdat=(unsigned char*)&sp[1];  // i2cdat starts at argument 2
			num=r0-1;                       // exclude address for number of I2C data
			addr=(unsigned char)sp[0]&0x7f; // address is 7 bits data
			// Construct byte array
			for(i=0;i<num;i++) i2cdat[i]=(unsigned char)sp[i+1];
			break;
		case LIB_I2C_I2CWRITEDATA:
		case LIB_I2C_I2CREADDATA:
			// Prepare values
			sp=(int*)r1;
			i2cdat=(unsigned char*)&sp[3];  // i2cdat (used for optional writing) starts at argument 4
			i2cdat2=(unsigned char*)sp[1];  // i2cdat2 (used for main writing/reading) is argument 2
			numdat2=sp[2];                  // size of i2cdat2 is argument 3
			num=r0-3;                       // exclude arguments 1, 2 and 3 for number of I2C data
			addr=(unsigned char)sp[0]&0x7f; // address is 7 bits data
			// Construct byte array
			for(i=0;i<num;i++) i2cdat[i]=(unsigned char)sp[i+3];
			break;
		case LIB_I2C_I2C:
		case LIB_I2C_I2CERROR:
		default:
			break;
	}
	// Main jobs
	switch(r2){
		case LIB_I2C_I2C:
			i2c_init(IO_I2C_CH, r0 * 1000);
			gpio_set_function(IO_I2C_SDA, GPIO_FUNC_I2C);
			gpio_set_function(IO_I2C_SCL, GPIO_FUNC_I2C);
			gpio_pull_up(IO_I2C_SDA);
			gpio_pull_up(IO_I2C_SCL);
			return r0;
		case LIB_I2C_I2CERROR:
			// PICO_OK = 0,
			// PICO_ERROR_NONE = 0,
			// PICO_ERROR_TIMEOUT = -1,
			// PICO_ERROR_GENERIC = -2,
			// PICO_ERROR_NO_DATA = -3,
			return s_err;
		case LIB_I2C_I2CWRITE:
			// Write
			num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat,num,false);
			if (num<0) return s_err=num;
			else return s_err=0;
		case LIB_I2C_I2CREAD:
			if (num) {
				// Optional writing before reading
				num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat,num,true);
				if (num<0) return s_err=num;
				else s_err=0;
			}
			// Read a byte
			num=i2c_read_blocking(IO_I2C_CH,addr,&i2cdat[-1],1,false);
			if (num<0) {
				s_err=num;
				return -1;
			} else s_err=0;
			return (unsigned int)i2cdat[-1];
		case LIB_I2C_I2CWRITEDATA:
			if (num) {
				// Optional writing 
				num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat,num,true);
				if (num<0) return s_err=num;
				else s_err=0;
			}
			// Write
			num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat2,numdat2,false);
			if (num<0) return s_err=num;
			else return s_err=0;
		case LIB_I2C_I2CREADDATA:
			if (num) {
				// Optional writing 
				num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat,num,true);
				if (num<0) return s_err=num;
				else s_err=0;
			}
			// Read
			num=i2c_read_blocking(IO_I2C_CH,addr,i2cdat2,numdat2,false);
			if (num<0) return s_err=num;
			else return s_err=0;
		default:
			// Invalid
			return -1;
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
	int i;
	switch(r2){
		case LIB_GPIO_IN:
			if (r0<0 || 15<r0) return -1;
			gpio_init(r0);
			gpio_set_dir(r0,GPIO_IN);
			gpio_pull_up(r0);
			return gpio_get(r0) ? 1:0;
		case LIB_GPIO_IN8H: // TODO: revise positions of bits 8-15
			gpio_init_mask(0xff00);
			gpio_set_dir_in_masked(0xff00);
			for(i=8;i<16;i++) gpio_pull_up(i);
			return (gpio_get_all()>>8) & 0xff;
		case LIB_GPIO_IN8L:
			gpio_init_mask(0xff);
			gpio_set_dir_in_masked(0xff);
			for(i=0;i<8;i++) gpio_pull_up(i);
			return gpio_get_all() & 0xff;
		case LIB_GPIO_IN16: // TODO: revise positions of bits 8-15
			gpio_init_mask(0xff);
			gpio_set_dir_in_masked(0xff);
			for(i=0;i<16;i++) gpio_pull_up(i);
			return gpio_get_all() & 0xffff;
		case LIB_GPIO_OUT:
			if (r1<0 || 15<r1) return r0;
		    gpio_init(r1);
		    gpio_set_dir(r1, GPIO_OUT);
	        gpio_put(r1, r0 ? 1:0);
	        return r0;
		case LIB_GPIO_OUT8H: // TODO: revise positions of bits 8-15
			gpio_init_mask(0xff00);
			gpio_set_dir_out_masked(0xff00);
			gpio_put_masked(0xff00,(r0&0xff)<<8);
			return r0;
		case LIB_GPIO_OUT8L:
			gpio_init_mask(0xff);
			gpio_set_dir_out_masked(0xff);
			gpio_put_masked(0xff,r0&0xff);
			return r0;
		case LIB_GPIO_OUT16: // TODO: revise positions of bits 8-15
			gpio_init_mask(0xffff);
			gpio_set_dir_out_masked(0xffff);
			gpio_put_masked(0xffff,r0&0xffff);
			return r0;
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
