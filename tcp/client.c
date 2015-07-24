#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "commsocket.h"

#define CONTRY 5

#define SERVERIP  	"192.168.200.147"
//#define SERVERIP  	"192.168.1.113"
#define PORT 		1234

void my_handler(int num)
{
	if (num == SIGPIPE)
	{
		fprintf(stdout, "get the signal SIGPIPS.\n");
		exit(0);
	}
}

int main(void)
{
	struct sigaction my_act;
	my_act.sa_handler = my_handler;
	sigaction(SIGPIPE, &my_act, NULL);

	void * sockhandler;
	int res;
	res = clt_socket_init(&sockhandler, 10, 5, 5);
	if (res != 0)
	{
		fprintf(stderr, "clt_socket_init, %d\n", res);
	}

	int connfd;
	int connretry = 0;

	fprintf(stderr, "begin to connect\n");

	res = clt_socket_getcoon(sockhandler, SERVERIP, PORT, &connfd);

	if (res != 0)
	{
		fprintf(stderr, "clt_socket_getcoon err :%d\n", res);
		exit(0);
	}
	fprintf(stdout, "connection success.\n");
	 
	char send_buf[MAX_BUF_SIZE] = {0};
	char rec_buf[MAX_BUF_SIZE] = {0};
	int rvlen;

	fd_set myset;
	struct timeval mytime;

	FD_ZERO(&myset);
	FD_SET(connfd, &myset);
	FD_SET(STDIN_FILENO, &myset);
	mytime.tv_sec = 5;
	mytime.tv_usec = 0;

	while(1)
	{
		mytime.tv_sec = 5;
		mytime.tv_usec = 0;
		FD_ZERO(&myset);
		FD_SET(connfd, &myset);
		FD_SET(STDIN_FILENO, &myset);
		do 
		{
			res = select (connfd + 1, &myset, NULL, NULL, &mytime);
		} while(res < 0 && errno == EINTR);

		if (res < 0)
		{
			
			fprintf(stderr, "select err :%s\n", strerror(errno));
			continue;
		}
		else if (res == 0)
		{
			fprintf(stderr, "time out \n");
			continue;
		}
		else 
		{
			
	 		if (FD_ISSET(connfd, &myset))
			{
				
				res = clt_socket_rev(sockhandler, connfd, rec_buf, &rvlen);
				if (res != 0)
				{
					if (res == SOCKET_TIMEOUT_ERR)
					{
						fprintf(stdout, "read time out\n");
					}
					else if (res == SOCKET_UNCOON_ERR)
					{
						fprintf(stdout, "server is closed\n");
						clt_socket_closeconn(&connfd);
						clt_socket_destory(&sockhandler);
						exit(0);
					}
					else 
					{
						fprintf(stderr, "clt_socket_rev err: %d\n", res);
						exit(0);
					}

				}
				else if (res == 0)
				{
					rec_buf[rvlen] = '\0';
					fprintf(stdout, "buf from server is %s\n", rec_buf);
					memset(rec_buf, 0, sizeof(rec_buf));
					continue;
				}

			}

			if (FD_ISSET(STDIN_FILENO, &myset))
			{
				if (fgets(send_buf, sizeof(send_buf), stdin) == NULL)
					continue;

				res = clt_socket_send(sockhandler, connfd, send_buf, strlen(send_buf));
				if (res != 0)
				{
					if (res == SOCKET_TIMEOUT_ERR)
					{
						fprintf(stderr, "write time out.\n");
						continue;
					}
					if (res == SOCKET_UNCOON_ERR)
					{
						fprintf(stderr, "server is closed.\n");
						clt_socket_closeconn(&connfd);
						clt_socket_destory(&sockhandler);
						
						//exit(0);	
					}
					
					fprintf(stderr, "clt_socket_send err %d\n", res);
					exit(0);
				}
				memset(send_buf, 0, sizeof(send_buf));
			}
			
		}
	}
	
	clt_socket_closeconn(&connfd);
	clt_socket_destory(&sockhandler);
	return 0;
	
}

