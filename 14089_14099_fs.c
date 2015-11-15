#include "14089_14099_fs.h"
unsigned int BLK_SIZE = 4096;
unsigned int INODE_SIZE = 16;
//Bitmap Manipulation
void loadBitmap(bitmap* b,int blk,int disk)
{
	b->bit_array = (char*)calloc(BLK_SIZE+1,sizeof(char));
	assert(readData(disk,blk,b->bit_array) == BLK_SIZE);
}

void flushBitmap(bitmap* b,int blk,int disk)
{
	assert(writeData(disk,blk,b->bit_array) == BLK_SIZE);
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

//Data Sturctures
inode* generateInode(char* buffer)
{
	unsigned char* buf = (unsigned char*) buffer;
	inode* a = (inode*)malloc(sizeof(inode));
	int i;
	memcpy(a->name,buf,8);
	for(i=0;i<8;i++) a->name[i] = buf[i];
	memcpy(&(a->nblocks),buf+8,sizeof(a->nblocks));
	memcpy(&(a->first_blk),buf+10,sizeof(a->first_blk));
	memcpy(&(a->file_size),buf+12,sizeof(a->file_size));
	return a;
}

void* dumpInode(inode* a)
{
	void* buf = calloc(16,1);
	int i;
	memcpy(buf,a->name,8);
	memcpy(buf+8,&(a->nblocks),sizeof(a->nblocks));
	memcpy(buf+10,&(a->first_blk),sizeof(a->first_blk));
	memcpy(buf+12,&(a->file_size),sizeof(a->file_size));
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

int readInode(int disk,int inodeNum,inode* Inode)
{
	int blockNum = INODE_BLK_ST+(inodeNum / INODES_PER_BLOCK);
	void* buf = malloc(BLK_SIZE);
	assert(readData(disk,blockNum,buf) == BLK_SIZE);
	void* buf_inode = malloc(INODE_SIZE);
	memcpy(buf_inode,buf+(inodeNum%(INODES_PER_BLOCK))*INODE_SIZE,INODE_SIZE);
	Inode = generateInode(buf_inode);
	return 1;
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
	BITMAP_SIZE = BLK_SIZE*8;
	INODES_PER_BLOCK = BLK_SIZE/INODE_SIZE;
	MIN_DISK_SIZE = INODE_BLK_ST+BITMAP_SIZE/INODES_PER_BLOCK; 
	return fd;
}

void print_inodeBitmaps(int fileSystemId)
{
	printf("INODE BITMAP:\n");
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemId,INODE_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
	printf("\n");
}

void print_dataBitmaps(int fileSystemId)
{
	printf("DATA BITMAP:\n");
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemId,DATA_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
	printf("\n");
}

void print_FileList(int fileSystemId)
{
	printf("Files in the system:\n");
	loadBitmap(&inode_bitmap,INODE_BITMAP_BLK,fileSystemId);
	int i;
	for(i=0;i<BITMAP_SIZE;i++) if(check_bit(&inode_bitmap,i)) 
	{
		inode* in;
		readInode(fileSystemId,i,in);
		printf("%s,%d bytes @ block %d\n",in->name,in->file_size,in->first_blk);
	}
}