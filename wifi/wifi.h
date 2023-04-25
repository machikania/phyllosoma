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

// See lwip/err.h for error values, like ERR_OK=0, ERR_INPROGRESS=-5, etc

