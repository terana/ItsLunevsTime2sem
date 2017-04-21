#include <stdio.h>
#include "sys_info.h"

int main()
{
	phys_info info;
	int       res = getPhysicalProcessorsInfo(&info);
	if (res)
	{
		printf("Error getting info\n");
		return 1;
	}
	long phys   = info.phys_cpus;
	long *cores = info.cores_per_phys;
	if (cores == NULL)
	{
		printf("Error getting cores\n");
		return 1;
	}
	printf("total numberof cores: %ld\n", info.cores);
	for (long i = 0; i < phys; i++)
	{
		printf("Physical cpu: %ld has %ld cores\n", i, cores[i]);
	}
	return 0;
}

