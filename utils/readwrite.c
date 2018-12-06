
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "readwrite.h"

#include "get_put_block.h"
#include "../utils/bitmap.h"
#include "error_manager.h"


extern MINODE minodes[NMINODE];
extern int block_size, inodes_per_block, inode_table_block;
extern PROC *running;




MINODE *get_minode(int dev, int ino)
{
	MINODE *mip;
	INODE *ip, *table;
	char buf[BLOCK_SIZE];
	int i, block, offset;

	if(ino < ROOT_INODE)
	{
		return 0;
	}

	for(i = 0; i < NMINODE; i++)
	{
		mip = &minodes[i];

		if(mip->dev == dev &&
			mip->ino == ino)
		{
			mip->refCount++;
			return mip;
		}
	
	}

	for(i = 0; i < NMINODE; i++)
	{
		mip = &minodes[i];
		if(mip->refCount == 0)
		{
			mip->dev = dev;
			mip->ino = ino;
			mip->dirty = FALSE;
			mip->refCount = 1;
			
			block = (ino - 1) / inodes_per_block + inode_table_block;
			offset = (ino - 1) % inodes_per_block;

			get_block(dev, block, buf);

			ip = (INODE *)buf + offset;

			mip->ip = *ip;

			return mip;
		}
	}
	set_error((char*)"No more memory inodes\n");
	return 0;
}

int put_minode(MINODE *mip)
{
	if(!mip) 
	{
		return 0;	
	}
	if(--(mip->refCount) <= 0 && mip->dirty == TRUE)
	{
		int block, index;
		char buf[BLOCK_SIZE];
		INODE *inode_table;

		block = (mip->ino - 1) / inodes_per_block + inode_table_block;
		index = (mip->ino - 1) % inodes_per_block;

		get_block(mip->dev, block, buf);
		if(thrown_error == TRUE)
		{
			return -1;
		}

		inode_table = (INODE *)buf;
		inode_table[index] = mip->ip;

		put_block(mip->dev, block, (char*)inode_table);
		if(thrown_error == TRUE)
		{
			return -1;
		}
		mip->dirty = FALSE;
	}
	return 0;
}

int enter_dir_entry(MINODE *parent_mip, int inode_number, char *name)
{
	int i, block_number, device = parent_mip->dev;
	int need_length, remaining_length, ideal_last_entry;
	char buf[BLOCK_SIZE], *current_ptr;
	DIR *dp;
	INODE *parent_ip = &parent_mip->ip;

	//assume only direct 
	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(parent_ip->i_block[i] == 0)
		{
			break;
		}

		block_number = parent_ip->i_block[i];

		get_block(device, block_number, buf);
		if(thrown_error == TRUE)
		{
			return -1;
		}

		current_ptr = buf;
		dp = (DIR*)buf;

		while(current_ptr + dp->rec_len < buf + block_size)
		{
			current_ptr += dp->rec_len;
			dp = (DIR *)current_ptr;
		}

		need_length = (4 * (( 8 + strlen(name) + 3) / 4));
		ideal_last_entry = (4 * (( 8 + dp->name_len + 3) / 4));
		remaining_length = dp->rec_len - ideal_last_entry;

		if(remaining_length >= need_length)
		{
			dp->rec_len = ideal_last_entry;

			current_ptr += dp->rec_len;
			dp = (DIR*)current_ptr;

			dp->inode = inode_number;
			dp->rec_len = (block_size - (current_ptr - buf));
			dp->name_len = strlen(name);
			dp->file_type = EXT2_FT_DIR;
			strcpy(dp->name, name);

			put_block(device, block_number, buf);
			if(thrown_error == TRUE)
			{
				return -1;
			}
			return 0;
		}
	}

	block_number = allocate_block(device);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	parent_ip->i_block[i] = block_number;
	parent_ip->i_size += block_size;
	parent_mip->dirty = TRUE;

	get_block(device, block_number, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	current_ptr = buf;
	dp = (DIR *)buf;

	dp->inode = inode_number;
	dp->rec_len = block_size;
	dp->name_len = strlen(name);
	dp->file_type = EXT2_FT_DIR;
	strcpy(dp->name, name);

	put_block(device, block_number, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}
	return 0;
}

