#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <semaphore.h>
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <pthread.h>
#include <sys/time.h>
#include "global_data.h"


extern struct video_data v_data;
extern struct udp_flag u_flag;
extern sem_t v_get,v_send;

void* video_broadcast_thread(void)
{ 
	struct sockaddr_in client_addr;
//	char msg[BUFFER_SIZE];
	int ss,ret,send_len,opt=1;
	int j;
	char end[3]={"end"};
	char start[5]={"start"};
    /* create the socket commanted by greenstar*/  
	if ((ss = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {  
	perror("Create UDPclient failed");  
	exit(0);  
	}

	memset(&client_addr,0,sizeof(client_addr));  
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
//	client_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
//	client_addr.sin_addr.s_addr = inet_addr(AIMED_IP);
	client_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

	socklen_t addr_len=sizeof(client_addr);

	ret = setsockopt(ss, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));  
	if(ret == -1)
		{  
		printf("set udp socket error...\n");  
		exit(0);  
		}

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
while(1)
{
	pthread_testcancel();	
	sem_wait(&v_send);
	sendto(ss,start,5,0,(struct sockaddr*)&client_addr,addr_len);
//	printf("send start\n"); 
	for(j=0;j<v_data.length/1024+1;j++)
		{
		if(j==v_data.length/1024){
        		if((v_data.length-j*1024)!=0)
				send_len=sendto(ss, v_data.start_data+j*1024,
	v_data.length%1024,0,(struct sockaddr*)&client_addr,addr_len); 
			else
				send_len=sendto(ss, v_data.start_data+j*1024,
		1024,0,(struct sockaddr*)&client_addr,addr_len); }
		else
				send_len= sendto(ss, v_data.start_data+j*1024,
		1024,0,(struct sockaddr*)&client_addr,addr_len); 
		if(send_len<0)printf("send video data error");
		}
	sendto(ss,end,3,0,(struct sockaddr*)&client_addr,addr_len);
	sem_post(&v_get);
//	printf("send over\n");
	
}
	close(ss);
	printf("Send finish\n");  
	return 0;  
}
//////////////////////////////////////////////
