/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "./compiler.h"
#include "./api.h"
#include "./io.h"
#include "./config.h"

unsigned char g_io_spi_rx=IO_SPI_RX;
unsigned char g_io_spi_tx=IO_SPI_TX;
unsigned char g_io_spi_sck=IO_SPI_SCK;
void* g_io_spi_ch=IO_SPI_CH;
volatile unsigned int* g_io_spi_sspcr=IO_SPI_SSPCR0;

int ini_file_io(char* line){
	int i;
	if (!strncmp(line,"SPIMISO=",8)) {
		i=atoi(line+8);
		if (0==i || 4==i | 16==i) g_io_spi_rx=i;
		if (8==i || 12==i) g_io_spi_rx=i;
	} else if (!strncmp(line,"SPIMOSI=",8)) {
		i=atoi(line+8);
		if (3==i || 7==i | 19==i) g_io_spi_tx=i;
		if (11==i || 15==i) g_io_spi_tx=i;
	} else if (!strncmp(line,"SPICLK=",7)) {
		i=atoi(line+7);
		if (2==i || 6==i | 18==i) g_io_spi_sck=i;
		if (10==i || 14==i) {
			g_io_spi_sck=i;
			g_io_spi_ch=spi1;
			g_io_spi_sspcr=((volatile unsigned int*)(SPI1_BASE + SPI_SSPCR0_OFFSET));
		}
	} else return 0;
	return 1;
}

static unsigned char gpio_table[]={
	IO_GPIO0,
	IO_GPIO1,
	IO_GPIO2,
	IO_GPIO3,
	IO_GPIO4,
	IO_GPIO5,
	IO_GPIO6,
	IO_GPIO7,
	IO_GPIO8,
	IO_GPIO9,
	IO_GPIO10,
	IO_GPIO11,
	IO_GPIO12,
	IO_GPIO13,
	IO_GPIO14,
	IO_GPIO15
#ifdef IO_GPIO16
	,
	IO_GPIO16,
	IO_GPIO17,
	IO_GPIO18,
	IO_GPIO19,
	IO_GPIO20,
	IO_GPIO21,
	IO_GPIO22,
	IO_GPIO23,
	IO_GPIO24,
	IO_GPIO25,
	IO_GPIO26,
	IO_GPIO27,
	IO_GPIO28,
	IO_GPIO29	
#endif
};

// UART data
static unsigned short* io_uart_buff;
static int io_uart_var;
static int io_uart_buff_size;
static int io_uart_buff_read_pos;
static int io_uart_buff_write_pos;

// SPI data
static unsigned int io_spi_sspcr[2];
static unsigned int sd_spi_sspcr[2];

void io_init(void){
	int i;
	// Clear all GPIO settings and let all ports to be input
	gpio_init_mask(GPIO_ALL_MASK);
	// All pull up
	for(i=0;i<29;i++){
		if (GPIO_ALL_MASK & (1<<i) ) gpio_pull_up(i);
	}
	// Disable PWMs
	pwm_set_enabled(IO_PWM1_SLICE, false);
	pwm_set_enabled(IO_PWM2_SLICE, false);
	pwm_set_enabled(IO_PWM3_SLICE, false);
	// Disable I2C and UART
	i2c_deinit(IO_I2C_CH);
	uart_deinit(IO_UART_CH);
	// Clear UART buffer
	io_uart_buff=0;
	io_uart_var=0;
	// Resister SSPCR0 for SD
	sd_spi_sspcr[0]=g_io_spi_sspcr[0];
	sd_spi_sspcr[1]=g_io_spi_sspcr[1];
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
	unsigned int res,k;
	k=~gpio_get_all() & KEYSMASK;
	k=rotate_buttons_data(k);
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
	if (1000<=r1) {
		// Set clock divier for frequency
		pwm_set_clkdiv(slice,((float)(g_clock_hz/1000)) / ((float)r1));
		// 1000 cycles PWM
		pwm_set_wrap(slice, 1000);
		// Set duty
		pwm_set_chan_level(slice, channel, r2);
	} else {
		// Set clock divier for frequency
		pwm_set_clkdiv(slice,((float)(g_clock_hz/1000/65)) / ((float)r1));
		// 65000 cycles PWM
		pwm_set_wrap(slice, 65000);
		// Set duty
		pwm_set_chan_level(slice, channel, r2*65);
	}
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
		case 0:
		case 13:
		case IO_ADC0:
			// ADC0
			port=26;
			input=0;
			break;
		case 1:
		case 14:
		case IO_ADC1:
			// ADC1
			port=27;
			input=1;
			break;
		case 2:
		case 15:
		case IO_ADC2:
			// ADC2
			port=28;
			input=2;
			break;
		case 3:
		case 16:
		case IO_ADC3:
			// ADC3
			port=29;
			input=3;
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
	gpio_disable_pulls(port);
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
	g_default_args[2]=8;
	g_default_args[3]=0;
	g_default_args[4]=IO_SPI_CS;
	return argn_function(LIB_SPI,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		ARG_INTEGER_OPTIONAL<<ARG3 |
		ARG_INTEGER_OPTIONAL<<ARG4 |
		LIB_SPI_SPI<<LIBOPTION);
}

