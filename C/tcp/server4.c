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
ssize_t read_fd(int fd, void *buf, size_t nbytes, int *recvfd);
ssize_t write_fd(int fd, void *buf, size_t nbytes, int sendfd);

typedef struct 
{
	int child_pid;
	int child_status;
	long child_count;
	int child_pipefd;
} child_t;

int childnum = 1;
child_t  *arraychild;
long *ptr;


/**
 * new connecion comes, copy file descroptor of new connecton to child 
 * process , parent process select 
 * @return  0
 */
int main(int argc, char **argv)
{

	int res;
	res = register_sig_handler();
	if (res < 0)
		err_sys("register_sig_handler is err");

	my_getopt(argc, argv);

	int pipefd[2];

	int socket_fd;
	res = srv_socket_init(&socket_fd, 50, PORT);
	if (res < 0)
		err_sys("srv_socket_init err");

	fd_set myset;
	FD_ZERO(&myset);
	FD_SET(socket_fd, &myset);
	int max = socket_fd;
	
	arraychild = (child_t *) malloc(sizeof(child_t) * childnum);
	memset(arraychild, 0, sizeof(child_t) * childnum);
	for (int i = 0 ; i < childnum; i++)
	{
		
		res = socketpair(AF_LOCAL, SOCK_STREAM, 0, pipefd);
		if (res < 0)
			err_sys("socketpair is err");
		arraychild[i].child_pipefd = pipefd[0];
		FD_SET(pipefd[0], &myset);
		if (pipefd[0] > max)
			max = pipefd[0];

		int pid = fork();
		if (pid < 0)
			err_sys("fork err");
		
		if (pid == 0)
		{
			srv_socket_destory(&socket_fd);

			int connfd;
			int childpid = getpid();
			while(1)
			{				
				
				char c;
				res = read_fd(pipefd[1], &c, 1, &connfd);
				if (res < 0)
					err_sys("read_fd err");
				fprintf(stdout, "pid is %d, accept success.\n",childpid);																																		
				child_process(connfd);
				
				write(pipefd[1], "", 1);

				srv_socket_close(&connfd);		
			}
			//exit(0);		
		}
		else 
		{
			arraychild[i].child_pid = pid;
		}
	}

	struct sigaction myact;
	myact.sa_handler = sig_int_handler;
	if (sigaction(SIGINT, &myact, NULL) < 0)
		err_sys("sigaction err");

	int navail = childnum;
	fd_set rset ;
	int newfd;
	int i = 0;
	while(1)
	{
		rset = myset;
		if (navail <=0 )
			FD_CLR(socket_fd, &rset);
		

		select(max + 1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(socket_fd, &rset))
		{
			newfd = accept(socket_fd, NULL, NULL);

			for (i = 0; i < childnum; i++)
			{
				if (arraychild[i].child_status == 0)
				break;
			}

			res = write_fd(arraychild[i].child_pipefd, "", 1, newfd);
			if (res < 0)
			{
				continue;
			}
			srv_socket_close(&newfd);
			arraychild[i].child_status = 1;
			arraychild[i].child_count++;
			navail--;
		} 
		for (int i = 0; i < childnum; i++)
		{			
			char c;
			if (FD_ISSET(arraychild[i].child_pipefd, &rset))
			{
				read(arraychild[i].child_pipefd, &c, 1);
				arraychild[i].child_status = 0;
				navail++;
			}
		}
	}

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
		printf("number of every process accept connection:\n");
		long sum = 0;
		for (int i = 0; i < childnum; i++)
		{
			sum += arraychild[i].child_count;
			printf("%ld ", arraychild[i].child_count);
		}
		printf("toal connection is %ld\n", sum);
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
 * read file descriptor 
 * @param  fd     file descriptor to read
 * @param  buf    read buffer
 * @param  nbytes nength of buffer
 * @param  recvfd file descriptor 
 * @return        >0: all bytes including buf ,and msg_control; <=0: err 
 */
ssize_t read_fd(int fd, void *buf, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	iov[0].iov_base = buf;
	iov[0].iov_len = nbytes;

	char control[CMSG_SPACE(sizeof(int))];
	
	msg.msg_control = control;
	msg.msg_controllen = CMSG_SPACE(sizeof(int));

	int n;
	if ((n = recvmsg(fd, &msg, 0)) <= 0)
		return n;

	struct cmsghdr *phdr;
	if ((phdr = CMSG_FIRSTHDR(&msg)) != NULL)
	{
		if (phdr->cmsg_len != CMSG_LEN(sizeof(int)) || 
			 phdr->cmsg_level != SOL_SOCKET || 
			 phdr->cmsg_type != SCM_RIGHTS)
			return -1;

		*recvfd = *(int *)(CMSG_DATA(phdr)); 
	}
	else 
	{
		*recvfd = -1;
		return -1;
	}	
	
	return n;

}

/**
 * write file descriptor
 * @param  fd     file descriptor to write
 * @param  buf    buffer to write
 * @param  nbytes number of byres
 * @param  sendfd file descriptor to send
 * @return        >0: success; -1: err
 */
ssize_t write_fd(int fd, void *buf, size_t nbytes, int sendfd)
{
	struct msghdr  msg;
	struct iovec iov[1];

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	iov[0].iov_base = buf;
	iov[0].iov_len = nbytes;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	struct cmsghdr *pmsg;


	char control[CMSG_SPACE(sizeof(int))];
	
	msg.msg_control = control;
	msg.msg_controllen = CMSG_SPACE(sizeof(int));
	pmsg = CMSG_FIRSTHDR(&msg);

	pmsg->cmsg_len = CMSG_LEN(sizeof(int));
	pmsg->cmsg_level = SOL_SOCKET;
	pmsg->cmsg_type = SCM_RIGHTS;
	*(int *)CMSG_DATA(pmsg) = sendfd;
	int res;
	res = sendmsg(fd, &msg, 0);
	return res;

}