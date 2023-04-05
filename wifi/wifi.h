#define WIFI_BUFF_SIZE 2048

#define printf wifi_set_error(__LINE__); wifi_set_error_str
#define DEBUG_printf wifi_set_error(__LINE__); wifi_set_error_str

// wifierror.c
void wifi_set_error_str(char* err_str,...);
char* wifi_error_str(void);
void wifi_set_error(int err);
int wifi_error(void);

// rtc.c
void set_time_from_utc(time_t t);
time_t* get_ntp_time(char* ntp_server);

// socket.c
void init_tcp_socket(void);
char* tcp_receive_in_buff(char* data, int bytes);
int tcp_read_from_buffer(char* dest, int bytes);
err_t machikania_tcp_write(const void* arg, u16_t len);
int machikania_tcp_status(int mode);
err_t machikania_tcp_close(void);
void set_connection_flag(int flag);
void register_state(void* state);
void register_tcp_pcb(void* pcb);
void register_closing_function(void* func);

// picow_tcp_client.c
void start_tcp_client(const char* ipaddr, int tcp_port);

// See lwip/err.h for error values, like ERR_OK=0, ERR_INPROGRESS=-5, etc

// NTP test follows
//run_ntp_test();

// TCP client test follows
//run_tcp_client_test(ip4addr_ntoa(&server_address));

// TCP server test follows
//run_tcp_server_test();

// HTTPS client test follows
//run_tls_client_test();
