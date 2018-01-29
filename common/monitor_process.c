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
#include <signal.h>

extern void *move_thread(void);
extern void *video_get_thread(void);


int err;
pthread_t m_thread,vget_thread;
///////////////////////////////////////////
///////////////////////////////////////////
void signal_proceed(int signo)
{
if(signo==SIGKILL)
	cancel_all_process();
}
////////////////////////////////////////////
int create_all_thread()
{
	if(pthread_cancel(m_thread)<0)
		printf("cancel move thread failed");
	else{
		pthread_join(m_thread,NULL);
		printf("cancel move thread successfully\n");
		}
	if(pthread_cancel(vget_thread)<0)
		printf("cancel video get thread failed");
	else{
		pthread_join(vget_thread,NULL);
		printf("cancel video get thread successfully\n");
		}
}
////////////////////////////////
int create_all_thread()
{
	 err = pthread_create(&vget_thread, NULL, (void*)video_get_thread, &conn);
        if (err != 0) {
                fprintf(stderr, "can't create video get thread: %s\n",
                strerror(err));
                exit(1);
                }
        err = pthread_create(&m_thread, NULL, (void*)move_thread, NULL);
        if (err != 0) {
                fprintf(stderr, "can't create move thread: %s\n",
                strerror(err));
		exit(1);
			}
}

//////////////////////////////////////
void monitor_process()
{
	create_all_thread();
	if(signal(SIGKILL,signal_proceed)==SIG_ERR)
		perror("signal error");
}
