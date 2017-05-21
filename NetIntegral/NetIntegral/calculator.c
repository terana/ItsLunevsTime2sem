#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "sys_info.h"
#include <unistd.h>
#include <netinet/tcp.h>

#define UDP_PORT 51000
#define TCP_PORT 51000
#define MSGSIZE 20
#define FUNC(x) x*x

typedef struct
{
	long double a;
	long double b;
} limits_t;

typedef struct
{
	limits_t limits;
	long     parts;
} server_arg_t;

typedef struct
{
	int       socket;
	pthread_t *calculating_threads;
	long      numOfthreads;
} checker_arg_t;

typedef struct
{
	limits_t    limits;
	long        numberOfParts;
	long double result;
	pthread_t   thread;
} thread_arg_t;

typedef struct
{
	long numOfThreads;
	long numOfParts;
} settings_t;

void *calculate(void *param)
{
	thread_arg_t *arg   = (thread_arg_t *) param;
	long double  result = 0;
	long double  a      = arg->limits.a;
	long double  b      = arg->limits.b;
	long         parts  = arg->numberOfParts;
	long double  step   = (b - a) / parts;
	b = a + step;
	while (b < arg->limits.b)
	{
		long double c = (a + b) / 2;
		result += (b - a) / 6 * (FUNC(a) + 4 * FUNC(c) + FUNC(b));
		a             = b;
		b += step;
	}

	arg->result = result;
	return NULL;
}

void *checkClient(void *param)
{
	printf("Checking client...\n");
	checker_arg_t *arg        = (checker_arg_t *) param;
	char          buf[10];

	ssize_t res = recv(arg->socket, buf, 10, 0);
	if (res < 0)
	{
		perror("Error in checking thread");
		exit(EXIT_FAILURE);
	}
	printf("Program will  exit due to the closed connection\n");
	exit(EXIT_FAILURE);
}

int createAndBindBroadcastScoket()
{
	printf("Creating broadcast listening socket\n");

	int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0)
	{
		perror("Error creating broadcast listening socket:");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in recv_addr;
	bzero(&recv_addr, sizeof(recv_addr));
	recv_addr.sin_family      = AF_INET;
	recv_addr.sin_port        = htons(UDP_PORT);
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Binding broadcast listening socket\n");

	if (bind(socket_fd, (struct sockaddr *) &recv_addr, sizeof(recv_addr)) < 0)
	{
		perror("Error binding udp socket");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	return socket_fd;
}

void listenAndAnswerBroadcast(int socket_fd, long threads)
{
	char recieved_msg[MSGSIZE];
	bzero(recieved_msg, MSGSIZE);
	fcntl(socket_fd, F_SETFL, O_NONBLOCK);

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(socket_fd, &readset);

//	struct timeval timeout;
//	timeout.tv_sec  = 180;
//	timeout.tv_usec = 0;

	struct sockaddr_in sendr_addr;;
	bzero(&sendr_addr, sizeof(sendr_addr));
	socklen_t sendr_addr_len = sizeof(struct sockaddr_in);

	printf("Waiting for distributor for 5 min\n");

	int sockst = select(socket_fd + 1, &readset, NULL, &readset, NULL);
	if (sockst < 0)
	{
		perror("Error on select in broadcast socket");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
	if (sockst == 0)
	{
		printf("Programm will exit due to the timeout for waiting distributor is expired\n");
		exit(EXIT_FAILURE);
	}

	printf("Receiving broadcast message\n");

	ssize_t n = recvfrom(socket_fd, (void *) recieved_msg, MSGSIZE, MSG_TRUNC, (struct sockaddr *) &sendr_addr, &sendr_addr_len);
	if (n < 0)
	{
		perror("Error reading from broadcast socket");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	printf("Received broadcast message\n");

	if (strcmp(recieved_msg, "Hello") == 0)
	{
		printf("Sending answer\n");

		if (sendto(socket_fd, &(threads), sizeof(threads), 0, (struct sockaddr *) &sendr_addr, sendr_addr_len) < 0)
		{
			perror("Error sending answer");
			close(socket_fd);
			exit(EXIT_FAILURE);
		}
		return;
	}
	else
	{
		printf("Received strange message.\n");
		exit(EXIT_FAILURE);
	}
}

int setTCPOptions(int socket_fd)
{
	printf("Setting tcp socket options\n");

	int reuseaddr = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) < 0)
	{
		perror("Error enabling reusing socket addres");
		return 1;
	}
	int keepalive = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int)) != 0)
	{
		perror("Error setting keepalive");
		return 1;
	}
	int keepcnt = 1;
	if (setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(int)) != 0)
	{
		perror("Error setting keepcnt");
		return 1;
	}

	int keepidle = 1;
	if (setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int)) != 0)
	{
		perror("Error setting keepcnt");
		return 1;
	}

	int keepintvl = 1;
	if (setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(int)) != 0)
	{
		perror("Error setting keepcnt");
		return 1;
	}
	return 0;
}

