#define printf wifi_set_error(__LINE__); wifi_set_error_str
void wifi_set_error_str(char* err_str,...);
char* wifi_error_str(void);
void wifi_set_error(int err);
int wifi_error(void);

// rtc.c
void set_time_from_utc(time_t t);

time_t* get_ntp_time(char* ntp_server);

// See lwip/err.h for error values, like ERR_OK=0, ERR_INPROGRESS=-5, etc

// NTP test follows
//run_ntp_test();

// TCP client test follows
//run_tcp_client_test(ip4addr_ntoa(&server_address));

// TCP server test follows
//run_tcp_server_test();

// HTTPS client test follows
//run_tls_client_test();
