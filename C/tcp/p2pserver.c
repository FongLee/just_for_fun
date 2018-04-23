#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "commsocket.h"

/**
 * parent process get data from client, child process 
 * send data from stdin to client
 * when parent process detects that client is closed, it 
 * closes itself and send siganl to child process to let 
 * child process close.
 */

void server_handler(int num)
{
	fprintf(stdout, "child process exit\n");
	exit(0);
}

int main(void)
{
	int res; 
	//int srv_socket_init(int *listenfd, int listennum, int port);
	int socket_fd;
	res = srv_socket_init(&socket_fd, 50, 1234);
	if (res != 0)
	{
		fprintf(stderr, "srv_socket_init err: %d\n", res);
		exit(0);
	}

	struct sigaction my_act;
	my_act.sa_handler = server_handler;

	if ( sigaction(SIGUSR1, &my_act, NULL) < 0)
	{
		fprintf(stderr, "sigaction err : %s\n", strerror(errno));
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
				//fprintf(stderr, "srv_socket_accept err: %d\n", res);
				fprintf(stderr, "srv_socket_accept err: %d\n", res);
				continue;
			}
			exit(0);
		}
		fprintf(stdout, "accept success.\n");

		int pid = 0;
		pid = fork();
		if (pid == -1)
		{
			fprintf(stderr, "fork err: %d\n", res);
			exit(0);
		}
		//parent process : receive buf
		else if (pid > 0)
		{
			char rec_buf[MAX_BUF_SIZE];
			int buflen;
			memset(rec_buf, 0, sizeof(rec_buf));

			while(1)
			{
				//int srv_socket_rev(int connfd, char *buf, int *buflen, int revtime);
				
				res = srv_socket_rev(newfd, rec_buf, &buflen, 5);
				if (res != 0)
				{
					if (res == SOCKET_TIMEOUT_ERR)
					{
						fprintf(stdout, "read time out\n");
					}
					else if (res == SOCKET_UNCOON_ERR)
					{
						fprintf(stdout, "client is closed\n");
						kill(pid, SIGUSR1);
						sleep(1);
						srv_socket_close(&newfd);
						break;
						//srv_socket_destory(&socket_fd);
						//exit(0);
					}
					else 
					{
						fprintf(stderr, "srv_socket_rev err: %d\n", res);

					}
				}
				else if (res == 0)
				{
					fprintf(stdout, "buf from client is %s\n", rec_buf);
					memset(rec_buf, 0, sizeof(rec_buf));
				}	
				
			}
		}
		//child process : send buf
		else if (pid == 0)
		{
			char send_buf[MAX_BUF_SIZE];

			while(fgets(send_buf, MAX_BUF_SIZE, stdin) != NULL)
			{
				while(1)
				{	
					//int srv_socket_send(int connfd, char *buf, int buflen, int sendtime)
					srv_socket_send(newfd, send_buf, sizeof(send_buf), 5);
					if (res != 0)
					{
						if (res == SOCKET_TIMEOUT_ERR)
						{
							fprintf(stdout, "send time out\n");
						}
						else if (res == SOCKET_UNCOON_ERR)
						{
							fprintf(stdout, "connection is closed\n");
							//exit(0);
							//break;
						}
						fprintf(stdout, "srv_socket_send err\n");
					}
					else if (res == 0)
					{
						//write(newfd, send_buf, strlen(send_buf));
						memset(send_buf, 0, sizeof(send_buf));
						break;
					}
				}
				
			}

		}

	}	
	return 0;
	// }

}