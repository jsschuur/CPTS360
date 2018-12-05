#include "bitmap.h"
#include "readwrite.h"

int test_bit(char *buf, int bit)
{
	int byte = bit / 8;
	bit = bit % 8;
	
	if (buf[byte] & (1 << bit))
	{
		return 1;
	}
	return 0;
}

void set_bit(char *buf, int bit)
{
	int byte = bit / 8;
	bit = bit % 8;

	buf[byte] |= (1 << bit);
}

void clear_bit(char *buf, int bit)
{
	int byte = bit / 8;
	bit = bit % 8;

	buf[byte] &= ~(1 << bit);
}

int decFreeBlocks(int dev)
{
	char buf[BLOCK_SIZE];
	SUPER *sp;
	GD *gp;

	get_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	sp = (SUPER *)buf;
	sp->s_free_blocks_count--;

	put_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	get_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	gp = (GD*)buf;
	gp->bg_free_blocks_count--;
	
	put_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	return 0;
}


int decFreeInodes(int dev)
{
	char buf[BLOCK_SIZE];
	SUPER *sp;
	GD *gp;

	get_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	sp = (SUPER *)buf;
	sp->s_free_inodes_count--;

	put_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	get_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	gp = (GD*)buf;
	gp->bg_free_inodes_count--;
	
	put_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	return 0;
}

int incFreeBlocks(int dev)
{
	char buf[BLOCK_SIZE];
	SUPER *sp;
	GD *gp;

	get_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;

	put_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	get_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	gp = (GD*)buf;
	gp->bg_free_blocks_count++;
	
	put_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	return 0;
}

int incFreeInodes(int dev)
{
	char buf[BLOCK_SIZE];
	SUPER *sp;
	GD *gp;

	get_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	sp = (SUPER *)buf;
	sp->s_free_inodes_count++;

	put_block(dev, SUPER_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	get_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	gp = (GD*)buf;
	gp->bg_free_inodes_count++;
	
	put_block(dev, GD_BLOCK_OFFSET, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	return 0;
}

int allocate_block(int dev)
{
	int i;
	char buf[BLOCK_SIZE];

	get_block(dev, block_bitmap, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	for(i = 0; i < nblocks; i++)
	{
		if(test_bit(buf, i) == 0)
		{
			set_bit(buf, i);
			decFreeBlocks(dev);

			put_block(dev, block_bitmap, buf);
			if(thrown_error == TRUE)
			{
				return -1;
			}
			return i + 1;
		}
	}
	set_error("No more free inodes\n");
	return -1;
}


int allocate_inode(int dev)
{
	int i;
	char buf[BLOCK_SIZE];

	get_block(dev, inode_bitmap, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	for(i = 0; i < ninodes; i++)
	{
		if(test_bit(buf, i) == 0)
		{
			set_bit(buf, i);
			decFreeInodes(dev);

			put_block(dev, inode_bitmap, buf);
			if(thrown_error == TRUE)
			{
				return -1;
			}
			return i + 1;
		}
	}
	set_error("No more free inodes\n");
	return -1;
}

int deallocate_block(int dev, int block)
{
	char buf[BLOCK_SIZE];
	if(block <= 0)
	{
		set_error("block number invalid (bitmap.c)\n");
		return -1;
	}

	get_block(dev, block_bitmap, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	clear_bit(buf, block);
	incFreeBlocks(dev);

	put_block(dev, block_bitmap, buf);

	return 0;
}

int deallocate_inode(int dev, int ino)
{
	char buf[BLOCK_SIZE];
	if(ino <= 0)
	{
		set_error("inode number invalid (bitmap.c)\n");
		return -1;
	}

	get_block(dev, block_bitmap, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	clear_bit(buf, ino);
	incFreeInodes(dev);

	put_block(dev, inode_bitmap, buf);

	return 0;
}