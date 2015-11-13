#include "14089_14099_fs.h"

int formatDrive(long long size)
{
	remove("14089_14099_disk.dat");
	FILE* d = fopen("14089_14099_disk.dat","wb");
	long long i;
	for(i=0;i<size;i++) fputc((char)0,d);
	fclose(d);
}