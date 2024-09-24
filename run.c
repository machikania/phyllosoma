/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./core1.h"
#include "./api.h"

const int const g_r6_array[]={
	0,                         // Pointer to object
	(const int)&g_r6_array[0], // Pointer to previous argument array (recursive)
	0                          // Number of argument(s)
};

void run_code(void){
	// See also call_interrupt_function()
	// Push r0-r12
	asm("push {lr}");
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("mov r0,r8");
	asm("mov r1,r9");
	asm("mov r2,r10");
	asm("mov r3,r11");
	asm("mov r4,r12");
	asm("push {r0,r1,r2,r3,r4}");
	// Set special registers
	// R5 is pointer to array containing variable values
	asm("ldr r5,=kmbasic_variables");
	// R6 is pointer to argument array
	asm("ldr r6,=g_r6_array");
	// R7 is pointer to array containing various data
	asm("ldr r7,=kmbasic_data");
	// R8 is pointer to library function
	asm("ldr r0,=kmbasic_library");
	asm("mov r8,r0");
	// Store SP
	asm("mov r0,sp");
	asm("str r0,[r7,#0]");
	// Reserve 1016 bytes stack area for some library functions (fprint etc)
	asm("sub sp,#508");
	asm("sub sp,#508");
	// Store return address and call kmbasic_object
	asm("ldr r1,=kmbasic_object+1");
	asm("mov r0,pc");
	asm("add r0,#7");
	asm("str r0,[r7,#4]");
	asm("bx r1");
	// Pop r0-r12
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("pop {r0,r1,r2,r3,r4}");
	asm("mov r8,r0");
	asm("mov r9,r1");
	asm("mov r10,r2");
	asm("mov r11,r3");
	asm("mov r12,r4");
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("pop {pc}");
}

void call_interrupt_function(void* r0){
	// See also run_code()
	// Push registers
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("mov r1,r8");
	asm("mov r2,r9");
	asm("mov r3,r10");
	asm("mov r4,r11");
	asm("mov r5,r12");
	asm("mov r6,lr");
	asm("push {r1,r2,r3,r4,r5,r6}");
	// Push current interrupt flag status
	asm("ldr r1,=g_interrupt_code");
	asm("ldrb r2,[r1,#0]");
	asm("push {r2}");
	// Set g_interrupt_code flag
	asm("movs r2,#01");
	asm("strb r2,[r1,#0]");
	// Set special registers
	// R5 is pointer to array containing variable values
	asm("ldr r5,=kmbasic_variables");
	// R6 is pointer to argument array
	asm("ldr r6,=g_r6_array");
	// R7 is pointer to array containing various data
	asm("ldr r7,=kmbasic_data");
	// R8 is pointer to library function
	asm("ldr r1,=kmbasic_library");
	asm("mov r8,r1");
	// Call the code
	asm("blx r0");
	// Pop interrupt flag status
	asm("pop {r2}");
	asm("ldr r1,=g_interrupt_code");
	asm("strb r2,[r1,#0]");
	// Pop registers
	asm("pop {r1,r2,r3,r4,r5,r6}");
	asm("mov r8,r1");
	asm("mov r9,r2");
	asm("mov r10,r3");
	asm("mov r11,r4");
	asm("mov r12,r5");
	asm("mov lr,r6");
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7}");
}

void pre_run(void){
	// Initializing environment
	init_memory();
	// Initialize READ/DATA
	g_read_point=&kmbasic_object[0];
	g_read_mode=0;
	g_read_valid_len=0;
	// Random seed
	g_rnd_seed=0x92D68CA2; //2463534242
	// Inilialize kmbasic_data[] (see also run_code() )
	kmbasic_data[2]=(int)&kmbasic_var_size[0];
	// Close all files
	close_all_files();
	// Init I/O
	io_init();
	// Init music
	init_music();
	// Lower interrupt flag
	g_interrupt_code=0;
	// Reset static variables
	lib_display(0,0,RESET_STATIC_VARS);
	lib_spi(0,0,RESET_STATIC_VARS);
	// Exception handling
	handle_exception(1);
	// Wifi
	pre_run_wifi();
}

void post_run(void){
	// Reset memory allocation
	reset_memory();
	// Close all files
	close_all_files();
	// Reset I/O and activate buttons
	io_init();
	lib_keys(63,0,0);
	// Init timer (to stop interrupt etc)
	timer_init();
	// Stop music
	stop_music();
	// Stop PCG
	stopPCG();
	// Stop core1 when not using USB keyboard
	if (!g_active_usb_keyboard) stop_core1();
	// Lower interrupt flag and cancel all interrupts
	g_interrupt_code=0;
	cancel_all_interrupts();
	// Restore exception handling
	handle_exception(0);
	// Wifi
	post_run_wifi();
}

/*
	Wrapper of snprintf() for using lib stack (1016 bytes)
	Note that the lib stack region is used for executing snprintf().
	When using lib stack, the interrupt is disabled because of the stricted length of stack area.
*/

int machikania_snprintf_main(char *buffer, size_t n, const char *format_string, float float_value) {
	return snprintf(buffer,n,format_string,float_value);
}

int machikania_snprintf(char *buffer, int n, const char *format_string, float float_value) {
	asm("push {r6,r7,lr}");
	asm("mov r6,sp");
	asm("ldr r7,=kmbasic_data");
	asm("ldr r7,[r7,#0]");
	asm("cpsid i");
	asm("mov sp,r7");
	asm("bl machikania_snprintf_main");
	asm("mov sp,r6");
	asm("cpsie i");
	asm("pop {r6,r7,pc}");
}
