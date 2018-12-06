#include "get_put_block.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "type.h"
#include "error_manager.h"


int get_block(int dev, int blk, char buf[])
{
	if(lseek(dev, (long)BLOCK_SIZE * blk, 0) == -1)
	{
		set_error(strerror(errno));
		return -1;
	}
	if(read(dev, buf, BLOCK_SIZE) == -1)
	{
		set_error(strerror(errno));
		return -1;
	}
	return 0;
}

int put_block(int dev, int blk, char buf[])
{
	if(lseek(dev, (long)BLOCK_SIZE * blk, 0) == -1)
	{
		set_error(strerror(errno));
		return -1;
	}
	if(write(dev, buf, BLOCK_SIZE) == -1)
	{
		set_error(strerror(errno));
		return -1;
	}
	return 0;
}
