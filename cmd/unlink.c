#define _DEFAULT_SOURCE

#include "../cmd.h"
#include "../utils/search.h"

#include <time.h>
#include <string.h>
#include <libgen.h>

extern PROC *running;

int js_unlink(int argc, char *argv[])
{
	int device = running->cwd->dev, inode_number, parent_ino, i, j;
	char *path, *parent, *child;
	MINODE *mip, *parent_mip;

	if(argc < 2)
	{
		set_error("unlink: missing operand");
		return -1;
	}

	for(i = 1; i < argc; i++)
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

		if(S_ISDIR(mip->ip.i_mode))
		{
			put_minode(mip);
			set_error("unlink: cannot unlink dir");
			return -1;
		}

		if(mip->refCount > 1)
		{
			set_error("File busy");
			put_minode(mip);
			return -1;
		}

		mip->ip.i_links_count--;
		mip->dirty = TRUE;

		if(mip->ip.i_links_count == 0 && !S_ISLNK(mip->ip.i_mode))
		{
			//assume direct blocks
			for(j = 0; j < NUM_DIRECT_BLOCKS && mip->ip.i_block[j] != 0; j++)
			{
				deallocate_block(device, mip->ip.i_block[j]);
				if(thrown_error == TRUE)
				{	 
					put_minode(mip);       	
					return -1;
				}
			}
			deallocate_inode(device, inode_number);	
		}
		//if symbolic link, deallocate inode
		else if(S_ISLNK(mip->ip.i_mode))
		{
			deallocate_inode(device, inode_number);
		}

		path = strdup(argv[i]);
		if(check_null_ptr(path))
		{
			return -1;
		}

		child = basename(path);
		parent = dirname(path);

		parent_ino = get_inode_number(device, parent);
		if(parent_ino < 0)
		{
			set_error("File does not exist");
			return -1;
		}

		parent_mip = get_minode(device, parent_ino);

		remove_dir_entry_by_name(parent_mip, child);
		
		put_minode(parent_mip);
		put_minode(mip);

	}
	return 0;
}