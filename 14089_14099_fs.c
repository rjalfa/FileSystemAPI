#include "14089_14099_fs.h"
#include <math.h>
//Bitmap Manipulation
void loadBitmap(bitmap* b,int blk,int fileSystemID)
{
	b->bit_array = (char*)calloc(BLK_SIZE+1,sizeof(char));
	assert(readData(fileSystemID,blk,b->bit_array) == BLK_SIZE);
}

void flushBitmap(bitmap* b,int blk,int fileSystemID)
{
	assert(writeData(fileSystemID,blk,b->bit_array) == BLK_SIZE);
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

//Data Structures
inode* generateInode(void* buf,int id)
{
	inode* a = (inode*)malloc(sizeof(inode));
	a->id = id;
	int i;
	memcpy(a->name,buf,8);
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
    for(i=0; i<=8*sizeof(x)-1; i++) {
        (x & (1 << i)) ? putchar('1') : putchar('0');
    }
}

/*
	Super Block layout: Block Size | Inode Size | Bitmap Size | Inodes_per_block | Disk_size
*/
void loadSuperBlock(int fileSystemID)
{
	lseek(fileSystemID,0,SEEK_SET);
	read(fileSystemID,&BLK_SIZE,sizeof(BLK_SIZE));
	read(fileSystemID,&INODE_SIZE,sizeof(INODE_SIZE));
	read(fileSystemID,&BITMAP_SIZE,sizeof(BITMAP_SIZE));
	read(fileSystemID,&INODES_PER_BLOCK,sizeof(INODES_PER_BLOCK));
	read(fileSystemID,&DISK_SIZE,sizeof(DISK_SIZE));
}

int readInode(int fileSystemID,int inodeNum,inode** Inode)
{
	loadSuperBlock(fileSystemID);
	int blockNum = INODE_BLK_ST+(inodeNum / INODES_PER_BLOCK);
	void* buf = malloc(BLK_SIZE);
	assert(readData(fileSystemID,blockNum,buf) == BLK_SIZE);
	void* buf_inode = malloc(INODE_SIZE);
	memcpy(buf_inode,buf+(inodeNum%(INODES_PER_BLOCK))*INODE_SIZE,INODE_SIZE);
	*Inode = generateInode(buf_inode,inodeNum);
	free(buf_inode);
	free(buf);
	return 1;
}

int writeInode(int fileSystemID,inode* Inode)
{
	loadSuperBlock(fileSystemID);
	int blockNum = INODE_BLK_ST+((Inode->id) / INODES_PER_BLOCK);
	void* buf = malloc(BLK_SIZE);
	errno = 0;
	int ch = readData(fileSystemID,blockNum,buf);
	assert(ch == BLK_SIZE);
	void* buf_inode = dumpInode(Inode);
	memcpy(buf+((Inode->id)%(INODES_PER_BLOCK))*INODE_SIZE,buf_inode,INODE_SIZE);
	free(buf_inode);
	return writeData(fileSystemID,blockNum,buf);
}

int GetBlockArray(int fileSystemID,int nblocks)
{
	loadBitmap(&data_bitmap,DATA_BITMAP_BLK,fileSystemID);
	int i,cnt=0,start;
	for(i=0;i<BITMAP_SIZE;i++)
	{
		if(cnt == nblocks) break;  
		if(check_bit(&data_bitmap,i))
		{
			cnt = 0;
			start = -1;
		}
		else
		{
			if(start == -1) start = i;
			cnt++;
		}
	}
	if(cnt == nblocks) return start;
	return -1;
}

int readData(int fileSystemID, int blockNum, void* block)
{
	loadSuperBlock(fileSystemID);
	errno = 0;
	lseek(fileSystemID,blockNum*BLK_SIZE,SEEK_SET);
	if(errno) return -1;
	int ret = read(fileSystemID,block,BLK_SIZE);
	return ret;
}

int writeData(int fileSystemID, int blockNum, void* block)
{
	loadSuperBlock(fileSystemID);
	errno = 0;
	lseek(fileSystemID,blockNum*BLK_SIZE,SEEK_SET);
	if(errno) return -1;
	errno = 0;
	int ret = write(fileSystemID,block,BLK_SIZE);
	return ret;
}

int createSFS(char* filename,int nbytes)
{
	int i,fileSystemID = open(filename,O_RDWR);
	if(fileSystemID < 0) 
	{
		fileSystemID = open(filename,O_RDWR | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if(fileSystemID < 0) return -1;
		lseek(fileSystemID,0,SEEK_SET);
		write(fileSystemID,(void*)calloc(nbytes,1),nbytes);
		BLK_SIZE = 4096;
		INODE_SIZE = 16;
		BITMAP_SIZE = BLK_SIZE*8;
		INODES_PER_BLOCK = BLK_SIZE/INODE_SIZE;
		DISK_SIZE = nbytes;
		lseek(fileSystemID,0,SEEK_SET);
		write(fileSystemID,&BLK_SIZE,sizeof(BLK_SIZE));
		write(fileSystemID,&INODE_SIZE,sizeof(INODE_SIZE));
		write(fileSystemID,&BITMAP_SIZE,sizeof(BITMAP_SIZE));
		write(fileSystemID,&INODES_PER_BLOCK,sizeof(INODES_PER_BLOCK));
		write(fileSystemID,&DISK_SIZE,sizeof(DISK_SIZE));
		loadBitmap(&data_bitmap,DATA_BITMAP_BLK,fileSystemID);
		for(i=0;i<3+BITMAP_SIZE/INODES_PER_BLOCK;i++) set_bit(&data_bitmap,i);

		flushBitmap(&data_bitmap,DATA_BITMAP_BLK,fileSystemID);
	}
	else loadSuperBlock(fileSystemID);
	if(BLK_SIZE == 0) {printf("Invalid File System!");return -1;}
	return fileSystemID;
}

int readFile(int fileSystemID,char* filename,void* block)
{
	int i,flag = 0;
	inode* in;
	loadSuperBlock(fileSystemID);
	loadBitmap(&inode_bitmap,INODE_BITMAP_BLK,fileSystemID);
	for(i=0;i<BITMAP_SIZE;i++) if(check_bit(&inode_bitmap,i)) 
	{
		readInode(fileSystemID,i,&in);
		if(strcmp(in->name,filename)==0) {flag = 1;break;}
		else free(in);
	}
	if(flag) 
	{
		lseek(fileSystemID,in->first_blk*BLK_SIZE,SEEK_SET);
		return read(fileSystemID,block,in->file_size);
	}
	else return -1;
}

int writeFile(int fileSystemID,char* filename,void* block)
{
	loadSuperBlock(fileSystemID);
	loadBitmap(&inode_bitmap,INODE_BITMAP_BLK,fileSystemID);
	int ret,i,space_free = -1,fs_free = -1,flag = 0;
	for(i=0;i<BITMAP_SIZE;i++) 
	{
		if(check_bit(&inode_bitmap,i) == 0) if(fs_free == -1) fs_free = i;
		if(fs_free != -1) {flag = 1;break;}
	}
	if(!flag) return -1;
	inode wrInode;
	strcpy(wrInode.name,filename);
	wrInode.id = fs_free;
	int fsize = 0;
	for(i=0;((unsigned char*)block)[i] != STREAM_DELIM;i++);
	fsize = i;
	wrInode.nblocks = ceil(fsize*1.0/BLK_SIZE);
	space_free = GetBlockArray(fileSystemID,wrInode.nblocks);
	if(space_free == -1) return -1; 
	wrInode.first_blk = space_free;
	wrInode.file_size = fsize;
	set_bit(&inode_bitmap,fs_free);
	lseek(fileSystemID,wrInode.first_blk*BLK_SIZE,SEEK_SET);
	if(write(fileSystemID,block,fsize) > 0)
	{
		writeInode(fileSystemID,&wrInode);
		for(i=wrInode.first_blk;i<wrInode.first_blk+wrInode.nblocks;i++) set_bit(&data_bitmap,i);
		flushBitmap(&inode_bitmap,INODE_BITMAP_BLK,fileSystemID);
		flushBitmap(&data_bitmap,DATA_BITMAP_BLK,fileSystemID);
	}
	return ret;
}

void print_inodeBitmaps(int fileSystemID)
{
	loadSuperBlock(fileSystemID);
	printf("INODE BITMAP:\n");
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemID,INODE_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
	printf("\n");
	free(buf);
}

void print_dataBitmaps(int fileSystemID)
{
	loadSuperBlock(fileSystemID);
	printf("DATA BITMAP:\n");
	void* buf = calloc(BLK_SIZE,1);
	assert(readData(fileSystemID,DATA_BITMAP_BLK,buf) == BLK_SIZE);
	int i;
	for (i = 0; i < BLK_SIZE/sizeof(int); i++) {
 		print_bits(((int *) buf) [i]);
	}
	printf("\n");
	free(buf);
}

void print_FileList(int fileSystemID)
{
	loadSuperBlock(fileSystemID);
	printf("Files in the system:\n");
	loadBitmap(&inode_bitmap,INODE_BITMAP_BLK,fileSystemID);
	int i;
	for(i=0;i<BITMAP_SIZE;i++) if(check_bit(&inode_bitmap,i)) 
	{
		inode* in;
		readInode(fileSystemID,i,&in);
		printf("%s,%d bytes @ block %d [ %d blocks ]\n",in->name,in->file_size,in->first_blk,in->nblocks);
		free(in);
	}
}