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


#include "commsocket.h"

#define PORT 		1234

void my_handler(int num)
{
	if (num == SIGCHLD)
	{
		fprintf(stdout, "get signal SIGCHLD\n");
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
		fprintf(stdout, "get the signal SIGPIPS.\n");
		exit(0);
	}
}

int main(void)
{
	//signal(SIGCHLD, SIG_IGN);
	struct sigaction myact;
	myact.sa_handler = my_handler;
	if (sigaction(SIGCHLD, &myact, NULL) < 0)
	{ 
		fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
	}
	if (sigaction(SIGPIPE, &myact, NULL) < 0)
	{ 
		fprintf(stderr, "sigaction is err : %s\n", strerror(errno));
	}
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
				fprintf(stderr, "srv_socket_accept err: %d\n", res);
				continue;
			}
			fprintf(stderr, "srv_socket_accept err: %d\n", res);
			continue;
			//exit(0);
		
		}
		fprintf(stdout, "accept success.\n");

		int pid = fork();
		if  (pid < 0)
		{
			srv_socket_close(&newfd);
			srv_socket_destory(&socket_fd);
			exit(0);
		}
		//child process 
		else if (pid == 0)
		{
			char rec_buf[MAX_BUF_SIZE];
			int buflen;
			memset(rec_buf, 0, sizeof(rec_buf));

			char send_buf[MAX_BUF_SIZE];
			memset(send_buf, 0, sizeof(rec_buf));

			srv_socket_destory(&socket_fd);
			while (1)
			{
				
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
						srv_socket_close(&newfd);
						exit(0);
					}
					else 
					{
						fprintf(stderr, "srv_socket_rev err: %d\n", res);
						continue;

					}
				}
				else if (res == 0)
				{
					rec_buf[buflen] = '\0';
					fprintf(stdout, "buf from client is %s\n", rec_buf);

					if(strcmp(rec_buf, "2\n") == 0)
					{
						fprintf(stdout, "server will close socket\n");
						shutdown(newfd, SHUT_RDWR);
						exit(0);
	
					}

					strcpy(send_buf, rec_buf);
					
					// for (int i = buflen - 1; i >= 0; i++)
					// {
					// 	send_buf[buflen - i - 1] = rec_buf[i];
					// }
					//strcpy(send_buf, strfry(rec_buf));
					while (1)
					{
						srv_socket_send(newfd, send_buf, sizeof(send_buf), 5);
						if (res != 0)
						{
							if (res == SOCKET_TIMEOUT_ERR)
							{
								fprintf(stdout, "send time out\n");
								continue;
							}
							else if (res == SOCKET_UNCOON_ERR)
							{
								fprintf(stdout, "connection is closed\n");
								srv_socket_close(&newfd);
								exit(0);
							}
							fprintf(stdout, "srv_socket_send err\n");
							continue;
						}
						else if (res == 0)
						{
							//write(newfd, send_buf, strlen(send_buf));
							break;
						}	
					}
	
					memset(send_buf, 0, sizeof(send_buf));
					memset(rec_buf, 0, sizeof(rec_buf));
				}	
			}

		}
		// explicit calls is needed when call function shutdown it is not needed.
		srv_socket_close(&newfd);

	}
	srv_socket_destory(&socket_fd);
	return 0;
}

