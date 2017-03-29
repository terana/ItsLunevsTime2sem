#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <strings.h>

#define FUNC(x) x*x

typedef struct
{
	int         limits;
	long        numoftry;
	long double *results;
} thread_args_t;

void *calculate(void *arg)
{
	thread_args_t *tinfo = (thread_args_t *) arg;
	long double   result = 0;
	int           xlim   = tinfo->limits;
	int           trys   = tinfo->numoftry;
	unsigned      a      = xlim;
	for (int      i      = 0; i < trys; ++i)
	{
		int    div  = rand_r(&a);
		int    div2 = rand_r(&a);
		double x    = div % xlim + (div2 / (div2 * 1.0 + div * 1.0));
		result += FUNC(x);
	}
	*(tinfo->results) = result;
	return NULL;
}

int main(int argc, char **argv)
{
	int numoftry = 1000000000;
	int limits   = 2;

	// Аргумент может быть только один - это кол-во тредов
	if (argc > 2)
	{
		fprintf(stderr, "Usage: %s [numofcpus]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int numofthread;

	if (argc == 2)
	{
		numofthread = atoi(argv[1]);
		if (numofthread < 1)
		{
			fprintf(stderr, "Incorrect num of threads!\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "Num of threads forced to %d\n", numofthread);
	}
	else
	{
		numofthread = 2;
		fprintf(stdout, "Num of threads detected automatically it's %d\n\n",
				numofthread);
	}
	int           numofthreadtry = numoftry / numofthread + 1;
	thread_args_t *tinfo;
	pthread_t     *calc_threads  =
			              (pthread_t *) malloc(sizeof(pthread_t) * numofthread);
	int           threads_trys   = numoftry % numofthread;
	long double   *results       =
			              (long double *) malloc(sizeof(long double) * numofthread);
	tinfo = (thread_args_t *) malloc(sizeof(thread_args_t) *
			numofthread);
	for (int    i          = 0; i < numofthread; ++i)
	{
		tinfo[i].limits   = limits;
		tinfo[i].numoftry = numofthreadtry;
		tinfo[i].results  = &results[i];
		if (pthread_create(&calc_threads[i], NULL, calculate, &tinfo[i])
				!= 0)
		{
			fprintf(stderr, "Can't create thread by num %d", i);
			perror("Detail:");
			exit(EXIT_FAILURE);
		}
	}
	int         iscanceled = 0; // Почему завершились треды?
	int         *exitstat;
	for (int    i          = 0; i < numofthread; ++i)
	{
		pthread_join(calc_threads[i], (void *) &exitstat);
	}
	long double *res       = (long double *) malloc(sizeof(long double));
	bzero(res, sizeof(long double));
	*res = 0.0;
	for (int i = 0; i < numofthread; ++i)
	{
		*res += results[i];
	}
	printf("%Lf\n", *res/numoftry*limits);
}
