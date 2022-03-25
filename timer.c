/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "./compiler.h"

int coretimer_function(){
	return call_lib_code(LIB_CORETIMERFUNC);
}

int lib_coretimerfunc(int r0, int r1, int r2){
	return time_us_32();
}
