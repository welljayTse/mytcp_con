#ifndef _IOT_UTIL_H_
#define _IOT_UTIL_H_

#include <stdio.h>
#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG 	"TCPCON"
#include <utils/Log.h>
#define IOT_AT_LOG  ALOGD

#define IOT_TRACING

#ifdef IOT_TRACING
#define IOT_LOG  ALOGD
#else
    //#define IOT_LOG  ALOGD
#define  IOT_LOG(format, ...)
#endif

#define IOT_AT_CHANNEL

	typedef pthread_t iotat_thread_t;
	typedef void* (*iotat_thread_func_t)( void*  arg);
	int  iotat_thread_create( iotat_thread_t  *pthread, iotat_thread_func_t  start, void*  arg );
	int iot_open_dev(char* dev, int mode);
	int set_raw_data_speed(int fd, int speed);
	int iotat_socket_connected(int sock);
	int iotat_socket_create(char *addr,int port,int *socket_fd);
	void *iotat_socket_receive_thread(void *x);
	int iotat_socket_send (int fd,char *buf,int len);
	int iotat_socket_close(int fd);
	int iotat_uart_send(char *rsp);
	void *iotat_socket_receive_thread(void *x);
	static char AT_EOL[]={0x0d,0x0a,0x0};
static char ALIVE_REQ[]="keepalive";
static char ALIVE_RSP[]="Alive";
#define SOCKET_BUF_SIZE (4096*4)
#define TIMEOUT_TIME 10

#ifdef __cplusplus
}
#endif

#endif /*!_IOT_UTIL_H_*/
