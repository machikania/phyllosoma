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
void init_socket_system(void);
void init_tcp_socket(void);
void init_tls_socket(void);
void sent_bytes(int bytes);
char add_pcb_to_fifo(void* tcp_pcb);
void* shift_pcb_fifo(void);
char* tcp_receive_in_buff(char* data, int bytes, void* tcp_pcb);
int tcp_read_from_buffer(char* dest, int bytes, void** connection_id);
err_t send_header_if_exists(void);
err_t machikania_tcp_write(const void* arg, u16_t len, void** connection_id);
int machikania_tcp_status(int mode, void** connection_id);
err_t machikania_tcp_close(void** connection_id);
void register_state(void* state);
void register_tcp_pcb(void* pcb);
void register_closing_function(void* func);
void set_connection_flag(int flag);
void connection_error(void);
void* new_connection_id(void* tcp_pcb);

// picow_tcp_client.c
void start_tcp_client(const char* ipaddr, int tcp_port);

// picow_tcp_server.c
void start_tcp_server(int tcp_port, int tcp_accept_mode);
err_t tcp_server_client_close(void* arg);

// picow_tls_server.c
void start_tls_client(const char* servername, int tcp_port);

// See lwip/err.h for error values, like ERR_OK=0, ERR_INPROGRESS=-5, etc

// Definitions

#define WIFI_ERROR_NO_ERROR 0
#define WIFI_ERROR_DNS_ERROR 1
#define WIFI_ERROR_CONNECTION_ERROR 2
#define WIFI_ERROR_WIFI_ERROR 3

// Macros

#define MACHIKANIA_DEFAULT_WIFI_SSID "MACHIKANIA_DEFAULT_WIFI_SSID\0\0\0"
#define MACHIKANIA_DEFAULT_WIFI_PASSWD "MACHIKANIA_DEFAULT_WIFI_PASSWD\0"

#define MACHIKANIA_DEFAULT_WIFI_SSID64 MACHIKANIA_DEFAULT_WIFI_SSID "\0" MACHIKANIA_DEFAULT_WIFI_SSID
#define MACHIKANIA_DEFAULT_WIFI_SSID128 MACHIKANIA_DEFAULT_WIFI_SSID64 "\0" MACHIKANIA_DEFAULT_WIFI_SSID64
#define MACHIKANIA_DEFAULT_WIFI_PASSWD64 MACHIKANIA_DEFAULT_WIFI_PASSWD "\0" MACHIKANIA_DEFAULT_WIFI_PASSWD
#define MACHIKANIA_DEFAULT_WIFI_PASSWD128 MACHIKANIA_DEFAULT_WIFI_PASSWD64 "\0" MACHIKANIA_DEFAULT_WIFI_PASSWD64
