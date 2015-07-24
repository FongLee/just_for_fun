#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
 #include <sys/select.h>

#include "commsocket.h"
#include "error.h"

#define PORT 		1234

void my_getopt(int argc, char **argv);
void child_process(int newfd);
void pr_cpu_time(void);
int register_sig_handler();
void my_handler(int num);
void sig_int_handler(int num);
void usage(char *argv);
void * thread_main(void *arg);



typedef struct
{
	pthread_t thread_tid;
	long thread_count;
} thread_t;

int childnum = 1;
thread_t *ptr;
int socket_fd;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * create thread's poll to handle (+lock) ,child thread select.
 * @return  0
 */
int main(int argc, char **argv)
{

	int res;
	res = register_sig_handler();
	if (res < 0)
		err_sys("register_sig_handler is err");

	my_getopt(argc, argv);

	struct sigaction myact;
	myact.sa_handler = sig_int_handler;
	if (sigaction(SIGINT, &myact, NULL) < 0)
		err_sys("sigaction err");

	
	res = srv_socket_init(&socket_fd, 50, PORT);
	if (res != 0)
		err_sys("srv_socket_init err");
	
	ptr= (thread_t *) malloc(sizeof(thread_t) * childnum);

	for (int i = 0; i < childnum; i++)
	{
		res = pthread_create(&ptr[i].thread_tid, NULL, thread_main, (void *)i);
		if (res < 0)
			err_sys("pthread_create is err");
	}

	while(1)
		pause();

	return 0;
}

/**
 * parse argument of main
 * @param argc number of arguments of main
 * @param argv arguments of main
 */
void my_getopt(int argc, char **argv)
{
	int opt;	
	while((opt = getopt(argc, argv, "t:")) != -1)
	{
		switch(opt)
		{
			case 't':
				childnum = atoi(optarg);
				break;
			default:
				usage(argv[0]);
				break;
		}
	} 
}

/**
 * main thread
 * @param  arg number i
 * @return     void 
 */
void * thread_main(void *arg)
{
	pthread_detach(pthread_self());
	int tidnum = (int)arg; 
	int newfd;
	pthread_t tid = pthread_self();
	int res;

	while(1)
	{
		pthread_mutex_lock(&lock);
		while(1)
		{
			res = srv_socket_accept(socket_fd, &newfd, 5);
			if (res != 0)
			{
				if (res == SOCKET_TIMEOUT_ERR)
				{
					fprintf(stderr, "id is %u, server socket accept time out: %d\n", tid, res);
				}
				else 
				{
					fprintf(stderr, "id is %u, srv_socket_accept err: %d\n", tid, res);
				}
				continue;
			}
			else 
			 	break;		
		}

		pthread_mutex_unlock(&lock);
		ptr[tidnum].thread_count++;
		fprintf(stdout, "\n");
		fprintf(stdout, "id is %u accept success.\n", tid);
		child_process(newfd);
		srv_socket_close(&newfd);	
	}

	return NULL;

}

/**
 * child process functon
 * @param  newfd coonection file descriptor
 * @return       void
 */
void child_process(int newfd)
{
	int res;
	char rec_buf[MAX_BUF_SIZE];
	int buflen;
	char send_buf[MAX_BUF_SIZE];
	//long connection, it close socket until client is closed
	while(1)
	{
		memset(rec_buf, 0, sizeof(rec_buf));
		memset(send_buf, 0, sizeof(send_buf));		
		res = srv_socket_rev(newfd, rec_buf, &buflen, 5);
		if (res != 0)
		{
			//because time out,continue to receive buffer 
			if (res == SOCKET_TIMEOUT_ERR)
			{
				fprintf(stdout, "read time out\n");
				continue ;
			}
			else if (res == SOCKET_UNCOON_ERR)
			{
				fprintf(stdout, "client is closed\n");
			}
			else 
			{
				fprintf(stderr, "srv_socket_rev err: %d\n", res);
				
			}
			return ;
		}
		//great, get the buffer from client
		else if (res == 0)
		{
			rec_buf[buflen] = '\0';
			fprintf(stdout, "buf from client is : %s\n", rec_buf);

			if(strcmp(rec_buf, "2\n") == 0)
			{
				fprintf(stdout, "server will close socket\n");
				return ;
			}

			strcpy(send_buf, rec_buf);

			for(int i = 0; i < 10; i++)
			{
				srv_socket_send(newfd, send_buf, sizeof(send_buf), 5);
				if (res != 0)
				{
					//because time out,continue to receive buffer
					if (res == SOCKET_TIMEOUT_ERR)
					{
						fprintf(stdout, "send time out\n");
						continue;
					}
					else if (res == SOCKET_UNCOON_ERR)
					{
						fprintf(stdout, "connection is closed\n");
					}
					else
					{
						fprintf(stdout, "srv_socket_send err\n");
					}
					return ;

				}
				else
					break; //send buffer correctly, then continue to receive
				
			}

	
		}	
	}
}

/**
 * printf user and system time
 */
void pr_cpu_time(void)
{
	int res;
	struct rusage myusage, childusage;
	res = getrusage(RUSAGE_SELF, &myusage);
	if (res < 0)
		return;

	res = getrusage(RUSAGE_CHILDREN, &childusage);
	if (res < 0)
		return;

	double user;
	double sys;
	user = myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/1e6;
	user += childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec/1e6;

	sys = myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/1e6;
	sys += childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/1e6;

	fprintf(stdout, "user time is %g, sys time is %g , total is %g s\n", user, sys, user + sys);
}

/**
 * register signal handler function
 * @return 0: success; -1: err
 */
int register_sig_handler()
{
	struct sigaction myact;
	myact.sa_handler = my_handler;

	if (sigaction(SIGPIPE, &myact, NULL) < 0)
	{ 
		fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
		return -1;
	}

	return 0;

}

/**
 * handle of signal
 * @param num signal number
 */
void my_handler(int num)
{
	int res;
	if (num == SIGCHLD)
	{
		int store_errno;
		store_errno = errno;
		fprintf(stdout, "get signal SIGCHLD : \n");
		//WNOHANG means when no child process is exited, return immediately 
		while((res = waitpid(-1, NULL, WNOHANG)) > 0)
		{
			printf("child pid is %d\n", res); 		
		}	
		errno = store_errno;
	}
	if (num == SIGPIPE)
	{
		fprintf(stdout, "pid is %d, get the signal SIGPIPS.\n", getpid());
		return ;
		//exit(0);
	}

}

/**
 * handler of signal SIGINT
 * @param num number signal 
 */
void sig_int_handler(int num)
{
	int res;
	if (num == SIGINT)
	{
		fprintf(stdout, "pid is %d, get signal SIGINT\n", getpid());
		while((res = waitpid(-1, NULL, 0)) > 0)
		{
			printf("int, child pid is %d\n", res); 
		}	
		pr_cpu_time();
		fprintf(stdout, "number of every process accept connection:\n");
		long sum = 0;
		for (int i = 0; i < childnum; i++)
		{
			sum += ptr[i].thread_count;
			fprintf(stdout, "%ld ", ptr[i].thread_count);
		}
		fprintf(stdout, "total connecion is %ld\n", sum);
		exit(0);
	}
}

/**
 * usage of the programe
 * @param argv argument of main
 */
void usage(char *argv)
{
	fprintf(stdout, "%s usage: %s [-t processnum]\n", argv,argv);
	exit(0);
}