int createAndBindTCPSocket()
{
	printf("Creating tcp socket\n");

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		perror("Error creating tcp socket");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (setTCPOptions(listen_fd) != 0)
	{
		close(listen_fd);
		exit(EXIT_FAILURE);
	}

	printf("Binding tcp socket\n");

	if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		perror("Error binding tcp socket");
		close(listen_fd);
		exit(EXIT_FAILURE);
	}
	return listen_fd;
}

int parseArgs(int argc, char **argv, settings_t *settings)
{
	long threads;
	if (argc > 2)
	{
		printf("Usage: %s [number of threads]\n", argv[0]);
		return 1;
	}
	if (argc == 2)
	{
		char **endptr = NULL;
		errno = 0;
		threads = strtol(argv[1], endptr, 10);
		if (errno != 0)
		{
			return -1;
		}
		if (threads < 1)
		{
			printf("Incorrect num of threads\n");
			return -1;
		}
		printf("Set number of threads to %ld\n", threads);
		settings->numOfThreads = threads;
		return 0;
	}
	printf("Set number of threads to 2\n");
	settings->numOfThreads = 2;
	return 0;
}

int main(int argc, char **argv)
{
	settings_t settings;
	if (parseArgs(argc, argv, &settings) != 0)
	{
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		int udp_socket_fd = createAndBindBroadcastScoket();
		listenAndAnswerBroadcast(udp_socket_fd, settings.numOfThreads);

		int tcp_socket_fd = createAndBindTCPSocket();

		printf("Listening on tcp socket\n");

		if (listen(tcp_socket_fd, 128) < 0)
		{
			perror("Error listening socket");
			close(tcp_socket_fd);
			close(udp_socket_fd);
			exit(EXIT_FAILURE);
		}

		printf("Accepting tcp socket\n");

		struct timeval timeout;
		timeout.tv_sec                     = 60;
		timeout.tv_usec                    = 0;
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(tcp_socket_fd, &readset);
		int    res                         = select(tcp_socket_fd + 1, &readset, &readset, &readset, &timeout);
		if (res == 0)
		{
			printf("Time for accepting tcp socket is over\n");
			close(tcp_socket_fd);
			close(udp_socket_fd);
			continue;
		}
		if (res < 0)
		{
			perror("Error accepting tcp socket");
			close(tcp_socket_fd);
			close(udp_socket_fd);
			exit(EXIT_FAILURE);
		}

		ssize_t            n;
		struct sockaddr_in client_addr;
		socklen_t          client_addr_len = sizeof(client_addr);
		int                attached_fd     = accept(tcp_socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (attached_fd < 0)
		{
			perror("Error accepting client");
			close(tcp_socket_fd);
			exit(EXIT_FAILURE);
		}

		if (setTCPOptions(attached_fd) != 0){
			close(tcp_socket_fd);
			close(udp_socket_fd);
			close(attached_fd);
			exit(EXIT_FAILURE);
		}

		printf("Accepted tcp socket, receiving message\n");

		server_arg_t arg;
		n = recv(attached_fd, &arg, sizeof(arg), 0);
		if (n != sizeof(arg) || arg.parts < 1)
		{
			printf("Invalid argument from %s on port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			close(tcp_socket_fd);
			close(attached_fd);
			exit(EXIT_FAILURE);
		}

		printf("New task from %s on port %d\nlimits: %Lf %Lf\nparts: %ld\n", inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port), arg.limits.a, arg.limits.b, arg.parts);
		settings.numOfParts = arg.parts;

		long        numbrerOfPartsPerTread = settings.numOfParts / settings.numOfThreads;
		long double stepLimits             = (arg.limits.b - arg.limits.a) / settings.numOfThreads;
		long double a                      = arg.limits.a;
		long double b                      = a + stepLimits;

		//This doesn't work on MacOS. Just not to disturb
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(0, &cpuset);
		if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
		{
			perror("Can't set affinity for main thread");
			exit(1);
		}
		long cpu  = 1; // 0 cpu is for main thread
		long phys = 0;

		phys_info info;
		if (getPhysicalProcessorsInfo(&info) != 0)
		{
			perror("Can't get sys info");
			exit(1);
		}
		long max_core_id = info.cores_per_phys[0];

		thread_arg_t *args            = malloc(settings.numOfThreads * sizeof(thread_arg_t));
		int          i;
		int          usingRealThreads = 1;
		int          usingAllthreads  = 0;
		for (i = 0; i < settings.numOfThreads; i++)
		{
			(args[i]).limits.a      = a;
			(args[i]).limits.b      = b;
			(args[i]).numberOfParts = numbrerOfPartsPerTread;
			a = b;
			b += stepLimits;

			if (pthread_create(&(args[i].thread), NULL, calculate, &(args[i])) != 0)
			{
				perror("Can't create thread");
				close(tcp_socket_fd);
				close(attached_fd);
				free(args);
				exit(EXIT_FAILURE);
			}

			//This doesn't work on MacOS. Just not to disturb
			CPU_ZERO(&cpuset);
			CPU_SET(cpu, &cpuset);
			errno = 0;
			if (pthread_setaffinity_np(args[i].thread, sizeof(cpu_set_t), &cpuset) != 0)
			{
				perror("Can't set affinity");
				exit(1);
			}
			printf("set thread %d to cpu %ld\n", i, cpu);
			cpu++;
			if (usingAllthreads)
			{
				if (cpu >= info.cores * 2)
				{cpu = 0;}
				continue;
			}
			if (usingRealThreads)
			{
				if (cpu >= max_core_id)
				{
					cpu         = info.cores_per_phys[phys] * 2;
					phys++;
					if (phys >= info.phys_cpus)
					{
						usingRealThreads = 0;
						phys             = 0;
						cpu              = info.cores_per_phys[phys];
						max_core_id      = info.cores_per_phys[phys] * 2;
					}
					max_core_id = cpu + info.cores_per_phys[phys];
				}
				continue;
			}
			else
			{
				if (cpu >= max_core_id)
				{
					phys++;
					if (phys >= info.phys_cpus)
					{
						usingAllthreads = 1;
						phys            = 0;
						cpu             = 0;
					}
					else
					{
						cpu = max_core_id + info.cores_per_phys[phys];
					}
					max_core_id = cpu + info.cores_per_phys[phys];
				}
			}
		}

		checker_arg_t checker_arg;
		checker_arg.socket = attached_fd;
		pthread_t checker_thread;
		if (pthread_create(&checker_thread, NULL, checkClient, &checker_arg) != 0)
		{
			perror("Can't create client checker thread");
			exit(EXIT_FAILURE);
		}
		int smthWrong = 0;
		int *reasonOfExit;
		for (i = 0; i < settings.numOfThreads; i++)
		{
			if (pthread_join(args[i].thread, (void *) &reasonOfExit) == 0)
			{
				if (reasonOfExit == PTHREAD_CANCELED)
				{
					smthWrong = 1;
					break;
				}
			}
		}
		if (smthWrong == 1)
		{
			printf("Error: something wrong\n");
			close(tcp_socket_fd);
			close(attached_fd);
			free(args);
			exit(EXIT_FAILURE);
		}

		long double result = 0;

		for (i = 0; i < settings.numOfThreads; i++)
		{
			pthread_join(args[i].thread, NULL);
			result += args[i].result;
		}
		printf("result: %Lf\n", result);

		if (send(attached_fd, &result, sizeof(long double), 0) < 0)
		{
			perror("Error sending result");
		}
		printf("Closing connection\n");
		pthread_cancel(checker_thread);
		close(tcp_socket_fd);
		close(udp_socket_fd);
		close(attached_fd);
		free(args);
	}
	return 0;
}
