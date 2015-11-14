#include "14089_14099_fs.h"

int main()
{
	//128 MB Hard Disk with Block size 4 KB
	formatDrive(32*1024*4*1024);
	assert(bootDisk()==1);
	loadBitmap(&inode_bitmap,INODE_BLK);
	set_bit(&inode_bitmap,6);
	flushBitmap(&inode_bitmap,INODE_BLK);
	loadBitmap(&inode_bitmap,INODE_BLK);
	printf("%s\n",inode_bitmap.bit_array);
	return shutDown();
}