int spiwrite_statement(void){
	return io_call_lib_with_stack(LIB_SPI,LIB_SPI_SPIWRITE,1);
}

int spiwritedata_statement(void){
	return io_call_lib_with_stack(LIB_SPI,LIB_SPI_SPIWRITEDATA,2);
}

int spiread_function(void){
	return io_call_lib_with_stack(LIB_SPI,LIB_SPI_SPIREAD,0);
}

int spireaddata_statement(void){
	return io_call_lib_with_stack(LIB_SPI,LIB_SPI_SPIREADDATA,2);
}

int spiswapdata_statement(void){
	return io_call_lib_with_stack(LIB_SPI,LIB_SPI_SPISWAPDATA,2);
}

int spi_read32_blocking(spi_inst_t *spi, uint32_t repeated_tx_data, uint32_t *dst, size_t len){
	int i;
	uint16_t* dst16=(uint16_t*)dst;
	for(i=0;i<len;i++){
		if (!spi_read16_blocking (spi,repeated_tx_data>>16,&dst16[i*2+1],1)) break;
		if (!spi_read16_blocking (spi,repeated_tx_data&0xffff,&dst16[i*2],1)) break;
	}
	return i;
}

int spi_write32_blocking(spi_inst_t *spi, const uint32_t *src, size_t len){
	int i;
	const uint16_t* src16=(const uint16_t*)src;
	for(i=0;i<len;i++){
		if (!spi_write16_blocking (spi,&src16[i*2+1],1)) break;
		if (!spi_write16_blocking (spi,&src16[i*2],1)) break;
	}
	return i;
}

int spi_write32_read32_blocking(spi_inst_t *spi, const uint32_t *src, uint32_t *dst, size_t len){
	int i;
	const uint16_t* src16=(const uint16_t*)src;
	uint16_t* dst16=(uint16_t*)dst;
	for(i=0;i<len;i++){
		if (!spi_write16_read16_blocking (spi,&src16[i*2+1],&dst16[i*2+1],1)) break;
		if (!spi_write16_read16_blocking (spi,&src16[i*2],&dst16[i*2],1)) break;
	}
	return i;
}

void spi_send_option(int bit_num, unsigned int* sp, int sp_num){
	int i;
	switch(bit_num){
		case 8:
			for(i=0;i<sp_num;i++) spi_write_blocking(g_io_spi_ch,(const unsigned char*)&sp[i],1);
			break;
		case 16:
			for(i=0;i<sp_num;i++) spi_write16_blocking(g_io_spi_ch,(const unsigned short*)&sp[i],1);
			break;
		case 32:
		default:
			for(i=0;i<sp_num;i++) spi_write32_blocking(g_io_spi_ch,((const unsigned long*)&sp[i]),1);
			break;
	}
}

