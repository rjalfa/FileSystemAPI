#include "14089_14099_fs.h"

int main()
{
	//128 MB Hard Disk with Block size 4 KB
	int disk = createSFS("14089_14099_disk.dat",1024*1024*4);
	//printf("%d\n",disk);
	//loadBitmap(&inode_bitmap,INODE_BLK,disk);
	//set_bit(&inode_bitmap,6);
	//flushBitmap(&inode_bitmap,INODE_BLK,disk);
	//loadBitmap(&inode_bitmap,INODE_BLK,disk);
	print_inodeBitmaps(disk);
}