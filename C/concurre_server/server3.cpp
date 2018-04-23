#include <sys/epoll.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>

#include "error.h"

#define BUFFERMAX 500

using namespace std;
typedef struct myepoll_event
{
	int fd;
	bool status;
	int send_offset;
	int recv_offset;
	char send_buffer[BUFFERMAX+1];
	char recv_buffer[BUFFERMAX+1];
	int (*callback)( struct myepoll_event * pev);
	time_t last_alive;
} myepoll_event_t;

typedef int (*callback_t)(myepoll_event_t * pev);

#define MAXEVENTS 500
#define PORT 	1234
#define LISTENNUM  10

int g_epollfd;
static long connsum = 0;
myepoll_event_t g_events[MAXEVENTS];

int set_event(int epfd, int fd, myepoll_event_t *pev);
int add_event(int epfd, uint32_t event,  myepoll_event_t * pev);
int mod_event(int epfd, uint32_t event, callback_t mycallback, myepoll_event_t * pev);
int del_event(int epfd, myepoll_event_t * pev);

int srv_init();
int search_idle(myepoll_event_t *pev, int length);
int myaccept(myepoll_event_t * pev);
int recv_data(myepoll_event_t * pev);
int send_data(myepoll_event_t *pev);
void pr_cpu_time(void);
int register_sig_handler();
void my_handler(int num);


/**
 * epoll handle new connection
 * @param  argc 
 * @param  argv 
 * @return      0
 */
int main(int argc, char const *argv[])
{
	if (srv_init() < 0)
		err_sys("srv_init err");

	register_sig_handler();
	struct epoll_event myevent[MAXEVENTS];
	myepoll_event_t *pev;
	int eventnum = 0;
	int checkpos = 0;
	time_t now = 0;
	long duration = 0;
	while(1)
	{	

		now = time(NULL);
		for (int i = 1; i < 100; i++, checkpos++)
		{
			if (checkpos == MAXEVENTS) 
				checkpos =0;
			if (!g_events[i].status)
				continue;
			duration = now - g_events[i].last_alive;

			if (duration >60)
			{
				close(g_events[i].fd);
				fprintf(stdout, "fd of connection : %d hasn't being active for a long time\n", g_events[i].fd);

			}
		}
		//fourth argument is milliseconds
		eventnum = epoll_wait(g_epollfd, &myevent[0], MAXEVENTS, 1000);
		if (eventnum < 0)
		err_sys("epoll_wait err");

		for (int i = 0; i < eventnum; i++)
		{
			pev = (myepoll_event_t *) myevent[i].data.ptr;
			pev->callback(pev);	
		}	
	}
	return 0;
}

/**
 * set a structure of myepoll_event_t 
 * @param  epfd       epoll file descripter
 * @param  fd         new file descripter, such as file descritper of connection
 * @param  mycallback event function
 * @param  pev        a poiner to myepoll_event_t
 * @return            0: success
 */
int set_event(int epfd, int fd, callback_t mycallback, myepoll_event_t *pev)
{
	pev->fd = fd;
	pev->status = false;
	pev->send_offset = 0;
	pev->recv_offset = 0;
	memset(pev->recv_buffer, 0, sizeof(pev->recv_buffer));
	memset(pev->send_buffer, 0, sizeof(pev->send_buffer));
	pev->callback = mycallback;
	pev->last_alive = time(NULL);
	return 0;
}

/**
 * add event to file descripter of epoll 
 * @param  epfd   epoll file descripter
 * @param  events EPOLLIN, EPOLLOUT
 * @param  pev    a poiner to myepoll_event_t
 * @return        0: success; -1: err
 */
int add_event(int epfd, uint32_t events, myepoll_event_t * pev)
{
	struct epoll_event myepoll;
	myepoll.events = events;
	myepoll.data.ptr = pev;
	pev->status = true;
	pev->last_alive = time(NULL);
	int res;
	res = epoll_ctl(epfd, EPOLL_CTL_ADD, pev->fd, &myepoll);

	return res;
}

/**
 * modify event of pev->fd to epoll
 * @param  epfd       epoll file descripter
 * @param  events     EPOLLIN, EPOLLOUT
 * @param  mycallback event function
 * @param  pev        a poiner to myepoll_event_t
 * @return            0: success; -1: err
 */
int mod_event(int epfd, uint32_t events, callback_t mycallback, myepoll_event_t * pev)
{
	struct epoll_event myepoll;
	myepoll.events = events;
	myepoll.data.ptr = pev;
	pev->status = true;
	pev->callback = mycallback;
	pev->last_alive = time(NULL);
	int res;
	res = epoll_ctl(epfd, EPOLL_CTL_MOD, pev->fd, &myepoll);
	return res;
}

/**
 * delete pev->fd from epoll
 * @param  epfd epoll file descripter
 * @param  pev  a poiner to myepoll_event_t
 * @return      0: success; -1: err
 */