int lib_spi(int r0, int r1, int r2){
	static char cs_port=-1;
	static unsigned char bit_num;
	int i;
	unsigned int* sp=(unsigned int*)r1;
	if (RESET_STATIC_VARS==r2) {
		cs_port=-1;
		return 0;
	}
	// Set control registers for IO
	g_io_spi_sspcr[0]=io_spi_sspcr[0];
	g_io_spi_sspcr[1]=io_spi_sspcr[1];
	// Activate SPI connection
	asm volatile("nop \n nop \n nop");
	if (0<=cs_port) gpio_put(cs_port,0);
	asm volatile("nop \n nop \n nop");
	// r2 is the option number
	switch(r2){
		case LIB_SPI_SPI:
			// First, inactivate previous CS port, if set
		    if (0<=cs_port) gpio_put(cs_port,1);
			// Store bit number (either 8, 16, or 32)
			switch(sp[1]){
				case 8:
				case 16:
				case 32:
					bit_num=sp[1];
					break;
				default:
					stop_with_error(ERROR_INVALID);
					return r0;
			}
			// Set CS port
			if (r0<16) cs_port=gpio_table[r0];
			else cs_port=r0;
			gpio_init(cs_port);
			gpio_set_dir(cs_port, GPIO_OUT);
			gpio_put(cs_port,1);
			// Init SPI
			spi_init(g_io_spi_ch,sp[0]*1000);
		    gpio_set_function(g_io_spi_rx, GPIO_FUNC_SPI);
		    gpio_set_function(g_io_spi_tx, GPIO_FUNC_SPI);
		    gpio_set_function(g_io_spi_sck, GPIO_FUNC_SPI);
			gpio_set_pulls(g_io_spi_rx, true, false); // pull-up DO
			// Set format
			// Note: order must be SPI_MSB_FIRST, no other values supported on the PL022 
			switch(sp[2]){
				case 0: // CPOL=0, CPHA=0
					spi_set_format(g_io_spi_ch,sp[1],SPI_CPOL_0,SPI_CPHA_0,SPI_MSB_FIRST);
					break;
				case 1: // CPOL=0, CPHA=1
					spi_set_format(g_io_spi_ch,sp[1],SPI_CPOL_0,SPI_CPHA_1,SPI_MSB_FIRST);
					break;
				case 2: // CPOL=1, CPHA=1
					spi_set_format(g_io_spi_ch,sp[1],SPI_CPOL_1,SPI_CPHA_1,SPI_MSB_FIRST);
					break;
				case 3: // CPOL=1, CPHA=0
					spi_set_format(g_io_spi_ch,sp[1],SPI_CPOL_1,SPI_CPHA_0,SPI_MSB_FIRST);
					break;
				default:
					stop_with_error(ERROR_INVALID);
					return r0;
			}
			// Store control registers for IO
			io_spi_sspcr[0]=g_io_spi_sspcr[0];
			io_spi_sspcr[1]=g_io_spi_sspcr[1];
			break;
		case LIB_SPI_SPIREAD:
			spi_send_option(bit_num,sp,r0);
			switch(bit_num){
				case 8:
					spi_read_blocking(g_io_spi_ch,0xff,(unsigned char*)&g_scratch_char[0],1);
					r0=(unsigned char)g_scratch_char[0];
					break;
				case 16:
					spi_read16_blocking(g_io_spi_ch,0xffff,(unsigned short*)&g_scratch_short[0],1);
					r0=(unsigned short)g_scratch_short[0];
					break;
				case 32:
				default:
					spi_read32_blocking(g_io_spi_ch,0xffffffff,(unsigned long*)&g_scratch_int[0],1);
					r0=(unsigned int)g_scratch_int[0];
					break;
			}
			break;
		case LIB_SPI_SPIWRITE:
			spi_send_option(bit_num,sp,r0);
			break;
		case LIB_SPI_SPIREADDATA:
			spi_send_option(bit_num,&sp[2],r0-2);
			switch(bit_num){
				case 8:
					spi_read_blocking(g_io_spi_ch,0xff,(unsigned char*)sp[0],sp[1]);
					break;
				case 16:
					spi_read16_blocking(g_io_spi_ch,0xffff,(unsigned short*)sp[0],sp[1]);
					break;
				case 32:
				default:
					spi_read32_blocking(g_io_spi_ch,0xffffffff,(unsigned long*)sp[0],sp[1]);
					break;
			}
			break;
		case LIB_SPI_SPIWRITEDATA:
			spi_send_option(bit_num,&sp[2],r0-2);
			switch(bit_num){
				case 8:
					spi_write_blocking(g_io_spi_ch,(unsigned char*)sp[0],sp[1]);
					break;
				case 16:
					spi_write16_blocking(g_io_spi_ch,(unsigned short*)sp[0],sp[1]);
					break;
				case 32:
				default:
					spi_write32_blocking(g_io_spi_ch,(unsigned long*)sp[0],sp[1]);
					break;
			}
			break;
		case LIB_SPI_SPISWAPDATA:
			spi_send_option(bit_num,&sp[2],r0-2);
			switch(bit_num){
				case 8:
					spi_write_read_blocking(g_io_spi_ch,(unsigned char*)sp[0],(unsigned char*)sp[0],sp[1]);
					break;
				case 16:
					spi_write16_read16_blocking(g_io_spi_ch,(unsigned short*)sp[0],(unsigned short*)sp[0],sp[1]);
					break;
				case 32:
				default:
					spi_write32_read32_blocking(g_io_spi_ch,(unsigned long*)sp[0],(unsigned long*)sp[0],sp[1]);
					break;
			}
			break;
		default:
			// Invalid
			break;
	}
	// Inactivate SPI connection
	asm volatile("nop \n nop \n nop");
	if (0<=cs_port) gpio_put(cs_port,1);
	asm volatile("nop \n nop \n nop");
	// Set control registers for SD
	g_io_spi_sspcr[0]=sd_spi_sspcr[0];
	g_io_spi_sspcr[1]=sd_spi_sspcr[1];
	return r0;
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
			// Prepare values
			sp=(int*)r1;
			//i2cdat=(unsigned char*)&sp[3];// i2cdat (used for optional writing) starts at argument 4
			i2cdat2=(unsigned char*)sp[1];  // i2cdat2 (used for main writing/reading) is argument 2
			numdat2=sp[2];                  // size of i2cdat2 is argument 3
			num=r0-3;                       // exclude arguments 1, 2 and 3 for number of I2C data
			addr=(unsigned char)sp[0]&0x7f; // address is 7 bits data
			// Allocate temporary memory for data to write
			i2cdat=alloc_memory((num+numdat2+3)/4,-1);
			// Construct byte array
			for(i=0;i<num+numdat2;i++) {
				if (i<num) i2cdat[i]=(unsigned char)sp[i+3];
				else i2cdat[i]=i2cdat2[i-num];
			}
			break;
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
			// Write
			num=i2c_write_blocking(IO_I2C_CH,addr,i2cdat,num+numdat2,false);
			garbage_collection(i2cdat);
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
	g_default_args[2]=0;
	g_default_args[3]=0;
	return argn_function(LIB_SERIAL,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER_OPTIONAL<<ARG2 |
		ARG_INTEGER_OPTIONAL<<ARG3 |
		LIB_SERIAL_SERIAL<<LIBOPTION);
}

