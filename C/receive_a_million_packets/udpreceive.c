#define _GNU_SOURCE // for recvmmsg


#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"


#define MTU_SIZE (2048-64*2)
#define MAX_MSG 512
#define IO_LEN 512 

struct state {
	int fd;
	volatile uint64_t bps;
	volatile uint64_t pps;
	//struct mmsghdr messages[MAX_MSG];
	struct msghdr messages;
	char buffers[IO_LEN][MTU_SIZE];
	struct iovec iovecs[IO_LEN];
	// char buffers[MAX_MSG][MTU_SIZE];
	// struct iovec iovecs[MAX_MSG];
} __attribute__ ((aligned (64)));

struct state *state_init(struct state *s) {
	int i;
	// for (i = 0; i < MAX_MSG; i++) {
	// 	char *buf = &s->buffers[i][0];
	// 	struct iovec *iovec = &s->iovecs[i];
	// 	struct mmsghdr *msg = &s->messages[i];

	// 	msg->msg_hdr.msg_iov = iovec;
	// 	msg->msg_hdr.msg_iovlen = 1;

	// 	iovec->iov_base = buf;
	// 	iovec->iov_len = MTU_SIZE;
	// }
	char *buf;
	struct msghdr *msg = &s->messages;
	struct iovec *iov = &s->iovecs[0];

	msg->msg_iov = iov;
	msg->msg_iovlen = IO_LEN;

	for (i = 0; i < IO_LEN ; i++)
	{
		buf = &s->buffers[i][0];
		iov = &s->iovecs[i];

		iov->iov_base = buf;
		iov->iov_len = MTU_SIZE;
	}

	return s;
}

static void thread_loop(void *userdata)
{
	struct state *state = userdata;

	while (1) {
		/* Blocking recv. */
		//int r = recvmmsg(state->fd, &state->messages[0], MAX_MSG, MSG_WAITFORONE, NULL);
		int r = recvmsg(state->fd, &state->messages, 0);
		if (r <= 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
				continue;
			}
			PFATAL("recvmmsg()");
		}

		int i, bytes = 0;

		// for (i = 0; i < r; i++) {
		// 	struct mmsghdr *msg = &state->messages[i];
		// 	 char *buf = msg->msg_hdr.msg_iov->iov_base; 
		// 	int len = msg->msg_len;
		// 	msg->msg_hdr.msg_flags = 0;
		// 	msg->msg_len = 0;
		// 	bytes += len;
		// }

		// __atomic_fetch_add(&state->pps, r, 0);
		// __atomic_fetch_add(&state->bps, bytes, 0);
		//r is the numbers of message
		//bytes is the numbers of bytes 
		//the two function is the atomic operation 
		//it is more efficient than lock 
		// __sync_fetch_and_add (&state->pps, r, 0);
		// __sync_fetch_and_add (&state->bps, bytes, 0);
		state->pps += 1;
		state->bps += r;
	
	}
}

int main(int argc, const char *argv[])
{
	const char *listen_addr_str = "0.0.0.0:4321";
	int recv_buf_size = 4*1024;
	int thread_num = 1;
	int reuseport = 0;

	switch (argc) {
	case 4:
		reuseport = atoi(argv[3]);
	case 3:
		thread_num = atoi(argv[2]);
	case 2:
		listen_addr_str = argv[1];
	case 1:
		break;
	default:
		FATAL("Usage: %s [listen ip:port] [fork cnt] [reuseport]", argv[0]);
	}


	struct net_addr listen_addr;
	parse_addr(&listen_addr, listen_addr_str);

	int main_fd = -1;
	if (reuseport == 0) {
		fprintf(stderr, "[*] Starting udpreceiver on %s, recv buffer %iKiB\n",
			addr_to_str(&listen_addr), recv_buf_size / 1024);

		main_fd = net_bind_udp(&listen_addr, 0);
		net_set_buffer_size(main_fd, recv_buf_size, 0);
	}

	struct state *array_of_states = calloc(thread_num, sizeof(struct state));

	int t;
	for (t = 0; t < thread_num; t++) {
		struct state *state = &array_of_states[t];
		state_init(state);
		if (reuseport == 0) {
			state->fd = main_fd;
		} else {
			fprintf(stderr, "[*] Starting udpreceiver on %s, recv buffer %iKiB\n",
				addr_to_str(&listen_addr), recv_buf_size / 1024);

			int fd = net_bind_udp(&listen_addr, 1);
			net_set_buffer_size(fd, recv_buf_size, 0);
			state->fd = fd;
		}
		thread_spawn(thread_loop, state);
	}

	uint64_t last_pps = 0;
	uint64_t last_bps = 0;

	while (1) {
		struct timeval timeout =
			NSEC_TIMEVAL(MSEC_NSEC(1000UL));
		while (1) {
			int r = select(0, NULL, NULL, NULL, &timeout);
			if (r != 0) {
				continue;
			}
			if (TIMEVAL_NSEC(&timeout) == 0) {
				break;
			}
		}

		uint64_t now_pps = 0, now_bps = 0;
		for (t = 0; t < thread_num; t++) {
			struct state *state = &array_of_states[t];
			//write 0 to state->pps
			// now_pps += __atomic_load_n(&state->pps, 0);
			// now_bps += __atomic_load_n(&state->bps, 0);
			// return state->pps and set state->pps = 0  
			//now_pps += __sync_lock_test_and_set (&state->pps, 0);
			//now_bps += __sync_lock_test_and_set(&state->bps, 0);

			now_pps += state->pps;
			now_bps += state->bps;
			 //state->pps = 0;
			//state->bps = 0;
		}

		double delta_pps = now_pps - last_pps;
		double delta_bps = now_bps - last_bps;
		last_pps = now_pps;
		last_bps = now_bps;

		printf("%7.3fM pps %7.3fMiB / %7.3fMb\n",
		       delta_pps / 1000.0 / 1000.0,
		       delta_bps / 1024.0 / 1024.0,
		       delta_bps * 8.0 / 1000.0 / 1000.0 );
	}

	return 0;
}