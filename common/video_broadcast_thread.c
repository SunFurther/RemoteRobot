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

extern int read_file_to_buff(char* filename,unsigned char* buff);
extern int yuyv_to_jpeg(unsigned char* yuv422,int quality,
int width, int height);
////////////////////////////////////////////
int get_len(unsigned char* buff,int len)
{
int num=0;
int i;
int jump=0;
for(i=0;i<len;i++)
{
	if(*(buff+i)!=0)
	num++;
	else{
		if((i<len-2)&&(*(buff+i+1)==0)&&(*(buff+i+2)==0))
		jump=1;
		else num++;
		}
	if(jump==1)break;
}
	return num;
}
//////////////////////////////////////////
void* video_broadcast_thread(void)
{
	struct sockaddr_in client_addr;
//	char msg[BUFFER_SIZE];
	int ss,ret,send_len,opt=1;
	int j,file_len;
	unsigned char video_buff[50*1024];
//	memset(video_buff,0,sizeof(video_buff));  

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
		printf("set udp broadcast error...\n");  
		exit(0);  
		}


//	printf("video bro break2");
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
while(1)
{

//	printf("break 1");	
	
	pthread_testcancel();	
	sem_wait(&v_send);

//	printf("break 2");	
	
	yuyv_to_jpeg(v_data.start_data,QUALITY,VIDEO_WIDTH,VIDEO_HEIGHT);	

	file_len=read_file_to_buff(FILENAME,video_buff);
	
//	printf("break 3");	
	
	printf("the video pic is %d bytes\n",file_len);

//	printf("the video_buff is %d bytes\n",get_len(video_buff,50*1024));


//	printf("the video pic is %d bytes\n",file_len%1024);

//	printf("send start\n");

	sendto(ss,start,5,0,(struct sockaddr*)&client_addr,addr_len);

	for(j=0;j<file_len/1024+1;j++)
		{
		if(j==file_len/1024){
        		if((file_len%1024)!=0)
				send_len=sendto(ss, video_buff+j*1024,
	file_len%1024,0,(struct sockaddr*)&client_addr,addr_len); 
			else
				send_len=sendto(ss, video_buff+j*1024,
	1024,0,(struct sockaddr*)&client_addr,addr_len); 
			}
		else
				send_len= sendto(ss, video_buff+j*1024,
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
