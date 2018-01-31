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

void* video_send_thread(int conn)
{
//	char msg[BUFFER_SIZE];
	int send_len;
	int j;
	char end[3]={"end"};
	char start[5]={"start"};
	v_data.send_status=0;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
while(1)
{
	pthread_testcancel();	
	while(v_data.send_status==0)
	pthread_testcancel();
	sem_wait(&v_send);
	yuyv_to_jpeg(v_data.start_data,80,320,240);	
	write(conn,start,5);

//	printf("send start"); 
/*	for(j=0;j<v_data.length/1024+1;j++)
		{
		if(j==v_data.length/1024){
        		if((v_data.length-j*1024)!=0)
				send_len=write(conn, v_data.start_data+j*1024,
	v_data.length%1024); 
			else	
				send_len=write(conn, v_data.start_data+j*1024,
	v_data.length%1024); 
		else	
				send_len=write(conn, v_data.start_data+j*1024,
	1024); 
		if(send_len<0)printf("send video data error");
		}*/


	
	write(conn,end,3);
	sem_post(&v_get);
//	printf("send over");
}
	close(conn);
	printf("Send finish\n");  
	return 0;  
}
