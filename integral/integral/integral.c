//
//  main.c
//  integral
//
//  Created by Anastasia on 22/03/2017.
//  Copyright (c) 2017 terana. All rights reserved.
//


#define _GNU_SOURCE

#include <stdio.h>

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

#define FUNC(x) (x*x)

typedef struct
{
	long double a;
	long double b;
} limits_t;

typedef struct
{
	limits_t    limits;
	long        numberOfParts;
	long double result;
	pthread_t   thread;
} thread_argument_t;

void *calculate(void *param)
{
	thread_argument_t *arg   = (thread_argument_t *) param;
	long double       result = 0;
	long double       a      = arg->limits.a;
	long double       b      = arg->limits.b;
	long              parts  = arg->numberOfParts;
	long double       step   = (b - a) / parts;
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

typedef struct
{
	limits_t lim;
	long     numberOfThreads;
	long     numberOfParts;
} settings_t;

void parseInputArgs(int argc, char **argv, settings_t *settings, int *needHelp)
{
	if (argc <= 1)
	{
		*needHelp = 1;
		return;
	}
	if (argc == 2)
	{
		char **endptr = NULL;
		errno         = 0;
		settings->numberOfThreads = strtol(argv[1], endptr, 10);
//		if (errno != 0 && )
//		{
//			*needHelp = 1;
//			return;
//		}
		*needHelp = 0;
		return;
	}
	int i;
	for (i    = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (!strcmp(argv[i], "-lim_a"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->lim.a = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					*needHelp = 1;
					return;
				}
			}
			if (!strcmp(argv[i], "-lim_b"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->lim.b = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					*needHelp = 1;
					return;
				}
			}
			if (!strcmp(argv[i], "-n_threads"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->numberOfThreads = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					*needHelp = 1;
					return;
				}
			}
			if (!strcmp(argv[i], "-n_parts"))
			{
				i++;
				char **endptr = NULL;
				errno         = 0;
				settings->numberOfParts = strtol(argv[i], endptr, 10);
				if (errno != 0)
				{
					*needHelp = 1;
					return;
				}
			}
		}
		else
		{
			*needHelp = 1;
			return;
		}
	}
	*needHelp = 0;
}

void printHelpMessage(const char *programName)
{
	printf("%s", "This programm calculates the integral of x^2 from a to b using Simpson method.\n");
	printf("Usage: %s [options]\n", programName);
	printf("%s", "Options:\n");
	printf("%s", "-lim_a <value>        Set lower limit\n");
	printf("%s", "-lim_b <value>        Set higher limit\n");
	printf("%s", "-n_threads <value>    Set number of calculating threads\n");
	printf("%s", "-n_parts <value>      Set number of parts of the interval\n");
	printf("%s", "If some options are not set the default value is used.\n");
	printf("%s", "The default values are: a = 0, b = 2\n");
	printf("%s", "Number of threads should be always set\n");
}

int main(int argc, char **argv)
{
	settings_t settings;
	settings.lim.a           = 0;
	settings.lim.b           = 5;
	settings.numberOfThreads = 0;
	settings.numberOfParts   = 2000000000;

	int needHelp;
	parseInputArgs(argc, argv, &settings, &needHelp);
	if (needHelp)
	{
		printHelpMessage(argv[0]);
		return 0;
	}

	if (settings.numberOfThreads == 0)
	{
		printf("%s", "number of threads:\n");
		scanf("%ld", &(settings.numberOfThreads));
	}
	long        numbrerOfPartsPerTread = settings.numberOfParts / settings.numberOfThreads;
	long double stepLimits             = (settings.lim.b - settings.lim.a) / settings.numberOfThreads;
	long double a                      = settings.lim.a;
	long double b                      = a + stepLimits;

	cpu_set_t cpuset;

	long num_cpu = sysconf(_SC_NPROCESSORS_ONLN);

	thread_argument_t *args = malloc(settings.numberOfThreads * sizeof(thread_argument_t));
	int               i;

	for (i = 0; i < settings.numberOfThreads; i++)
	{
		(args[i]).limits.a      = a;
		(args[i]).limits.b      = b;
		(args[i]).numberOfParts = numbrerOfPartsPerTread;
		a = b;
		b += stepLimits;

		if (pthread_create(&(args[i].thread), NULL, calculate, &(args[i])) != 0)
		{
			perror("Can't create thread");
			exit(1);
		}
		CPU_ZERO(&cpuset);
		CPU_SET(i % num_cpu, &cpuset);
		errno = 0;
		if (pthread_setaffinity_np(args[i].thread, sizeof(cpu_set_t), &cpuset) != 0)
		{
			perror("Can't set affinity");
			exit(1);
		}
		printf("set thread %d to cpu %ld\n", i, i % num_cpu);
	}

	long double result = 0;

	for (i = 0; i < settings.numberOfThreads; i++)
	{
		pthread_join(args[i].thread, NULL);
		result += args[i].result;
	}
	printf("%Lf\n", result);

	free(args);
	return 0;
}
