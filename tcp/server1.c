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

#include "commsocket.h"

#define PORT 		1234

void child_process(int newfd);
void pr_cpu_time(void);
int register_sig_handler();
void my_handler(int num);

/**
 * new connecion comes, fork child progress to handle.
 * @return  0
 */
int main(void)
{
	//signal(SIGCHLD, SIG_IGN);
	register_sig_handler();
	int socket_fd;
	int res;
	res = srv_socket_init(&socket_fd, 50, PORT);
	if (res != 0)
	{
		fprintf(stderr, "srv_socket_init err: %d\n", res);
		exit(0);
	}

	while(1)
	{
		int newfd;
		res = srv_socket_accept(socket_fd, &newfd, 5);
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
			//exit(0);
		}
		fprintf(stdout, "\n");
		fprintf(stdout, "accept success.\n");

		int pid ;
		pid = fork();
		if  (pid < 0)
		{
			fprintf(stderr, "fork is err.\n");
			srv_socket_close(&newfd);
			srv_socket_destory(&socket_fd);
			exit(0);
		}
		
		//child process 
		else if (pid == 0)
		{
			srv_socket_destory(&socket_fd);

			child_process(newfd);
			srv_socket_close(&newfd);
			exit(0);

		}
		// explicit calls is needed when call function shutdown it is not needed.
		//parent process execute
		srv_socket_close(&newfd);

	}
	srv_socket_destory(&socket_fd);
	return 0;
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

	fprintf(stdout, "user time is %g, sys time is %g, total time is %g\n", user, sys, user + sys);
}

/**
 * register signal handler function
 * @return 0: success; -1: err
 */
int register_sig_handler()
{
	struct sigaction myact;
	myact.sa_handler = my_handler;
	if (sigaction(SIGCHLD, &myact, NULL) < 0)
	{ 
		fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
		return -1;
	}
	if (sigaction(SIGPIPE, &myact, NULL) < 0)
	{ 
		fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
		return -1;
	}
	if (sigaction(SIGINT, &myact, NULL) < 0)
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
	if (num == SIGCHLD)
	{
		fprintf(stdout, "get signal SIGCHLD : \n");
		int res ;
		//WNOHANG means when no child process is exited, return immediately 
		while((res = waitpid(-1, NULL, WNOHANG)) > 0)
		{
			printf("child pid is %d\n", res); 
			
		}
		//wait(NULL);
	}
	if (num == SIGPIPE)
	{
		fprintf(stdout, "pid is %d, get the signal SIGPIPS.\n", getpid());
		exit(0);
	}

	if (num == SIGINT)
	{
		fprintf(stdout, "pid is %d, get signal SIGINT\n", getpid());
		pr_cpu_time();
		exit(0);
	}
}