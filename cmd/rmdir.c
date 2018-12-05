#include "../cmd.h"
#include "../utils/search.h"
#include <time.h>

extern PROC *running;

int js_rmdir(int argc, char *argv[])
{

	int i = 1, j, inode_number, device = running->cwd->dev, parent_ino, my_ino;
	char buf[BLOCK_SIZE], *current_ptr;
	DIR *dp;
	MINODE *mip, *parent_mip;

	if(argc < 2)
	{
		set_error("rmdir: missing operand");
		return -1;
	}

	while(i < argc)
	{
		inode_number = get_inode_number(device, argv[i]);
		if(inode_number < 0)
		{
			set_error("File does not exist");
			return -1;
		}

		mip = get_minode(device, inode_number);
		if(thrown_error == TRUE)
		{
			put_minode(mip);
			return -1;
		}

		if(running->uid != SUPER_USER && 
			running->uid != mip->ip.i_uid)
		{
			set_error("You don't have permission to do that");
			put_minode(mip);
			return -1;
		}

		if(!S_ISDIR(mip->ip.i_mode))
		{
			set_error("Not a directory");
			put_minode(mip);
			return -1;
		}

		if(mip->refCount > 1)
		{
			set_error("Directory busy");
			put_minode(mip);
			return -1;
		}

		if(mip->ip.i_links_count > 2)
		{
			set_error("Directory not empty");
			put_minode(mip);
			return -1;
		}

		ip = &mip->ip;

		get_block(device, ip->i_block[0], buf);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		current_ptr = buf;
		dp = (DIR*)buf;

		my_ino = dp->inode;

		current_ptr += dp->rec_len;
		dp = (DIR*)current_ptr;

		parent_ino = dp->inode;

		parent_mip = get_minode(device, parent_ino);

		for(j = 0; j < NUM_DIRECT_BLOCKS && ip->i_block[j] != 0; j++)
		{
			deallocate_block(device, ip->i_block[j]);
			if(thrown_error == TRUE)
			{        	
				return -1;
			}
		}

		deallocate_inode(device, my_ino);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		remove_dir_entry(parent_mip, my_ino);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		parent_mip->ip.i_links_count--;
		parent_mip->ip.i_atime = time(0L);
		parent_mip->ip.i_mtime = time(0L);
		parent_mip->dirty = TRUE;

		put_minode(parent_mip);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		put_minode(mip);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		i++;
	}
	return 0;
}