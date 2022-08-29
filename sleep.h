/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

extern char g_interrupt_code;

#define sleep_us(n) do {\
		if (g_interrupt_code) busy_wait_us(n);\
		else sleep_us(n);\
	} while(0)

#define sleep_ms(n) do {\
		if (g_interrupt_code) busy_wait_ms(n);\
		else sleep_ms(n);\
	} while(0)

