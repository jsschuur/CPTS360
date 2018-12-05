#define _DEFAULT_SOURCE

#include "../cmd.h"
#include "../utils/search.h"
#include <time.h>
#include <string.h>
#include <libgen.h>

extern PROC *running;

int js_rm(int argc, char *argv[])
{

	int i = 1, j, inode_number, device = running->cwd->dev, parent_ino;
	char buf[BLOCK_SIZE], *current_ptr, *path, *parent, *child;
	DIR *dp;
	MINODE *mip, *parent_mip;

	if(argc < 2)
	{
		set_error("rm: missing operand");
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

		if(S_ISDIR(mip->ip.i_mode))
		{
			set_error("Cannot rm a directory");
			put_minode(mip);
			return -1;
		}

		if(mip->refCount > 1)
		{
			set_error("File busy");
			put_minode(mip);
			return -1;
		}

		for(j = 0; j < NUM_DIRECT_BLOCKS && mip->ip.i_block[j] != 0; j++)
		{
			deallocate_block(device, mip->ip.i_block[j]);
			if(thrown_error == TRUE)
			{        	
				return -1;
			}
		}

		deallocate_inode(device, inode_number);
		if(thrown_error == TRUE)
		{        	
			return -1;
		}

		path = strdup(argv[i]);
		if(check_null_ptr(path))
		{
			return -1;
		}

		child = basename(path);
		parent = dirname(path);

		parent_ino = get_inode_number(device, parent);
		if(thrown_error == TRUE)
		{
			put_minode(mip);
			return -1;
		}

		parent_mip = get_minode(device, parent_ino);
		if(thrown_error == TRUE)
		{
			put_minode(mip);
			return -1;
		}

		ip = &mip->ip;

		remove_dir_entry(parent_mip, inode_number);
		if(thrown_error == TRUE)
		{   
			put_minode(mip);     	
			return -1;
		}

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