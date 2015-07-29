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
#include <stdbool.h>
#include <semaphore.h>

#include <queue>

#include "commsocket.h"
#include "error.h"

using namespace std;
#define PORT 		1234

void usage(char *argv);
void my_getopt(int argc, char **argv);
void thread_pool_init();
void thread_make(int i);
void * thread_main(void *arg);
void connection_handler(int newfd);
void pr_cpu_time(void);
int register_sig_handler();
void my_handler(int num);


const int MAXTHREADS = 50;
const int MINTHREADS = 15;
const int MIN_IDLETHREADS = 2;
const int ADD_THREADS = 5;

typedef struct
{
	bool thread_flag;
	pthread_t thread_tid;
	long thread_count;
} thread_info_t;

typedef struct thread_pool
{
	int max_threads; 
	int curr_threads;
	int idle_threads;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	thread_info_t *pchild;
} thread_pool_t;

int childnum = 1;


queue<int> childfd;

thread_pool_t my_pool;

/**
 * main thread handle new connection ,  thread poll is to handle.
 * @return  0
 */
int main(int argc, char **argv)
{

	int res;
	res = register_sig_handler();
	if (res < 0)
		err_sys("register_sig_handler is err");

	my_getopt(argc, argv);

	int socket_fd;
	res = srv_socket_init(&socket_fd, 50, PORT);
	if (res != 0)
		err_sys("srv_socket_init err");
	

	thread_pool_init();

	int num;
	int myfd;
	while(1)
	{
		while(1)
		{
			res = srv_socket_accept(socket_fd, &myfd, 5);
			if (res != 0)
			{
				if (res == SOCKET_TIMEOUT_ERR)
				{
					fprintf(stderr, "server socket accept time out: %d\n", res);
				}
				else 
				{
					fprintf(stderr, "srv_socket_accept err: %d\n", res);
				}
				continue;
			}
			else 
			 	break;		
		}

		pthread_mutex_lock(&my_pool.mutex);

		childfd.push(myfd);
		num = my_pool.idle_threads - childfd.size();
		if (num < MIN_IDLETHREADS && my_pool.curr_threads <= MAXTHREADS - 5)
		{			
				int j = 0;
				int k = 0;
				while (j < MAXTHREADS)
				{
					
					if (my_pool.pchild[j].thread_flag == false)
					{
						//my_pool.pchild[j].thread_flag = true;
						thread_make(j);
						//err_ret("make a new pthread");
						k++;
					}
					if (k == ADD_THREADS )
						break;
					
					j++;
				}

				//err_ret("make   pthread");
		}
	
		pthread_cond_signal(&my_pool.cond);

		pthread_mutex_unlock(&my_pool.mutex);

	}

	return 0;
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

void thread_pool_init()
{
	my_pool.max_threads = MAXTHREADS;
	my_pool.curr_threads = MINTHREADS;
	my_pool.idle_threads = MINTHREADS;
	pthread_mutex_init(&my_pool.mutex, NULL);
	pthread_cond_init(&my_pool.cond, NULL);
	my_pool.pchild= (thread_info_t *) malloc(sizeof(thread_info_t) * MAXTHREADS);
	memset(my_pool.pchild, 0, sizeof(thread_info_t) * MAXTHREADS);
	for (int i = 0; i < MAXTHREADS; i++)
		my_pool.pchild[i].thread_flag = false;

	for (int i = 0; i < MINTHREADS; i++)
	{
		thread_make(i);
	}
}

void thread_make(int i)
{
	int res;
	res = pthread_create(&my_pool.pchild[i].thread_tid, NULL, thread_main, (void *)i);
	if (res < 0)
		err_ret("pthread_create is err");

	my_pool.pchild[i].thread_flag = true;

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
	while(1)
	{
		pthread_mutex_lock(&my_pool.mutex);
		while(childfd.empty())
			pthread_cond_wait(&my_pool.cond, &my_pool.mutex);

		newfd = childfd.front();
		childfd.pop();

		my_pool.idle_threads--;
		// my_pool.pchild[tidnum].thread_count++; 
		pthread_mutex_unlock(&my_pool.mutex);

		my_pool.pchild[tidnum].thread_count++;
	
		fprintf(stdout, "\n");
		fprintf(stdout, "the %dst thread accept success.\n", (unsigned int)tidnum);
		connection_handler(newfd);
		srv_socket_close(&newfd);	
		
		pthread_mutex_lock(&my_pool.mutex);
		
		int num = my_pool.idle_threads - childfd.size();
		if (num > my_pool.curr_threads / 2 && my_pool.curr_threads > MINTHREADS)
		{
			my_pool.curr_threads--;
			my_pool.idle_threads--;
			my_pool.pchild[tidnum].thread_flag = false;
			pthread_mutex_unlock(&my_pool.mutex);

			return NULL;
		}
		my_pool.idle_threads++;
		pthread_mutex_unlock(&my_pool.mutex);

	}
		return NULL;

}

/**
 * child process functon
 * @param  newfd connection file descriptor
 * @return       void
 */
void connection_handler(int newfd)
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
			int k = 0;
			//because time out,continue to receive buffer 
			if (res == SOCKET_TIMEOUT_ERR)
			{
				k++;
				if (k == 3)
					return ;
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

	fprintf(stdout, "user time is %g s, sys time is %g s, total is %g s\n", user, sys, user + sys);
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
		err_ret("sigaction is err");
		return -1;
	}

	if (sigaction(SIGINT, &myact, NULL) < 0)
	{
		err_ret("sigaction err");
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
		for (int i = 0; i < MAXTHREADS; i++)
		{
			sum += my_pool.pchild[i].thread_count;
			fprintf(stdout, "%ld ", my_pool.pchild[i].thread_count);
		}

		fprintf(stdout, "\ntotal connection is %ld\n", sum);
		fprintf(stdout, "number of current thread is %d, number of idle_threads is %d\n", \
					my_pool.curr_threads, my_pool.idle_threads);
		exit(0);
	}

}




