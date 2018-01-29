#include  <unistd.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "global_data.h"
#include <semaphore.h>
#include "include.h"
#include <semaphore.h>

int signal_init(); 

struct socket_flag socket_fg;
struct move_cmd m_cmd;
struct move_info m_info;
struct video_data v_data;
struct udp_flag u_flag;

sem_t v_get,v_send,server_sock;

pid_t monitor_pro_pid,socket_pro_pid,cmd_pro_pid,err;
//////////////////////////////////////////////
int  main(int argc, char **argv)
{
	signal_init();
	create_all_process();
	if(signal(SIGKILL,signal_proceed)==SIG_ERR)
		perror("signal error");
	while(1);
	return 0;
}
///////////////////////////////////////////
int signal_init()
{
	if(sem_init(&v_get,0,1)>0)
	printf("v_get init error");
	if(sem_init(&v_send,0,0)>0)
	printf("v_send init error");
	if(sem_init(&server_sock,0,0)>0)
	printf("server_sock init error");
	return 0;

}
///////////////////////////////////////////
void signal_proceed(int signo)
{
if(signo==SIGKILL)
	cancel_all_process();
exit(1);
}
///////////////////////////////////////////
int cancel_all_process()
{
	int status;
	if(kill(monitor_pro_pid,SIGKILL)<0)
		printf("cancel monitor process failed");
	else{
		err=waitpid(monitor_pro_pid,&status,WUNTRACED|WCONTINUED);
		if(err>0)
		printf("cancel monitor process:%d successfully\n",err);
		else
		printf("cancel monitor process failed\n");
		}

	if(kill(cmd_pro_pid,SIGKILL)<0)
		printf("cancel cmd process failed");
	else{
		err=waitpid(cmd_pro_pid,&status,WUNTRACED|WCONTINUED);
		if(err>0)
		printf("cancel cmd process:%d successfully\n",err);
		else
		printf("cancel cmd process failed\n");
		}

	if(kill(socket_pro_pid,SIGKILL)<0)
		printf("cancel socket process failed");
	else{
		err=waitpid(socket_pro_pid,&status,WUNTRACED|WCONTINUED);
		if(err>0)
		printf("cancel socket process:%d successfully\n",err);
		else
		printf("cancel socket process failed\n");
		}
	return 0;
}
/////////////////////////////////////////////
int create_all_process()
{
	cmd_pro_pid=fork();
	switch(cmd_pro_pid)
	{
	case -1:printf("cmd process create error!\n");break;
	case 0:cmd_process();break;
	default:printf("cmd process pid is:\n",cmd_pro_pid);break;
	}
	monitor_pro_pid=fork();
	switch(monitor_pro_pid)
	{
	case -1:printf("monitor process create error!\n");break;
	case 0:monitor_process();break;
	default:printf("monitor process pid is:\n",monitor_pro_pid);break;
	}
	socket_pro_pid=fork();
	switch(socket_pro_pid)
	{
	case -1:printf("socket process create error!\n");break;
	case 0:socket_process();break;
	default:printf("socket process pid is:\n",socket_pro_pid);break;
	}
	return 0;
}
