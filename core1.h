/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#define CALLBACK_BUFFER_NUM 8

/*
	void start_core1(void);
	
	Start core1 loop.
	
*/
void start_core1(void);

/*
	void stop_core1(void);
	
	Stop core1 loop.
	
*/
void stop_core1(void);

/*
	void request_core1_callback(void* func);
	
	Must use this function by core0.
	Request a call back to "func" by core1.

*/
void request_core1_callback(void* func);

/*
	void request_core1_callback_at(void* func, unsigned int at);
	
	Must use this function by core1.
	Request a call back to "func" by core1 at the specified time.
*/
void request_core1_callback_at(void* func, unsigned int at);

/*
	void wait_core1_busy(void);
	
	Must use this function by core0.
	Wailt whle the core 1 is busy.
	Following operation must be as quick as possible.

*/
void wait_core1_busy(void);