int del_event(int epfd, myepoll_event_t * pev)
{
	struct epoll_event mypoll;
	mypoll.data.ptr = pev;
	pev->status = false;
	int res ;
	res = epoll_ctl(epfd, EPOLL_CTL_DEL, pev->fd, &mypoll);
	return res;
}

///////////////////////////////////////////////////////////////////

/**
 * server initialization
 * @return 0: success; -1: err
 */
int srv_init()
{
	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		return -1;

	int res;
	int optval = 1;
	res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (res < 0)
		return -1;

	int flags;
	if ((flags = fcntl(listenfd, F_GETFL, 0)) < 0)
		return -1;

	flags &= ~O_NONBLOCK;
	if (fcntl(listenfd, F_SETFL, flags) < 0)
		return -1;

	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(PORT);
	ser_addr.sin_addr.s_addr = INADDR_ANY;

	res = bind(listenfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if (res < 0)
		return -1;

	if (listen(listenfd, LISTENNUM) < 0)
		return -1;

	g_epollfd =  epoll_create(MAXEVENTS);
	if (g_epollfd < 0)
		return -1;

	for (int i = 0; i < MAXEVENTS; i++)
	{
		g_events[i].fd = -1;
		g_events[i].status = 0;
		memset(g_events[i].send_buffer, 0, sizeof(g_events[i].send_buffer));
		memset(g_events[i].recv_buffer, 0, sizeof(g_events[i].send_buffer));
		g_events[i].callback = NULL;
		g_events[i].last_alive = 0;
	}

	set_event(g_epollfd, listenfd, myaccept, &g_events[0]);
	add_event(g_epollfd, EPOLLIN, &g_events[0]);
	return 0;
}

/**
 * search  idle myepoll_event_t
 * @param  pev    Note: address of the first element 
 * @param  length length of array 
 * @return        >=0: success, idle index; -1: err
 */
int search_idle(myepoll_event_t *pev, int length)
{
	int i = 0;
	for (i = 0; i < length; i++)
	{
		if (!pev[i].status)
			return i;
	}

	return -1;
}

/**
 * accept client's connection , a EPOLLIN event function 
 * @param  pev a poiner to myepoll_event_t
 * @return     0: success; -1: err
 */
int myaccept(myepoll_event_t * pev)
{
	int i_idle = 0;
	i_idle = search_idle(&g_events[0], MAXEVENTS);
	//exceed the limit , max number of events 
	//being  monitored is limited 
	if (i_idle < 0)
	{
		err_msg("out of limit");
		return -1;
	}
		

	int newfd;
	newfd = accept(pev->fd, NULL, NULL);
	if (newfd < 0)
		return -1;
	
	set_event(g_epollfd, newfd, recv_data, &g_events[i_idle]);
	add_event(g_epollfd, EPOLLIN, &g_events[i_idle]);
	return 0;
}

/**
 * receive data from client, a EPOLLIN event function
 * @param  pev a poiner to myepoll_event_t
 * @return     0: success; -1: err
 */
int recv_data(myepoll_event_t * pev)
{
	int len = 0;
	len = read(pev->fd, pev->recv_buffer, BUFFERMAX);
	if (len < 0)
	{
		err_ret("read err");
		return -1;
	}
	if (len == 0)
	{
		close(pev->fd);
		connsum++;
		del_event(g_epollfd, pev);
	}
	else 
	{
		pev->recv_buffer[len] = '\0';
		fprintf(stdout, " recv buffer from client is %s\n", pev->recv_buffer);
		
		//very simple business process
		strcpy(pev->send_buffer, pev->recv_buffer);
		memset(pev->recv_buffer, 0, sizeof(pev->recv_buffer));

		mod_event(g_epollfd, EPOLLOUT, send_data, pev);
	}

	return 0;
}

/**
 * send data to client, a EPOLLOUT event function
 * @param  pev a poiner to myepoll_event_t
 * @return     0: succeed; -1: err
 */
int send_data(myepoll_event_t *pev)
{
	int len = 0;
	int buffer_len = strlen(pev->send_buffer);
	len = write(pev->fd, pev->send_buffer + pev->send_offset, buffer_len - pev->send_offset);
	if (len < 0)
	{
		err_ret("write err");
		return -1;
	}
	else 
	{
		pev->send_offset += len;
		if (pev->send_offset == buffer_len)
		{
			memset(pev->send_buffer, 0, sizeof(pev->send_buffer));
			mod_event(g_epollfd, EPOLLIN, recv_data, pev);
			pev->send_offset = 0;
		}	
	}
	return 0;
	
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

	if (num == SIGPIPE)
	{
		fprintf(stdout, "pid is %d, get the signal SIGPIPS.\n", getpid());
		return ;
		//exit(0);
	}
	if (num == SIGINT)
	{
		fprintf(stdout, "pid is %d, get signal SIGINT\n", getpid());
		pr_cpu_time();
		fprintf(stdout, "number of every process accept connection:\n");
		
		fprintf(stdout, "\ntotal connection is %ld\n", connsum);
		

		exit(0);
	}


}





