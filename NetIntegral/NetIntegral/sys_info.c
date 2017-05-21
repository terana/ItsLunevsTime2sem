#include "sys_info.h"
#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <string.h>

#define FREE_AND_CLOSE   do {fclose(cpuinfo);\
                                    free(buf);\
                                    free(cores);}\
                                 while(0);

#define RETURN_ERROR return -1;

long getNumber(const char *str)
{
	while ((*str < '0') || (*str > '9'))
	{str++;}
	return strtol(str, (char **) NULL, 10);
}

int getPhysicalProcessorsInfo(phys_info *info)
{
	FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo == NULL)
	{
		perror("Error finding cpuinfo");
		RETURN_ERROR
	}

	long max_cpu = sysconf(_SC_NPROCESSORS_ONLN);

	char *buf = malloc(30 * sizeof(char));
	if (buf == NULL)
	{
		perror(NULL);
		fclose(cpuinfo);
		RETURN_ERROR
	}

	long *cores      = calloc((size_t) max_cpu, sizeof(long));
	long total_cores = 0;
	if (cores == NULL)
	{
		perror(NULL);
		fclose(cpuinfo);
		free(buf);
		RETURN_ERROR
	}

	long phys_cpu = 0;
	char *res;
	long n;
	do
	{
		errno = 0;
		res = fgets(buf, 30, cpuinfo);
		if (errno != 0)
		{
			perror(NULL);
			FREE_AND_CLOSE
			RETURN_ERROR
		}
		if (strstr(buf, "physical"))
		{
			errno = 0;
			n = getNumber(buf);
			if (errno != 0)
			{
				perror(NULL);
				FREE_AND_CLOSE
				RETURN_ERROR
			}
			if (n >= phys_cpu)
			{
				phys_cpu++;
			}
			continue;
		}
		if (phys_cpu && (cores[phys_cpu - 1] == 0))
		{
			if (strstr(buf, "cpu cores"))
			{
				n = getNumber(buf);
				if (errno != 0)
				{
					perror(NULL);
					FREE_AND_CLOSE
					RETURN_ERROR
				}
				cores[phys_cpu - 1] = n;
				total_cores += n;
			}
		}
	}
	while (res);

	info->phys_cpus      = phys_cpu;
	info->cores_per_phys = malloc(phys_cpu * sizeof(long));
	info->cores          = total_cores;
	for (int i = 0; i < phys_cpu; i++)
	{
		info->cores_per_phys[i] = cores[i];
	}
	FREE_AND_CLOSE
	return 0;
}

