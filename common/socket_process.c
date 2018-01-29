#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "global_data.h"

#define MYPORT  50000
#define QUEUE   4
#define BUFFER_SIZE 1024

extern struct socket_flag socket_fg; 
extern void read_cmd(char* cmd);
extern sem_t server_sock;
extern void* video_send_thread(void);
pthread_t vsend_thread,vget_thread,info_thread;
int err;
////////////////////////////////////////////
void handle_request(int conn)
{
 err = pthread_create(&vget_thread, NULL, (void*)video_get_thread, &conn);
        if (err != 0) {
                fprintf(stderr, "can't create video get thread: %s\n",
                strerror(err));
		exit(1);
		}
 err = pthread_create(&vsend_thread, NULL, (void*)video_send_thread, &conn);
        if (err != 0) {
                fprintf(stderr, "can't create video send thread: %s\n",
                strerror(err));
		exit(1);
		}

 err = pthread_create(&info_thread, NULL, (void*)info_conm_thread, &conn);
        if (err != 0) {
                fprintf(stderr, "can't create info conmunication thread: %s\n",
                strerror(err));
		exit(1);
		}


}
////////////////////////////////////////////
void socket_process(void)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

	socket_fg.data_trans_status=0;
	socket_fg.socket_con_status=0;
	///定义sockfd
	int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
	///定义sockaddr_in
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(MYPORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	int conn;
	socket_fg.socket_con_status=1;
while(1)
	{
	conn=accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
	        if(conn<0)
                	{
                	perror("connect");
			exit(1);
               		 }
		else
			{
			if(fork()>0)
			close(conn);
			else
			handle_request(conn);
                printf("connection successful \n");
			}


	}
}
////////////////////////////////////////////
