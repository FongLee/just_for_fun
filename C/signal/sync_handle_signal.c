#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

void *handle_sig_thread((void *)arg)
{
	int res;
	res = pthread_detach(pthread_self());
	if (res != 0)
	{
		return ;
	}

	siginfo_t info;
	while(1)
	{
		res = sigwaitinfo(*(sigset_t *)arg, &info);
		if( res > 0)
		{
			fprintf(stdout, "get signal num: %d\n", info.si_signo);
		}
	}

	return ;
}

void *work_thread((void *)arg)
{
	while(1)
	{
		sleep(1);
		fprintf(stdout, "thread ID %d is runnning\n", pthread_self());
	}

}

int main(void)
{
	sigset_t myset;
	int res;
	sigemptyset(&myset);
	sigaddset(&myset, SIGUSR1);
	sigaddset(&myset, SIGUSR2);
	sigaddset(&myset, SIGRTMAX);
	sigaddset(&myset, SIGRTMIN);

	res = sigprocmask(SIG_BLOCK, &myset, NULL);
	if (res < 0)
	{
		return -1;
	}

	pthread_t handle_sig_id;
	res = pthread_create(&handle_sig_id, NULL, handle_sig_thread, &myset);

	pthread_t work_thread_id;
	res = pthread_create(&work_thread_id, NULL, work_thread, &myset);
	 
	pid_t mypid;
	mypid = getpid();
	kill(mypid, SIGUSR1);
	kill(mypid, SIGUSR1);
	kill(mypid, SIGUSR2);
	kill(mypid, SIGUSR2);
	kill(mypid, SIGRTMAX);
	kill(mypid, SIGRTMAX);
	kill(mypid, SIGRTMIN);
	kill(mypid, SIGRTMIN);
	kill(mypid, SIGUSR1);
	kill(mypid, SIGUSR1);
	while(1)
	{
		sleep(1);
	}


}