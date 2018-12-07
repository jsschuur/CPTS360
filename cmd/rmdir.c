#include <time.h>
#include <sys/stat.h>
#include <libgen.h>

#include "../cmd.h"
#include "../utils/readwrite.h"
#include "../utils/search.h"
#include "../utils/bitmap.h"
#include "../utils/get_put_block.h"
#include "../utils/error_manager.h"


extern PROC *running;

int js_rmdir(int argc, char *argv[])
{

	int i = 1, j, inode_number, device = running->cwd->dev, parent_ino, my_ino;
	char buf[BLOCK_SIZE], *current_ptr, *child;
	DIR *dp;
	MINODE *mip, *parent_mip;

	if(argc < 2)
	{
		set_error("rmdir: missing operand");
		return -1;
	}

	while(i < argc)
	{
		inode_number = get_inode_number(argv[i]);
		if(inode_number < 0)
		{
			set_error("File does not exist");
			return -1;
		}
		mip = get_minode(device, inode_number);

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

		if(mip->ip.i_links_count >= 2)
		{
			set_error("Directory not empty");
			put_minode(mip);
			return -1;
		}

		//get parent inode (the second entry in i_block[0])
		get_block(mip->dev, mip->ip.i_block[0], buf);

		current_ptr = buf;
		dp = (DIR*)buf;

		my_ino = dp->inode;

		current_ptr += dp->rec_len;
		dp = (DIR*)current_ptr;

		parent_ino = dp->inode;
		parent_mip = get_minode(device, parent_ino);

		clear_blocks(mip);
		deallocate_inode(mip->dev, mip->ino);
		put_minode(mip);

		child = basename(argv[i]);

		remove_dir_entry(parent_mip, child);

		parent_mip->ip.i_links_count--;
		parent_mip->ip.i_atime = time(0L);
		parent_mip->ip.i_mtime = time(0L);
		parent_mip->dirty = TRUE;

		put_minode(parent_mip);


		i++;
	}
	return 0;
}