int serialin_function(void){
	g_default_args[1]=0;
	return argn_function(LIB_SERIAL,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		LIB_SERIAL_SERIALIN<<LIBOPTION);
}

int serialout_statement(void){
	return argn_function(LIB_SERIAL,
		ARG_INTEGER<<ARG1 |
		LIB_SERIAL_SERIALOUT<<LIBOPTION);
}

void on_uart_rx() {
	// The interrupt function
	if((uart_get_hw(IO_UART_CH)->rsr & UART_UARTRSR_PE_BITS)){
		// Parity error
		io_uart_buff[io_uart_buff_write_pos++]=0x100 | uart_getc(IO_UART_CH);
	} else {
		// No parity error
		io_uart_buff[io_uart_buff_write_pos++]=uart_getc(IO_UART_CH);
	}
	if (io_uart_buff_size<=io_uart_buff_write_pos) io_uart_buff_write_pos=0;
}

int lib_serial(int r0, int r1, int r2){
	int* sp=(int*)r1;
	switch(r2){
		case LIB_SERIAL_SERIAL:
			if (0==sp[0]) {
				// End using UART
				uart_deinit(IO_UART_CH);
				return r0;
			}
			// Prepare io_uart_buff[]
			if (r0) {
				io_uart_buff_size=r0;
			} else {
				io_uart_buff_size=sp[0]/60/8+1;
			}
			io_uart_buff_read_pos=io_uart_buff_write_pos=0;
			if (!io_uart_var) io_uart_var=get_permanent_block_number();
			io_uart_buff=(unsigned short*)alloc_memory((io_uart_buff_size+1)/2,io_uart_var);
			// Initialize UART
			uart_init(IO_UART_CH, 2400);
			gpio_set_function(IO_UART_TX, GPIO_FUNC_UART);
			gpio_set_function(IO_UART_RX, GPIO_FUNC_UART);
			uart_set_baudrate(IO_UART_CH, sp[0]);
			uart_set_hw_flow(IO_UART_CH, false, false);
			switch(sp[1]){
				case 0:
					uart_set_format(IO_UART_CH, 8, 1, UART_PARITY_NONE);
					break;
				case 1:
					uart_set_format(IO_UART_CH, 8, 1, UART_PARITY_EVEN);
					break;
				case 2:
					uart_set_format(IO_UART_CH, 8, 1, UART_PARITY_ODD);
					break;
				case 3:
				default:
					stop_with_error(ERROR_INVALID);
					return r0;
			}
			uart_set_fifo_enabled(IO_UART_CH, false);
			irq_set_exclusive_handler(IO_UART_IRQ, on_uart_rx);
			irq_set_enabled(IO_UART_IRQ, true);
			uart_set_irq_enables(IO_UART_CH, true, false);
			return r0;
		case LIB_SERIAL_SERIALIN:
			switch(r0){
				case 1:
					// Return number in buffer
					r0=io_uart_buff_write_pos-io_uart_buff_read_pos;
					if (r0<0) r0+=io_uart_buff_size;
					return r0;
				case 0:
				default:
					// Return -1 if not received
					if (io_uart_buff_read_pos==io_uart_buff_write_pos) return -1;
					// Return a received byte
					r0=io_uart_buff[io_uart_buff_read_pos++];
					if (io_uart_buff_size<=io_uart_buff_read_pos) io_uart_buff_read_pos=0;
					return r0;
			}
		case LIB_SERIAL_SERIALOUT:
			uart_putc_raw(IO_UART_CH,r0);
			return r0;
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
			if (r0<0 || (sizeof gpio_table)<=r0) return -1;
			i=gpio_table[r0];
			gpio_init(i);
			gpio_set_dir(i,GPIO_IN);
			gpio_pull_up(i);
			return gpio_get(i) ? 1:0;
		case LIB_GPIO_IN8H:
			gpio_init_mask(IO_GPIO_8H_MASK);
			gpio_set_dir_in_masked(IO_GPIO_8H_MASK);
			for(i=8;i<16;i++) gpio_pull_up(gpio_table[i]);
			r0=gpio_get_all() & IO_GPIO_8H_MASK;
			return io_gpio_inh_conversion(r0);
		case LIB_GPIO_IN8L:
			gpio_init_mask(IO_GPIO_8L_MASK);
			gpio_set_dir_in_masked(IO_GPIO_8L_MASK);
			for(i=0;i<8;i++) gpio_pull_up(gpio_table[i]);
			r0=gpio_get_all() & IO_GPIO_8L_MASK;
			return io_gpio_inl_conversion(r0);
		case LIB_GPIO_IN16:
			gpio_init_mask(IO_GPIO_16_MASK);
			gpio_set_dir_in_masked(IO_GPIO_16_MASK);
			for(i=0;i<16;i++) gpio_pull_up(gpio_table[i]);
			r0=gpio_get_all() & IO_GPIO_16_MASK;
			return io_gpio_in16_conversion(r0);
		case LIB_GPIO_OUT:
			if (r1<0 || (sizeof gpio_table)<=r1) return r0;
			i=gpio_table[r1];
			gpio_init(i);
			gpio_set_dir(i, GPIO_OUT);
			gpio_put(i, r0 ? 1:0);
			return r0;
		case LIB_GPIO_OUT8H:
			gpio_init_mask(IO_GPIO_8H_MASK);
			gpio_set_dir_out_masked(IO_GPIO_8H_MASK);
			gpio_put_masked(IO_GPIO_8H_MASK,io_gpio_outh_conversion(r0));
			return r0;
		case LIB_GPIO_OUT8L:
			gpio_init_mask(IO_GPIO_8L_MASK);
			gpio_set_dir_out_masked(IO_GPIO_8L_MASK);
			gpio_put_masked(IO_GPIO_8L_MASK,io_gpio_outl_conversion(r0));
			return r0;
		case LIB_GPIO_OUT16:
			gpio_init_mask(IO_GPIO_16_MASK);
			gpio_set_dir_out_masked(IO_GPIO_16_MASK);
			gpio_put_masked(IO_GPIO_16_MASK,io_gpio_out16_conversion(r0));
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
	if (instruction_is("I2CWRITEDATA")) return i2cwritedata_statement();
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
