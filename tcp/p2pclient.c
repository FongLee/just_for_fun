#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include "commsocket.h"

#define SERVERIP  	"192.168.1.103"
//#define SERVERIP  	"192.168.1.113"
#define PORT 		1234

void client_handler(int num)
{
	fprintf(stdout, "parent process exit\n");

	exit(0);
}

int main(void)
{
	int res;

	void * sockhandler;

	res = clt_socket_init(&sockhandler, 5, 5, 5);
	if (res != 0)
	{
		fprintf(stderr, "clt_socket_init, %d\n", res);
	}

	//int clt_socket_getcoon(void **sockhandle, char *ip, int port, int *connfd);
	int connfd;

	fprintf(stdout, "begin to connect to server.\n");
	res = clt_socket_getcoon(sockhandler, SERVERIP, PORT, &connfd);
	if (res != 0)
	{
		fprintf(stderr, "clt_socket_getcoon err :%d\n", res);
		exit(0);
	}
	fprintf(stdout, "connection success.\n");
	
	struct sigaction my_act;
	my_act.sa_handler = client_handler;

	if ( sigaction(SIGUSR1, &my_act, NULL) < 0)
	{
		fprintf(stderr, "sigaction err : %s\n", strerror(errno));
		exit(0);
	}

	int pid = 0;
	pid = fork();
	if (pid == -1)
	{
		fprintf(stderr, "fork err : %s\n", strerror(errno));
		exit(0);
	}
	//parent process send buffer from stdin
	else if (pid > 0)
	{
		char send_buf[MAX_BUF_SIZE];

		while(fgets(send_buf, MAX_BUF_SIZE, stdin) != NULL)
		{
			//int clt_socket_send(void *sockhandle, int connfd, unsigned char *buf, int buflen);
			res = clt_socket_send(sockhandler, connfd, send_buf, strlen(send_buf));
			if (res != 0)
			{
				if (res == SOCKET_TIMEOUT_ERR)
				{
					fprintf(stderr, "write time out.\n");
				}
				if (res == SOCKET_UNCOON_ERR)
				{
					fprintf(stderr, "server is closed.\n");

					kill(pid, SIGUSR1);
					clt_socket_closeconn(&connfd);
					clt_socket_destory(&sockhandler);
					exit(0);
				}
				
				fprintf(stderr, "clt_socket_getcoon err %d\n", res);
			}
			memset(send_buf, 0, sizeof(send_buf));
		}

	}
	//child process receive buffer
	else if (pid == 0)
	{
		char rec_buf[MAX_BUF_SIZE];
		int rvlen;
		memset(rec_buf, 0, sizeof(rec_buf));
		while(1)
		{
			//int clt_socket_rev(void *sockhandle, int connfd, unsigned char *buf, int *buflen);
			res = clt_socket_rev(sockhandler, connfd, rec_buf, &rvlen);
			if (res != 0)
			{
				if (res == SOCKET_TIMEOUT_ERR)
				{
					fprintf(stdout, "read time out\n");
					continue;
				}
				else if (res == SOCKET_UNCOON_ERR)
				{
					fprintf(stdout, "server is closed\n");
					clt_socket_closeconn(&connfd);
					clt_socket_destory(&sockhandler);
					kill(getppid(), SIGUSR1);
					exit(0);
				}
				else 
				{
					fprintf(stderr, "clt_socket_rev err: %d\n", res);
					continue;
					//maybe close child process better
					

				}

			}
			else if (res == 0)
			{
				fprintf(stdout, "buf from server is %s\n", rec_buf);
				memset(rec_buf, 0, sizeof(rec_buf));
			}
			
		}
	}

	return 0;
}