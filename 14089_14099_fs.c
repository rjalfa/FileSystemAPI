#include "14089_14099_fs.h"

/*//Bitmap Manipulation
void loadBitmap(bitmap* b,int blk,int disk)
{
	b->bit_array = (char*)calloc(BLK_SIZE+1,sizeof(char));
	assert(readData(disk,blk,b->bit_array)>0);
}

void flushBitmap(bitmap* b,int blk,int disk)
{
	assert(writeData(disk,blk,b->bit_array)>0);
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

*/

//Data Sturctures
inode* generateInode(char* buffer)
{
	unsigned char* buf = (unsigned char*) buffer;
	inode* a = (inode*)malloc(sizeof(inode));
	int i;
	for(i=0;i<8;i++) a->name[i] = buf[i];
	a->name[i] = '\0';
	a->nblocks = (buf[i] << 8) + buf[i+1];
	i+=2;
	a->first_blk = (buf[i] << 8) + buf[i+1];
	i+=2;
	a->file_size = (buf[i] << 24) + (buf[i+1] << 16) + (buf[i+2] << 8) + buf[i+3];
	return a;
}

void* dumpInode(inode* a)
{
	void* buf = calloc(16,1);
	int i;
	for(i=0;i<strlen(a->name);i++) ((unsigned char*)buf)[i] = a->name[i];
	((unsigned short*)buf)[4] = a->nblocks;
	((unsigned short*)buf)[5] = a->first_blk;
	((unsigned int*)buf)[3] = a->file_size;
	return (void*)buf;
}

// File System APIs

void print_bits(unsigned int x)
{
    int i;
    for(i=8*sizeof(x)-1; i>=0; i--) {
        (x & (1 << i)) ? putchar('1') : putchar('0');
    }
}

int readData(int disk, int blockNum, void* block)
{
	lseek(disk,blockNum*BLK_SIZE,SEEK_SET);
	if(errno) return -1;
	int ret = read(disk,block,BLK_SIZE);
	return ret;
}

int writeData(int disk, int blockNum, void* block)
{
	lseek(disk,blockNum*BLK_SIZE,SEEK_SET);
	if(errno) return -1;
	int ret = write(disk,block,BLK_SIZE);
	return ret;
}

int createSFS(char* filename,int nbytes)
{
	int fd = open(filename,O_RDWR | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd < 0) return -1;
	lseek(fd,0,SEEK_SET);
	write(fd,(void*)calloc(nbytes,1),nbytes);
	return fd;
}

void print_inodeBitmaps(int fileSystemId)
{
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemId,INODE_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
}

void print_dataBitmaps(int fileSystemId)
{
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemId,DATA_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
}