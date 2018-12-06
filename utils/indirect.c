#include "indirect.h"

#include "readwrite.h"
#include "get_put_block.h"

int search_directory_block_by_ino(int block, int ino)
{
	return 0;
}
int search_directory_block_by_name(int device, int block, char *name)
{
	char buf[BLOCK_SIZE];


	get_block(device, block, buf);

	return 0;
}
