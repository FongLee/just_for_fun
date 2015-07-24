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
void my_accept(int socket_fd, int  *newfd);
void child_process(int newfd);
void pr_cpu_time(void);
int register_sig_handler();
void my_handler(int num);
void sig_int_handler(int num);
void usage(char *argv);
int shared_memory_get();
int shared_memory_init();
int shared_memory_rm();
int my_lock_init();
void my_lock_wait();
void my_lock_release();

int childnum = 1;
int *arraypid;
long *ptr;
static pthread_mutex_t *mptr;

/**
 * new connecion comes, create progress (accept)  to handle. select + lock
 * @return  0
 */
int main(int argc, char **argv)
{

	int res;
	res = register_sig_handler();
	if (res < 0)
		err_sys("register_sig_handler is err");

	my_getopt(argc, argv);
	res = shared_memory_init();
	if (res < 0)
		err_sys("shared_memory_init err");

	res = my_lock_init();
	if (res < 0)
		err_sys("my_lock_init err");

	int socket_fd;
	res = srv_socket_init(&socket_fd, 50, PORT);
	if (res < 0)
		err_sys("srv_socket_init err");
	
	arraypid = (int *) malloc(sizeof(int) * childnum);
	for (int i = 0 ; i < childnum; i++)
	{
		int pid = fork();
		if (pid < 0)
			err_sys("fork err");
		
		if (pid == 0)
		{
			int newfd;
			int childpid = getpid();
			while(1)
			{				
				my_lock_wait();
				my_accept(socket_fd, &newfd);
				my_lock_release();
				ptr[i]++;
				fprintf(stdout, "\n");
				fprintf(stdout, "pid is %d, accept success.\n",childpid);

				child_process(newfd);
				srv_socket_close(&newfd);		
			}
			//exit(0);		
		}
		else 
		{
			arraypid[i] = pid;
		}
	}

	struct sigaction myact;
	myact.sa_handler = sig_int_handler;
	if (sigaction(SIGINT, &myact, NULL) < 0)
		err_sys("sigaction err");

	srv_socket_destory(&socket_fd);

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
 * accept 
 * @param socket_fd file descriptor of listening socket
 * @param newfd     file descriptor of connection socket
 */
void my_accept(int socket_fd, int *newfd)
{
	int res;
	int childpid = getpid();			
	while(1)
	{
		res = srv_socket_accept(socket_fd, newfd, 5);
		if (res != 0)
		{
			if (res == SOCKET_TIMEOUT_ERR)
			{
				fprintf(stderr, "pid is %d, server socket accept time out: %d\n", childpid, res);
				
			}
			else 
			{
				fprintf(stderr, "pid is %d, srv_socket_accept err: %d\n", childpid, res);
			}
			continue;
			//exit(0);
		}
		break;
	}
}

/**
 * child process functon
 * @param  newfd coonection file descriptor
 * @return       void
 */
void   child_process(int newfd)
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

	fprintf(stdout, "user time is %g, sys time is %g, total is %g\n", user, sys, user+sys);
}

/**
 * register signal handler function
 * @return 0: success; -1: err
 */
int register_sig_handler()
{
	struct sigaction myact;
	myact.sa_handler = my_handler;
	// if (sigaction(SIGCHLD, &myact, NULL) < 0)
	// { 
	// 	fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
	// 	return -1;
	// }

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
		shared_memory_rm();
		fprintf(stdout, "number of every process accept connection:\n");
		long sum=0;
		for(int i = 0; i < childnum; i++)
		{
			sum += ptr[i];
			
			fprintf(stdout, " %ld ", ptr[i]);
		}

		fprintf(stdout, "total connection is %ld.\n", sum);

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

/**
 * get id of shared memory 
 * @return >0:shmid; -1: err
 */
int shared_memory_get()
{
	key_t key;
	key = ftok("./", 0);
	if (key < 0)
		return -1;

	int shmid;
	shmid = shmget(key, childnum * sizeof(long), 0644 | IPC_CREAT);
	if (shmid < 0)
		return -1;

	return shmid;
}

/**
 * shared memory initialize 
 * @return 0: success;-1: err
 */
int shared_memory_init()
{
	int shmid;
	shmid = shared_memory_get();
	if (shmid < 0)
		return -1;

	ptr = shmat(shmid, NULL, 0);
	if (ptr == (void *) -1)
		return -1;

	for(int i = 0; i < childnum; i++)
	{
		*ptr = 0;
		 ptr++;
	}

	return 0;
		
}

/**
 * remove shared memory 
 * @return 0: success;-1: err
 */
int shared_memory_rm()
{
	int shmid;
	shmid = shared_memory_get();
	if (shmid < 0)
		return -1;
	int res ;
	res = shmctl(shmid, IPC_RMID, NULL);
	if (res < 0)
		return -1;

	return 0;
}

int my_lock_init()
{
	key_t key;
	key = ftok("./", 1);
	if (key < 0)
		return -1;

	int shmid;
	shmid = shmget(key, sizeof(pthread_mutex_t), 0644 | IPC_CREAT);
	if (shmid < 0)
		return -1;

	mptr = shmat(shmid, NULL, 0);
	if (mptr == (void *) -1)
		return -1;
	int res;
	res = pthread_mutex_init(mptr, NULL);
	if (res < 0)
		return -1;

	return 0;

}

void my_lock_wait()
{
	pthread_mutex_lock(mptr);
}

void my_lock_release()
{
	pthread_mutex_unlock(mptr);
}