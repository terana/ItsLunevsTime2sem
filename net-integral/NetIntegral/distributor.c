#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <netinet/tcp.h>

#define UDP_PORT 51000
#define TCP_PORT 51000
#define MSGSIZE 20

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
	struct sockaddr_in addr;
	long               threads;
	server_arg_t       arg;
} server_t;

typedef struct
{
	server_t    *server;
	long double result;
	pthread_t   thread;
} thread_arg_t;

typedef struct
{
	limits_t limits;
	long     numOfParts;
	long     numOfServers;
} settings_t;

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

void *calculate(void *param)
{
	printf("Creating tcp socket...\n");

	thread_arg_t *arg      = (thread_arg_t *) param;
	int          socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		perror("Error creating tcp socket");
		exit(EXIT_FAILURE);
	}

	if (setTCPOptions(socket_fd) != 0){
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	//struct sockaddr_in tcp_addr;
//	tcp_addr.sin_family      = AF_INET;
//	tcp_addr.sin_addr.s_addr = INADDR_ANY;
//	tcp_addr.sin_port        = 0;

//	printf("Binding new tcp socket...\n");
//
//	if (bind(socket_fd, (struct sockaddr *) &tcp_addr, sizeof(tcp_addr)) < 0)
//	{
//		perror("Error binding tcp socket");
//		exit(EXIT_FAILURE);
//	}

	printf("Connecting to server %s on port %d...\n", inet_ntoa((arg->server->addr).sin_addr), ntohs((arg->server->addr).sin_port));

	socklen_t server_addr_len = sizeof(arg->server->addr);
	if (connect(socket_fd, (struct sockaddr *) &(arg->server->addr), server_addr_len) < 0)
	{
		perror("Error connecting to server");
		exit(EXIT_FAILURE);
	}

	printf("Sending tcp message to server %s on port %d...\n", inet_ntoa((arg->server->addr).sin_addr), ntohs((arg->server->addr).sin_port));

	if (send(socket_fd, &(arg->server->arg), sizeof(arg->server->arg), 0) < 0)
	{
		perror("Error sending data to server");
		exit(EXIT_FAILURE);
	}

	printf("Waiting tcp message from server %s on port %d...\n", inet_ntoa((arg->server->addr).sin_addr), ntohs((arg->server->addr).sin_port));

	ssize_t n = recv(socket_fd, &arg->result, sizeof(long double), 0);
	if (n <= 0)
	{
		printf("Server %s on port %d doesn't respond correctly", inet_ntoa((arg->server->addr).sin_addr), ntohs((arg->server->addr).sin_port));
		exit(EXIT_FAILURE);
	}

	printf("Got result from server %s on port %d\n%Lf\n", inet_ntoa((arg->server->addr).sin_addr), ntohs((arg->server->addr).sin_port), arg->result);
	close(socket_fd);
	return NULL;
}

void printHelpMessage(const char *programName)
{
	printf("%s", "This programm calculates the integral of x^2 from a to b using Simpson method.\n");
	printf("Usage: %s [options]\n", programName);
	printf("%s", "Options:\n");
	printf("%s", "-lim_a <value>        Set lower limit\n");
	printf("%s", "-lim_b <value>        Set higher limit\n");
	printf("%s", "-n_parts <value>      Set number of parts of the interval\n");
	printf("%s", "-n_calcs <value>      Set number of calculators\n");
	printf("%s", "If some options are not set the default value is used.\n");
	printf("%s", "The default values are: a = 0, b = 2, number of parts = 20000000, 1 calculator\n");
}

void parseInputArgs(int argc, char **argv, settings_t *settings)
{
	if (argc < 2)
	{
		printHelpMessage(argv[0]);
		return;
	}
	if (argc == 2)
	{
		char **endptr = NULL;
		errno         = 0;
		settings->numOfParts = strtol(argv[1], endptr, 10);
		if (errno != 0)
		{
			printHelpMessage(argv[0]);
			return;
		}
		return;
	}
	int i;
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (!strcmp(argv[i], "-lim_a"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->limits.a = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					printHelpMessage(argv[0]);
					return;
				}
			}
			if (!strcmp(argv[i], "-lim_b"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->limits.b = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					printHelpMessage(argv[0]);
					return;
				}
			}
			if (!strcmp(argv[i], "-n_parts"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->numOfParts = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					printHelpMessage(argv[0]);
					return;
				}
			}
			if (!strcmp(argv[i], "-n_calcs"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->numOfServers = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					printHelpMessage(argv[0]);
					return;
				}
			}
		}
		else
		{
			printHelpMessage(argv[0]);
			return;
		}
	}
}

