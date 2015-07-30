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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>

#include "error.h"

using namespace std;

#define BUFFERMAX 500

typedef struct myepoll_event
{
	int fd;
	int status;
	int send_offset;
	int recv_offset;
	char send_buffer[BUFFERMAX+1];
	char recv_buffer[BUFFERMAX+1];
	int (*callback)( struct myepoll_event * pev);
	time_t last_alive;
} myepoll_event_t;

typedef int (*callback_t)(myepoll_event_t * pev);

typedef struct progress_pool
{
	int current_progress;
	int index;
	int listenfd;
	pthread_mutex_t *mptr;
} progress_pool_t;

int set_event(int epfd, int fd, callback_t mycallback, myepoll_event_t *pev);
int add_event(int epfd, uint32_t event,  myepoll_event_t * pev);
int mod_event(int epfd, uint32_t event, callback_t mycallback, myepoll_event_t * pev);
int del_event(int epfd, myepoll_event_t * pev);

int srv_init();
int epoll_init();
void progress_handle();
int add_listen();
int del_listen();
int search_idle(myepoll_event_t *pev, int length);
int myaccept(myepoll_event_t * pev);
int recv_data(myepoll_event_t * pev);
int send_data(myepoll_event_t *pev);
void pr_cpu_time(void);
int shared_memory_get();
int shared_memory_init();
int shared_memory_rm();
int my_lock_init();
int my_lock_wait();
int my_lock_try_wait();
int my_lock_release();
int register_sig_handler();
void sig_handler(int num);
int register_int_handler();
void sig_int_handler(int num);

#define MAXEVENTS 500
#define PORT 	1234
#define LISTENNUM  10

#define PROGRESSNUM 15

int g_epollfd;
static long *connsum;
myepoll_event_t g_events[MAXEVENTS];
progress_pool_t myprogress_pool;
struct epoll_event myevent[MAXEVENTS];
int listenst = 0; //the index of fd of listen array of g_events

/**
 * create chilid progress pool to handle new connection with epoll
 * @param  argc 
 * @param  argv 
 * @return      0
 */
int main(int argc, char const *argv[])
{
	if (srv_init() < 0)
		err_sys("srv_init err");

	if (register_sig_handler() < 0)
		err_sys("register_sig_handler err");

	if (shared_memory_init() < 0)
		err_sys("shared_memory_init err");

	if (my_lock_init() < 0)
		err_sys("my_lock_init err");
	
	myprogress_pool.current_progress = PROGRESSNUM;

	int pid = 0;
	for (int i = 0; i < myprogress_pool.current_progress; i++)
	{
		pid = fork();
		if (pid < 0)
			err_sys("fork err");

		else if (pid == 0)
		{
			myprogress_pool.index = i;
			progress_handle();
		}

	}
	
	if (register_int_handler() < 0)
		err_sys("register_int_handler err");

	while(1)
		pause();

	return 0;
}

/**
 * child progress handle 
 */
