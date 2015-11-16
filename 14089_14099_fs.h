#ifndef FS_H
#define FS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define STREAM_DELIM 0
#define SUPER_BLK 0
#define DATA_BITMAP_BLK 1
#define INODE_BITMAP_BLK 2
#define INODE_BLK_ST 3
unsigned int BLK_SIZE;
unsigned int INODE_SIZE;
unsigned int BITMAP_SIZE;
unsigned int INODES_PER_BLOCK;
unsigned int DISK_SIZE; 
unsigned int FREE_BLK_CNT;
unsigned int FREE_INODE_CNT;
//Data Structures

/*
	Super Block layout: Block Size | Inode Size | Bitmap Size | Inodes_per_block | Disk_size
*/

/*Inode - 16 Bytes
 * [0-7] = Name
 * [8-9] = No. of blocks
 * [10-11] = First block
 * [12-15] = Size of file
 */
typedef struct _inode
{
	int id;
	char name[9];
	unsigned short nblocks;
	unsigned short first_blk;
	unsigned int file_size;
} inode;

inode* generateInode(void* buffer,int id);
void* dumpInode(inode* a);

typedef struct _bitmap
{
	char* bit_array;	
} bitmap;

bitmap data_bitmap,inode_bitmap;

//Bitmap Manipulation
void loadBitmap(bitmap* b,int blk,int disk);
void flushBitmap(bitmap* b,int blk,int disk);
void set_bit(bitmap *b,int i);
void unset_bit(bitmap *b,int i);
int check_bit(bitmap *b,int i);


//File System APIs
int createSFS(char* filename, int nbytes);
int readData(int disk, int blockNum, void* block);
int writeData(int disk, int blockNum, void* block);

int writeFile(int disk, char* filename, void* block);
int readFile(int disk, char* filename, void* block);

//Diagnostic APIs
void print_inodeBitmaps(int fileSystemId);
void print_dataBitmaps(int fileSystemId);
void print_FileList(int fileSystemId);

//Support Functions
void print_bits(unsigned int x);
void loadSuperBlock(int fileSystemId);
void dumpSuperBlock(int fileSystemId);
int writeFileExisting(int disk,inode* Inode,void* block);
int writeFileNew(int disk,char* filename,void* block);
#endif
