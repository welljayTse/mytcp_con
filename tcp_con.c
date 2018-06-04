#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include "cutils/sockets.h"
#include "cutils/properties.h"
#include <private/android_filesystem_config.h>
#include "tcp_con.h"
#include "iot_util.h"




iotat_data_chn_t *iotat_active_chnl_p = NULL;
iotat_data_chn_t  iotat_active_chnl; 
int socket_fd = 0;
int main (int argc, char** argv)
{
    
    char ser_addr_ip[IP_ADDRESS_LEN] = "101.231.56.153";					//service ip address support ipv4\ipv6
    char ser_port[PORT_NUM_LEN] = "9088";						  //service port number support ipv4\ipv6
    iotat_active_chnl_p = &iotat_active_chnl;
    char get_prop[PROPERTY_VALUE_MAX]={0};
    property_get(TCP_SERVER_ADDR,get_prop,"not_find");
    if(0 != strcmp(get_prop, "not_find"))
    	{
    	IOT_LOG("TCP_SERVER_ADDR: %s\n",get_prop);
    	strcpy(ser_addr_ip,get_prop);
    	}
    memset(get_prop,0,PROPERTY_VALUE_MAX);	
    property_get(TCP_SERVER_PORT,get_prop,"not_find");
    if(0 != strcmp(get_prop, "not_find"))
    	{
    	IOT_LOG("TCP_SERVER_ADDR: %s\n",get_prop);
    	strcpy(ser_port,get_prop);
    	}
    /*Call socket create function*/
  
    IOT_LOG("addr is %s ,port is %s \n",ser_addr_ip,ser_port);
    int port = atoi(ser_port);
    int chl_errno = iotat_socket_create(ser_addr_ip,port,&socket_fd);
    strcpy(iotat_active_chnl_p->ser_addr_ip,ser_addr_ip);
    strcpy(iotat_active_chnl_p->ser_port,ser_port);
	if( socket_fd >0){  /*socket create success */
		iotat_thread_t t;
		IOT_LOG("create socket receive thread \n");
		iotat_active_chnl_p->socket_hdl = socket_fd;
		if (0 != iotat_thread_create( &t, iotat_socket_receive_thread,&iotat_active_chnl)){
			IOT_LOG("socket_write thread start error");
		}
	}else{ /* socket create failed */
		IOT_LOG("create socket receive thread fail\n");
	}
	
    while(1){
        sleep(10000);
    }
	if( socket_fd >0)
	iotat_socket_close(socket_fd);

    return 0;
}