int remove_dir_entry(MINODE *parent_mip, char *name)
{
	int device = parent_mip->dev, i;
	char buf[BLOCK_SIZE], *current_ptr, *last_ptr, *start, *end;
	DIR *dp, *prev_dp, *last_dp;

	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(parent_mip->ip.i_block[i] == 0)
		{
			break;
		}

		get_block(device, parent_mip->ip.i_block[i], buf);
		if(thrown_error == TRUE)
		{
			return -1;
		}

		current_ptr = buf;
		dp = (DIR *)buf;

		while(current_ptr < buf + block_size)
		{
			//child found
			if(strncmp(dp->name, name, dp->name_len) == 0)
			{
				//only one in block
				if(current_ptr == buf &&
					current_ptr + dp->rec_len == buf + block_size)
				{
					deallocate_block(device, parent_mip->ip.i_block[i]);
					if(thrown_error == TRUE)
					{
						return -1;
					}

					parent_mip->ip.i_size -= block_size;

					while(parent_mip->ip.i_block[i + 1] && i + 1 < NUM_DIRECT_BLOCKS)
					{
						i++;

						get_block(device, parent_mip->ip.i_block[i], buf);
						if(thrown_error == TRUE)
						{
							return -1;
						}
						put_block(device, parent_mip->ip.i_block[i - 1], buf);
						if(thrown_error == TRUE)
						{
							return -1;
						}
					}
				}
				//last entry in block
				else if(current_ptr + dp->rec_len == buf + block_size)
				{
					prev_dp->rec_len += dp->rec_len;
					put_block(device, parent_mip->ip.i_block[i], buf);
				}
				//some middle entry
				else
				{
					last_ptr = current_ptr;
					while(last_ptr + dp->rec_len < buf + block_size)
					{
						last_ptr += dp->rec_len;
						dp = (DIR *)last_ptr;
					}
					dp = (DIR *)current_ptr;
					last_dp = (DIR *)last_ptr;

					last_dp->rec_len += dp->rec_len;

					start = current_ptr + dp->rec_len;
					end = buf + block_size;
					memmove(current_ptr, start, end - start);
					if(check_null_ptr(current_ptr))
					{
						return -1;
					}

					put_block(device, parent_mip->ip.i_block[i], buf);
					if(thrown_error == TRUE)
					{
						return -1;
					}
				}

				parent_mip->dirty = TRUE;

				return 0;
			}

			prev_dp = dp;
			current_ptr += dp->rec_len;
			dp = (DIR *)current_ptr;
		}
	}
	set_error("File does not exist");
	return -1;
}

//will also clear the data in each block literal
int clear_blocks(MINODE *mip)
{
	int *indirect_block_buf, *double_indirect_block_buf, i, j, device = mip->dev;
	char block_buf[BLOCK_SIZE], indirect_buf[BLOCK_SIZE];

	//direct blocks
	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(mip->ip.i_block[i] != 0)
		{
			deallocate_block(device, mip->ip.i_block[i]);
		}
	}

	//indirect blocks
	if(mip->ip.i_block[INDIRECT_BLOCK_NUMBER] != 0)
	{
		get_block(device, mip->ip.i_block[INDIRECT_BLOCK_NUMBER], block_buf);

		indirect_block_buf = (int *)block_buf;

		for(i = 0; i < BLOCK_NUMBERS_PER_BLOCK; i++)
		{
			if(indirect_block_buf[i] != 0)
			{
				deallocate_block(device, indirect_block_buf[i]);
			}
		}
		deallocate_block(device, mip->ip.i_block[INDIRECT_BLOCK_NUMBER]);
	}

	//doubly indirect blocks 
	if(mip->ip.i_block[DOUBLE_INDIRECT_BLOCK_NUMBER] != 0)
	{
		get_block(device, mip->ip.i_block[DOUBLE_INDIRECT_BLOCK_NUMBER], block_buf);

		double_indirect_block_buf = (int *)block_buf;

		for(i = 0; i < BLOCK_NUMBERS_PER_BLOCK; i++)
		{
			if(double_indirect_block_buf[i] != 0)
			{
				get_block(device, double_indirect_block_buf[i], indirect_buf);

				indirect_block_buf = (int *)indirect_buf;

				for(int j = 0; i < BLOCK_NUMBERS_PER_BLOCK; j++)
				{
					if(indirect_block_buf[i] != 0)
					{
						deallocate_block(device, indirect_block_buf[i]);
					}
				}
				deallocate_inode(device, double_indirect_block_buf[i]);
			}
		}
		deallocate_block(device, mip->ip.i_block[DOUBLE_INDIRECT_BLOCK_NUMBER]);
	}

	return 0;
}