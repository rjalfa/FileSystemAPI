#ifndef FS_H
#define FS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#define BLK_SIZE 4096
#define SUPER_BLK 0
#define BITMAP_BLK 1
#define INODE_BLK 2

//Data Structures
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

#endif
