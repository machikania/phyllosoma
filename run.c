/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

const int const g_r6_array[]={
	0,                         // Pointer to object
	(const int)&g_r6_array[0], // Pointer to previous argument array (recursive)
	0                          // Number of argument(s)
};

void run_code(void){
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
	asm("add r0,#5");
	asm("str r0,[r7,#4]");
	asm("bx r1");
	// Pop r0-r12
	asm("pop {r0,r1,r2,r3,r4}");
	asm("mov r8,r0");
	asm("mov r9,r1");
	asm("mov r10,r2");
	asm("mov r11,r3");
	asm("mov r12,r4");
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7}");
	asm("pop {pc}");
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
}

void post_run(void){

}