int createAndBindUDPSocket()
{
	printf("Creating udp socket...\n");

	int udp_socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (udp_socket_fd < 0)
	{
		perror("Error creating broadcast socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in udp_addr;
	bzero(&udp_addr, sizeof(udp_addr));
	udp_addr.sin_family      = AF_INET;
	udp_addr.sin_port        = htons(0);
	udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Binding udp socket...\n");

	if (bind(udp_socket_fd, (struct sockaddr *) &udp_addr, sizeof(udp_addr)) < 0)
	{
		perror("Error binding udp socket");
		close(udp_socket_fd);
		exit(EXIT_FAILURE);
	}
	return udp_socket_fd;
}

void *broadcast(void *param)
{
	int                udp_socket_fd = *((int *) param);
	struct sockaddr_in brcast_addr;
	bzero(&brcast_addr, sizeof(brcast_addr));
	brcast_addr.sin_family      = AF_INET;
	brcast_addr.sin_port        = htons(UDP_PORT);
	brcast_addr.sin_addr.s_addr = htonl(0xffffffff);

	int opt_val = 1;
	if (setsockopt(udp_socket_fd, SOL_SOCKET, SO_BROADCAST, (const void *) &opt_val, sizeof(opt_val)) < 0)
	{
		perror("Error setting udp socket options");
		close(udp_socket_fd);
		exit(EXIT_FAILURE);
	}

	char msg[MSGSIZE];
	bzero(msg, MSGSIZE);
	strcpy(msg, "Hello");

	printf("Broadcasting...\n");

	while (1)
	{
		if (sendto(udp_socket_fd, msg, MSGSIZE, 0, (struct sockaddr *) &brcast_addr, sizeof(brcast_addr)) < 0)
		{
			perror("Error broadcasting msg");
			close(udp_socket_fd);
			exit(EXIT_FAILURE);
		}
	}
	return NULL;
}

long listenForAnswers(int udp_socket_fd, server_t *calculators, long numOfServers)
{
	printf("Listening for broadcast answers...\n");

	//fcntl(udp_socket_fd, F_SETFL, O_NONBLOCK);

	fd_set read_set;

	struct timeval timeout;
	timeout.tv_sec  = 60;
	timeout.tv_usec = 0;

	socklen_t serv_addr_len = sizeof(struct sockaddr_in);

	long servers       = 0;
	long threads       = 0;
	long total_threads = 0;
	int  sockst        = 1;

	while (servers < numOfServers)
	{
		FD_ZERO(&read_set);
		FD_SET(udp_socket_fd, &read_set);

		sockst = select(udp_socket_fd + 1, &read_set, NULL, &read_set, &timeout);
		if (sockst == 0)
		{
			printf("Can't wait for calculators any more\n");
			exit(EXIT_FAILURE);
		}
		if (sockst < 0)
		{
			perror("Error in select");
			exit(EXIT_FAILURE);
		}

		printf("Recieving answer from udp socket\n");

		ssize_t n = recvfrom(udp_socket_fd, &threads, sizeof(threads), MSG_TRUNC, (struct sockaddr *) &(calculators[servers].addr), &serv_addr_len);
		if (n <= 0 || threads < 1)
		{
			perror("Error receiving message from udp socket");
			exit(EXIT_FAILURE);
		}

		printf("Recieved %ld threads from server %s port %d", threads, inet_ntoa((calculators[servers].addr).sin_addr), ntohs((calculators[servers].addr).sin_port));
		calculators[servers].threads = threads;
		total_threads += threads;
		servers++;
	}

	printf("All %ld servers are found\nTotal threads: %ld", servers, total_threads);
	return total_threads;
}

int main(int argc, char **argv)
{
	settings_t settings;
	settings.numOfServers = 1;
	settings.limits.a     = 0;
	settings.limits.b     = 10;
	settings.numOfParts   = 5000000000;
	parseInputArgs(argc, argv, &settings);

	printf("Settings:\nlimits = [%Lf ; %Lf]\nmax servers = %ld\nparts = %ld\n", settings.limits.a, settings.limits.b, settings.numOfServers, settings.numOfParts);

	int udp_socket_fd = createAndBindUDPSocket();

	pthread_t broadcast_thread;
	if (pthread_create(&broadcast_thread, NULL, broadcast, &udp_socket_fd) != 0)
	{
		perror("Can't create broadcast thread");
		exit(1);
	}

	server_t *calculators  = malloc(settings.numOfServers * sizeof(server_t));
	long     total_threads = listenForAnswers(udp_socket_fd, calculators, settings.numOfServers);
	pthread_cancel(broadcast_thread);

	thread_arg_t *args                  = malloc(sizeof(thread_arg_t) * settings.numOfServers);
	long         numberOfPartsPerThread = settings.numOfParts / total_threads;
	long double  stepLimits             = (settings.limits.b - settings.limits.a) / total_threads;
	long double  a                      = settings.limits.a;
	long double  b                      = a;
	int          i;
	for (i = 0; i < settings.numOfServers; i++)
	{
		printf("Server answer from %s on port %d\n", inet_ntoa(calculators[i].addr.sin_addr), ntohs(calculators[i].addr.sin_port));
		b += calculators[i].threads * stepLimits;
		calculators[i].arg.limits.a = a;
		calculators[i].arg.limits.b = b;
		a = b;
		calculators[i].arg.parts = numberOfPartsPerThread * calculators[i].threads;
		args[i].server           = &(calculators[i]);

		if (pthread_create(&(args[i].thread), NULL, calculate, &(args[i])) != 0)
		{
			perror("Can't create thread");
			exit(1);
		}
	}
	long double result = 0;
	for (i = 0; i < settings.numOfServers; i++)
	{
		pthread_join(args[i].thread, NULL);
		result += args[i].result;
	}
	printf("%Lf\n", result);

	free(args);
	free(calculators);
	close(udp_socket_fd);

	return 0;
}

