#include "14089_14099_fs.h"

int main()
{
	//128 MB Hard Disk with Block size 4 KB
	int disk = createSFS("14089_14099_disk.dat",1024*1024*128);
	print_dataBitmaps(disk);
	//loadBitmap(&inode_bitmap,INODE_BLK,disk);
	//set_bit(&inode_bitmap,6);
	//flushBitmap(&inode_bitmap,INODE_BLK,disk);
	//loadBitmap(&inode_bitmap,INODE_BLK,disk);
	printf("%u,%u,%u,%u,%u\n",BLK_SIZE,INODE_SIZE,BITMAP_SIZE,INODES_PER_BLOCK,DISK_SIZE);
	void* str = malloc(1000);
	strcpy(str,"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	printf("%d\n",writeFile(disk,"f.out",str));
	printf("%s\n",(char*)str);
	print_FileList(disk);
}