void progress_handle()
{
	myepoll_event_t *pev;
	int eventnum = 0;
	int checkpos = 0;
	time_t now = 0;
	long duration = 0;
	bool flag_mutex = false;
	epoll_init();
	while(1)
	{	
		now = time(NULL);
		for (int i = 1; i < 100; i++, checkpos++)
		{
			if (checkpos == MAXEVENTS) 
				checkpos =1;
			if (g_events[checkpos].status == 0)
				continue;
			duration = now - g_events[checkpos].last_alive;

			if (duration >60)
			{
				pev = &g_events[checkpos];
				if (del_event(g_epollfd, pev) < 0)
					err_ret("del_event err");

				close(g_events[checkpos].fd);
				fprintf(stdout, "fd of connection : %d hasn't being active for a long time\n", g_events[checkpos].fd);
			}
		}

		if (my_lock_try_wait() == 0)
		{
			//fprintf(stdout, "I got mutex , pid is %d\n", getpid());
			add_listen();
			flag_mutex = true;
		}
		else 
		{
			del_listen();
		}
		//fourth argument is milliseconds
		eventnum = epoll_wait(g_epollfd, &myevent[0], MAXEVENTS, 1000);
		if (eventnum < 0)
			err_sys("epoll_wait err");

		if (flag_mutex)
		{
			my_lock_release();
			flag_mutex = false;
		}
		
		for (int i = 0; i < eventnum; i++)
		{
			pev = (myepoll_event_t *) myevent[i].data.ptr;
			pev->callback(pev);	
		}
	}
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
	pev->status = 0;
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
	pev->status = 1;
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
	pev->status = 1;
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
	pev->status = 0;
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
	
	myprogress_pool.listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (myprogress_pool.listenfd < 0)
		return -1;

	int res;
	int optval = 1;
	res = setsockopt(myprogress_pool.listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (res < 0)
		return -1;

	int flags;
	if ((flags = fcntl(myprogress_pool.listenfd, F_GETFL, 0)) < 0)
		return -1;

	flags |= O_NONBLOCK;
	if (fcntl(myprogress_pool.listenfd, F_SETFL, flags) < 0)
		return -1;

	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(PORT);
	ser_addr.sin_addr.s_addr = INADDR_ANY;

	res = bind(myprogress_pool.listenfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if (res < 0)
		return -1;

	if (listen(myprogress_pool.listenfd, LISTENNUM) < 0)
		return -1;

	return 0;
}

/**
 * epoll initialization 
 * @return 0: success; -1: err
 */
int epoll_init()
{
	int res = 0;
	g_epollfd =  epoll_create(MAXEVENTS);
	if (g_epollfd < 0)
	{
		err_sys("epoll create err");
	}
		
	for (int i = 0; i < MAXEVENTS; i++)
	{
		g_events[i].fd = -1;
		g_events[i].status = 0;
		memset(g_events[i].send_buffer, 0, sizeof(g_events[i].send_buffer));
		memset(g_events[i].recv_buffer, 0, sizeof(g_events[i].send_buffer));
		g_events[i].callback = NULL;
		g_events[i].last_alive = 0;
	}

	res = set_event(g_epollfd, myprogress_pool.listenfd, myaccept, &g_events[0]);
	if (res < 0)
		err_ret("set_event");

	res = add_event(g_epollfd, EPOLLIN, &g_events[0]);
	if (res < 0)
		err_ret("add_event");

	return res;
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
	for (i = 1; i < length; i++)
	{
		if (pev[i].status == 0)
			return i;
	}

	return -1;
}

/**
 * add listen fd into epoll
 * @return 
 */
int add_listen()
{
	//set_event(g_epollfd, listenfd, myaccept, &g_events[0]);
	return add_event(g_epollfd, EPOLLIN, &g_events[0]);
}

int del_listen()
{
	return del_event(g_epollfd, &g_events[0]);
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
	{
		err_ret("pid is %d, accept err", getpid());
		return -1;
	}

		
	//set new fd nonblocking
	int flags;
	if ((flags = fcntl(newfd, F_GETFL, 0)) < 0)
		return -1;
	flags |= O_NONBLOCK;
	if (fcntl(newfd, F_SETFL, flags) < 0)
		return -1;

	int res;
	res = set_event(g_epollfd, newfd, recv_data, &g_events[i_idle]);
	if (res < 0)
		err_ret("set_event");
	res = add_event(g_epollfd, EPOLLIN, &g_events[i_idle]);
	if (res < 0)
		err_ret("add_event");
	
	return res;
}

/**
 * receive data from client, a EPOLLIN event function
 * @param  pev a poiner to myepoll_event_t
 * @return     0: success; -1: err
 */
int recv_data(myepoll_event_t * pev)
{
	int len = 0;
	int res = 0;
	len = read(pev->fd, pev->recv_buffer, BUFFERMAX);
	if (len < 0)
	{

		if (errno == EAGAIN)
		{
			err_ret("pid is %d, fd is %d, read err, EAGAIN", getpid(), pev->fd);
			return -1;
		}

		err_ret("pid is %d, fd is %d , read err", getpid(), pev->fd);	
		connsum[myprogress_pool.index]++;
		del_event(g_epollfd, pev);
		close(pev->fd);
		return -1;
	}
	if (len == 0)
	{	

		connsum[myprogress_pool.index]++;
		if (del_event(g_epollfd, pev) < 0)
			err_ret("del_event is err");

		close(pev->fd);

	}
	else 
	{
		pev->recv_buffer[len] = '\0';
		fprintf(stdout, "recv buffer from client is %s\n", pev->recv_buffer);
		
		//very simple business process
		strcpy(pev->send_buffer, pev->recv_buffer);
		memset(pev->recv_buffer, 0, sizeof(pev->recv_buffer));

		res = mod_event(g_epollfd, EPOLLOUT, send_data, pev);
		if (res < 0)
			err_ret("mod_event err");
	}

	return res;
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

	shmid = shmget(key, PROGRESSNUM * sizeof(long), 0644 | IPC_CREAT);
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

	connsum = (long *)shmat(shmid, NULL, 0);
	if (connsum == (void *) -1)
		return -1;

	for(int i = 0; i < PROGRESSNUM; i++)
	{
		connsum[i] = 0;
		
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

/**
 * lock initialization, using pthread mutex 
 * @return 0: sucess; -1: err
 */
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

	myprogress_pool.mptr = (pthread_mutex_t *)shmat(shmid, NULL, 0);
	if (myprogress_pool.mptr == (void *) -1)
		return -1;

	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	int res;
	res = pthread_mutex_init(myprogress_pool.mptr, &mattr);
	if (res < 0)
		return -1;

	return 0;

}

/**
 * lock the mutex
 * @return 
 */
int my_lock_wait()
{
	return pthread_mutex_lock(myprogress_pool.mptr);
}

/**
 * try to get the lock , nonblocking
 * @return [
 */
int my_lock_try_wait()
{
	return pthread_mutex_trylock(myprogress_pool.mptr);
}

/**
 * realease lock
 * @return 
 */
int my_lock_release()
{
	return pthread_mutex_unlock(myprogress_pool.mptr);
}
/**
 * register signal handler function
 * @return 0: success; -1: err
 */
int register_sig_handler()
{
	struct sigaction myact;
	myact.sa_handler = sig_handler;

	if (sigaction(SIGPIPE, &myact, NULL) < 0)
	{
		err_ret("sigaction is err");
		return -1;
	}

	if(sigaction(SIGCHLD, &myact, NULL) < 0)
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
void sig_handler(int num)
{

	if (num == SIGPIPE)
	{
		fprintf(stdout, "pid is %d, get the signal SIGPIPS.\n", getpid());
		return ;
		//exit(0);
	}
	//if (num == )

}

int register_int_handler()
{
	
	struct sigaction myact;
	myact.sa_handler = sig_int_handler;
	if (sigaction(SIGINT, &myact, NULL) < 0)
	{
		err_ret("sigaction err");
		return -1;
	}
	return 0;
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
		for(int i = 0; i < PROGRESSNUM; i++)
		{
			sum += connsum[i];
			
			fprintf(stdout, " %ld ", connsum[i]);
		}

		fprintf(stdout, "\ntotal connection is %ld.\n", sum);

		exit(0);
	}

}


