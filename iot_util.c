#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <ctype.h>
#include "iot_util.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"
#include "tcp_con.h"




static char Connect_Req[1]={0x98};
pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;




int  iotat_thread_create( iotat_thread_t  *pthread, iotat_thread_func_t  start, void*  arg )
{
    pthread_attr_t   attr;

    pthread_attr_init (&attr);
    return pthread_create( pthread, &attr, start, arg );
}


const char* strstri(const char* str, const char* subStr){
	int len = strlen(subStr);
	if(len == 0){
		return NULL;
	}
    while(*str){
		if(strncasecmp(str, subStr, len) == 0){
			return str;
		}
        ++str;
	}
	return NULL;
}


int iotat_socket_connected(int sock){
	if(sock <= 0)
		return 0;
	struct tcp_info info;
	int len=sizeof(info);
	getsockopt(sock,IPPROTO_TCP, TCP_INFO,&info,(socklen_t *)&len);
	if((info.tcpi_state==TCP_ESTABLISHED)){
		//printf("socket still connected\n");
		return 1;
	}else{
		printf("socket disconnected.\n");
		return 0;
	}
}
int iotat_socket_create(char *addr,int port,int *socket_fd){
	IOT_LOG("iotat_socket_create addr=%s ,port=%d \n",addr,port);
	//socket()
	int sockfd;
	char buf[SOCKET_BUF_SIZE];
	struct sockaddr_in servaddr;
	IOT_LOG("socket start \n");
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0) {//AF_INET:IPv4 Internet protocols
		IOT_LOG("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		return errno;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if(inet_pton(AF_INET,addr,&servaddr.sin_addr)<=0){//is addr valid?
		IOT_LOG("inet_pton error for %s",addr);
		return -1;
	}
    //connect()
    IOT_LOG("connect start \n");
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
		IOT_LOG("connect error: %s(errno: %d)\n",strerror(errno),errno);
		return errno;
	}
	IOT_LOG("connect OK!sockfd=%d \n",sockfd);
	*socket_fd = sockfd;
	return 0;//no error,create successfull
}

//socket send
int iotat_socket_send (int fd,char *buf,int len){
	int w_cnt = 0;
	int offset = 0;
	//char backup_data_buf[DATA_EXT_DIAG_SIZE];
	int backup_data_len = len;
	if(fd > 0){
		do{
			w_cnt = send(fd,buf+offset,backup_data_len,0);
			if( w_cnt <0){
				IOT_LOG("send data error:%s(errno: %d)",strerror(errno),errno);
				continue; //continue or break return errno??
			}else{
				backup_data_len -= w_cnt;
				offset += w_cnt;
			}
			IOT_LOG("iotat_socket_send,w_cnt:%d,offset:%d,backup_data_len:%d\n",w_cnt,offset,backup_data_len);
		}while(backup_data_len > 0);
	}
	return 0;
}

int iotat_socket_close(int fd){
		IOT_LOG("close socket fd:%d",fd);
		return close(fd);
}

int is_ipaddr_invalid(char *str){//for IPV4
	if(str == NULL || *str == '\0')
		return 1;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	if(inet_pton(AF_INET,str,&servaddr.sin_addr)==1)
		return 0;
	return 1;
}

void *iotat_socket_receive_thread(void *x){
	int r_cnt,socket_fd,chanelNum;
	char rsp[512];
	char read_socket_data[SOCKET_BUF_SIZE];
	char IMEI[100] = "867400020316613";
	iotat_data_chn_t* iotat_socket_chnl_receive = (iotat_data_chn_t*)x;
	socket_fd = iotat_socket_chnl_receive->socket_hdl;
	chanelNum = iotat_socket_chnl_receive->num;
	IOT_LOG("Channel %d socket receive thread create!\n",chanelNum);

       iotat_socket_send(socket_fd,IMEI,strlen(IMEI));
	//set recv timeout
	struct timeval timeout;
	int cir_count;
	timeout.tv_sec = TIMEOUT_TIME;
	timeout.tv_usec = 0;
	setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(struct timeval));
	while(1){
		memset(read_socket_data,0,sizeof(read_socket_data));
		memset(rsp, 0, sizeof(rsp));
		
		if(iotat_socket_connected(socket_fd)== 0){ //when the socket_fd is closed,exit this thread
			IOT_LOG("This socket is already closed!exit socket receive thread!\n");
			break;//exit this thread
		}
		r_cnt = recv(socket_fd,read_socket_data,SOCKET_BUF_SIZE,MSG_WAITALL);
		if(r_cnt > 0){
			IOT_LOG("Read success!read_socket_data is:%s \n",read_socket_data);
			if(read_socket_data[0]==0x99)
				{
				 iotat_socket_send(socket_fd,Connect_Req,sizeof(Connect_Req));
				 IOT_LOG("reply topway ");
				 }
			if(NULL != strstri(read_socket_data,ALIVE_REQ))
				{
				 iotat_socket_send(socket_fd,ALIVE_RSP,sizeof(ALIVE_RSP));
				 IOT_LOG("reply topway ");
				 }
		}else{
			if((r_cnt < 0)&&(errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR)){
				IOT_LOG("recv timeout!continue receive\n");
				continue;
			}
			IOT_LOG("read socket error:%s(errno:%d)\n",strerror(errno),errno);
			
			break;
		}
	}
	iotat_socket_close(socket_fd);
	IOT_LOG("socket %d exit socket_receive_thread!Close channel %d\n",socket_fd,chanelNum);
	return NULL;
}


