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
#include <signal.h>
#include "global_data.h"

#define MYPORT  50000
#define QUEUE   4
#define BUFFER_SIZE 1024

extern struct socket_flag socket_fg; 
extern void read_cmd(char* cmd);
extern sem_t v_get,v_send;
extern void* video_send_thread(void);
extern void* info_conm_thread(void);

pthread_t vsend_thread,vget_thread,info_thread;
pid_t socket_fork[QUEUE];
int err;
static int socket_num=0;
////////////////////////////////////////////
void handle_request(int conn)
{
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

///////////////////////////////////////////
void cancel_socket_pro_thread()
{

	if(pthread_cancel(info_thread)<0)
		printf("cancel info thread failed");
	else{
		pthread_join(info_thread,NULL);
		printf("cancel info thread successfully\n");
		}
	if(pthread_cancel(vsend_thread)<0)
		printf("cancel video send thread failed");
	else{
		pthread_join(vsend_thread,NULL);
		printf("cancel video send thread successfully\n");
		}
}
///////////////////////////////////////////
void signal_proceed(int signo)
{
if(signo==SIGKILL)
	cancel_socket_pro_thread();
kill(getpid(),SIGKILL);
exit(1);
}

///////////////////////////////////////////
void signal_socket_proceed(int signo)
{
int i;
if(signo==SIGKILL)
	for(i=0;i<socket_num;i++)
	kill(socket_fork[i],SIGKILL);
	
exit(1);
}
////////////////////////////////////////////
void socket_process(void)
{
	if(signal(SIGKILL,signal_socket_proceed)==SIG_ERR)
		perror("signal error");
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
			if(socket_fork[socket_num]=fork()>0)
				if( socket_num<QUEUE)socket_num++;
				else  socket_num=0;	
			close(conn);
			else
			handle_request(conn);
                printf("connection successful \n");
			}


	}
}
////////////////////////////////////////////
