#ifndef FS_H
#define FS_H

//File System APIs
int formatDrive()
int createSFS(char* filename, int nbytes);
int readData(int disk, int blockNum, void* block);
int writeData(int disk, int blockNum, void* block);

int writeFile(int disk, char* filename, void* block);
int readFile(int disk, char* filename, void* block);

//Diagnostic APIs
void print_inodeBitmaps(int fileSystemId)
void print_dataBitmaps(int fileSystemId)
void print_FileList(int fileSystemId)

#endif
