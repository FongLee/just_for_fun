#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

//input: ^C
//output: sigwaitinfo return 2(Success), signum is 2
//
//input: ^\
//output: 
//--Thread 0 Received Signal  3(Quit)!
//Quit (core dumped)

//this is a test of sigwaitinfo function, this function can 
//interrupted by other signals
void sighandler(int signum)
{   //
    printf("--Thread %x Received Signal %2d(%s)!\n",
           (unsigned int)pthread_self(), signum, strsignal(signum));
}

int main(void)
{
	sigset_t blockset;
	sigemptyset(&blockset);
	sigaddset(&blockset, SIGINT);
	sigprocmask(SIG_BLOCK, &blockset, NULL);

	signal(SIGQUIT, sighandler);

	siginfo_t info;
	int res;
	res = sigwaitinfo(&blockset, &info);

	fprintf(stdout, "sigwaitinfo return %d(%s), signum is %d\n", res, strerror(errno), info.si_signo);

}