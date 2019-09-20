typedef struct
{
	long phys_cpus;
	long *cores_per_phys;
	long cores;
} phys_info;

int getPhysicalProcessorsInfo(phys_info *info);