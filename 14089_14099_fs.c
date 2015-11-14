#include "14089_14099_fs.h"

//Bitmap Manipulation
void loadBitmap(bitmap* b,int blk)
{
	b->bit_array = (char*)malloc(BLK_SIZE+1);
	assert(readData(PRIMARY_DISK,blk,b->bit_array)>0);
}

void flushBitmap(bitmap* b,int blk)
{
	assert(writeData(PRIMARY_DISK,blk,b->bit_array)>0);
}

void set_bit(bitmap *b,int i)
{
	b->bit_array[i/8] |= (1 << (i%8));
}

void unset_bit(bitmap *b,int i)
{
	b->bit_array[i/8] &= ~(1 << (i%8));
}

int check_bit(bitmap *b,int i)
{
	return (b->bit_array[i/8] & (1 << i%8)) != 0;	
}

// File System APIs
int formatDrive(long long size)
{
	remove("14089_14099_disk.dat");
	disk_ptr = fopen("14089_14099_disk.dat","wb");
	long long i;
	for(i=0;i<size;i++) fputc((char)0,disk_ptr);
	fclose(disk_ptr);
}

int bootDisk()
{
	disk_ptr = fopen("14089_14099_disk.dat","rb+");
	if(disk_ptr == NULL) return 0;
	return 1;
}

int shutDown()
{
	return fclose(disk_ptr);
}

int readData(int disk, int blockNum, void* block)
{
	if(disk_ptr && disk == PRIMARY_DISK)
	{
		fseek(disk_ptr,blockNum*BLK_SIZE,SEEK_SET);
		fgets(block,sizeof(block),disk_ptr);
		return sizeof(block);
	}
	else return 0;
}

int writeData(int disk, int blockNum, void* block)
{
	if(disk_ptr && disk == PRIMARY_DISK)
	{
		fseek(disk_ptr,blockNum*BLK_SIZE,SEEK_SET);
		fputs(block,disk_ptr);
		return sizeof(block);
	}
	else return 0;
}

int createSFS(char* filename,int nbytes)
{
	int i;
	while(!check_bit(inode_bitmap,i) && i < BLK_SIZE*8) i++;
	if(i == BLK_SIZE*8) return -1;
	set_bit(inode_bitmap,i);
	return i;
}