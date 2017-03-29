//
//  main.c
//  integral
//
//  Created by Anastasia on 22/03/2017.
//  Copyright (c) 2017 terana. All rights reserved.
//

#define FUNC(x) x*x

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/errno.h>

typedef struct
{
	long        limits;
	long        numberOfPoints;
	long double *result;
} thread_argument_t;

void *calculate(void *param)
{
	thread_argument_t *arg   = (thread_argument_t *) param;
	long double       result = 0;
	long              lim    = arg->limits;
	long              points = arg->numberOfPoints;
	unsigned          a      = lim;
	for (int          i      = 0; i < points; i++)
	{
		int    div  = rand_r(&a);
		int    div2 = rand_r(&a);
		double x    = div % lim + (div2 / (div2 * 1.0 + div * 1.0));
		result += FUNC(x);
	}
	*(arg->result) = result;
	free(arg);
	return NULL;
}

typedef struct
{
	long a;
	long b;
} limits_t;

typedef struct
{
	limits_t lim;
	long     numberOfThreads;
	long     numberOfPoints;
} settings_t;

void parseInputArgs(int argc, char **argv, settings_t *settings, int *needHelp)
{
	if (argc <= 1)
	{
		*needHelp = 1;
		return;
	}
	if (argc == 2){
		char **endptr = NULL;
		errno         = 0;
		settings->numberOfThreads = strtol(argv[1], endptr, 10);
//		if (errno != 0 && )
//		{
//			*needHelp = 1;
//			return;
//		}
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
			if (!strcmp(argv[i], "-n_points"))
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
	printf("%s", "This programm calculates the integral of x^2 from a to b using Monte Carlo method.\n");
	printf("Usage: %s [options]\n", programName);
	printf("%s", "Options:\n");
	printf("%s", "-lim_a <value>        Set lower limit\n");
	printf("%s", "-lim_b <value>        Set higher limit\n");
	printf("%s", "-n_threads <value>    Set number of calculating threads\n");
	printf("%s", "-n_points <value>     Set total number of points\n");
	printf("%s", "If some options are not set the default value is used.\n");
	printf("%s", "The default values are: a = 0, b = 2, number of points = 1000000000\n");
	printf("%s", "Number of threadsshould be always set\n");
}


int main(int argc, char **argv)
{
	settings_t settings;
	settings.lim.a           = 0;
	settings.lim.b           = 2;
	settings.numberOfThreads = 0;
	settings.numberOfPoints  = 1000000000;

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
	long lim = settings.lim.b;

	long              numberOfPointsPerThread = settings.numberOfPoints / settings.numberOfThreads;
	long double       *results                = malloc(settings.numberOfThreads * sizeof(long double));
	thread_argument_t *arg;
	pthread_t         *threads                = malloc(settings.numberOfThreads * (sizeof(pthread_t)));
	int               i;

	for (i = 0; i < settings.numberOfThreads; i++)
	{
		arg = malloc(sizeof(thread_argument_t));
		arg->limits         = lim;
		arg->numberOfPoints = numberOfPointsPerThread;
		arg->result         = &(results[i]);

		if (pthread_create(&(threads[i]), NULL, calculate, arg) != 0)
		{
			perror("Can't create thread");
			exit(1);
		}
	}

	long double result = 0;

	for (i = 0; i < settings.numberOfThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	for (i = 0; i < settings.numberOfThreads; i++)
	{
		result += results[i] / numberOfPointsPerThread * lim;
	}
	printf("%Lf\n", result / settings.numberOfThreads);

	free(results);
	free(threads);
	return 0;
}