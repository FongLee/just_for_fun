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
#include <sys/wait.h>

#include "commsocket.h"
#include "error.h"

#define CONTRY 5

#define SERVERIP  	"192.168.1.102"
//#define SERVERIP  	"192.168.1.113"
#define PORT 		1234

void my_handler(int num);
void usage(char *argv);

/**
 * client test programe
 * @param  argc number of arguments
 * @param  argv [-p address] [-c connum] [-t processnum]
 * @return      0
 */
int main(int argc, char **argv)
{
	struct sigaction my_act;
	my_act.sa_handler = my_handler;
	sigaction(SIGPIPE, &my_act, NULL);

	char *address = NULL;
	int connum = 1;
	int pronum = 1;
	int opt;
	while((opt = getopt(argc, argv, "p:c:t:")) != -1)
	{
		switch(opt)
		{
			case 'p':
				address = optarg;
				break;
			case 'c':
				connum = atoi(optarg);
				break;
			case 't':
				pronum = atoi(optarg);
				break;
			default:
				usage(argv[0]);
				break;
		}
	}

	if (address == NULL)
		address = (char *)SERVERIP;


	void * sockhandler;
	int res;
	res = clt_socket_init(&sockhandler, 10, 5, 5);
	if (res != 0)
	{
		fprintf(stderr, "clt_socket_init, %d\n", res);
	}
	fprintf(stdout, "begin to connect\n");

	int connfd;
	char send_buf[MAX_BUF_SIZE] = {0};
	char rec_buf[MAX_BUF_SIZE] = {0};

	int pid = 0;
	for (int j = 0; j < pronum; j++)
	{
		pid = fork();
		if  (pid < 0)
		{
			//srv_socket_close(&newfd);
			fprintf(stderr, "fork err\n" );
			continue;
		}

		if (pid == 0)
		{
			int mypid = getpid();
			for (int i = 0; i < connum; i++)
			{
				res = clt_socket_getcoon(sockhandler, address, PORT, &connfd);

				if (res != 0)
				{
					fprintf(stderr, "pid  is %d, %dst connection, clt_socket_getcoon err :%d\n", mypid, i+1, res);
					continue;
				}
				memset(send_buf, 0, sizeof(send_buf));
				memset(rec_buf, 0, sizeof(rec_buf));

				fprintf(stdout, "pid  is %d, %dst connection success.\n",mypid,  i+1);
				sprintf(send_buf, "pid  is %d, %dst connection 's buf", mypid, i+1);
				res = writen(connfd, send_buf, strlen(send_buf));
				//res = clt_socket_send(sockhandler, connfd, send_buf, strlen(send_buf));
				if (res < 0)
				{
					err_ret("writen err");
					clt_socket_closeconn(&connfd);
					continue;
				}
				res = read(connfd, rec_buf, MAX_BUF_SIZE);
				if (res < 0)
				{
					err_ret("pid is %d, read err", getpid());
					clt_socket_closeconn(&connfd);
					continue;
				}
	
				rec_buf[res] = '\0';
				fprintf(stdout, "buf from server is : %s\n", rec_buf);
				fprintf(stdout, "\n");
				
				clt_socket_closeconn(&connfd);		
			}

			exit(0);
		}
	}

	while((res = waitpid(-1, NULL, 0)) > 0)
	{
		printf("child process exit,  child pid is %d\n", res); 
		
	}
	
	return 0;
	
}

/**
 * handler of signal
 * @param num signal number
 */
void my_handler(int num)
{
	if (num == SIGPIPE)
	{
		fprintf(stdout, "get the signal SIGPIPS.\n");
		exit(0);
	}
}

/**
 * usage of the programe
 * @param argv argument of main
 */
void usage(char *argv)
{
	fprintf(stdout, "%s usage: %s [-p address] [-c connum] [-t processnum]\n", argv,argv);
	exit(0);
}
