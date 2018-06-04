#ifndef __TCP_CON_H__
#define __TCP_CON_H__

ghjkl;'
sdf
#define IOT_DEV_PATH_LEN        260
#define MAX_OPEN_TIMES  100
#define DATA_BUF_SIZE (4096*4)
#define DATA_EXT_BUF_SIZE (4096*4)
#define IP_ADDRESS_LEN  64
#define PORT_NUM_LEN     8

typedef struct iotat_host_int_t {
    char dev_uart[IOT_DEV_PATH_LEN];
}iotat_host_int_t;

typedef struct iotat_modem_int {
    char sipc_chan[IOT_DEV_PATH_LEN];
}iotat_modem_int_t;
typedef struct iotat_dev_info {
    iotat_host_int_t host_int;
    iotat_modem_int_t modem_int;
}iotat_dev_info_t;

typedef enum {
	IOTAT_UART_AT = 0,
	IOTAT_UART_RAW_DATA
}IOTAT_FLAG;

extern iotat_dev_info_t *g_dev_info;
extern int g_pc_fd;
extern int g_uart_speed;
extern int g_iotat_flag_uart_mode;
extern char uart_buf[DATA_BUF_SIZE];

typedef struct iotat_data_chn {
	int num;										 //channel num:0~15
	char ser_addr_ip[IP_ADDRESS_LEN];					//service ip address support ipv4\ipv6
	char ser_port[PORT_NUM_LEN];						  //service port number support ipv4\ipv6
	int socket_hdl; 							 //channel socket handle
	int state;										 //channel state:0 channel un-create;1 channel create success;3 channel in data transfer
	int send_len;								 //Single raw data send length
	int recv_len;								  //Single raw data recv length
}iotat_data_chn_t;	


#define TCP_SERVER_ADDR   "persist.tcp_con.addr"
#define TCP_SERVER_PORT    "persist.tcp_con.port"

#endif
