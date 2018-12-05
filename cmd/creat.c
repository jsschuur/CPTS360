#define _DEFAULT_SOURCE

#include <string.h>
#include <libgen.h>
#include <time.h>

#include "../cmd.h"
#include "../utils/search.h"

extern MINODE *root;
extern PROC *running;
extern int block_size;

int creat_file(MINODE *parent_mip, char *name)
{
	int inode_number, block_number, device = parent_mip->dev, i;
	char buf[BLOCK_SIZE], *current_ptr;
	MINODE *mip;
	INODE *ip;
	DIR *dp;

	inode_number = allocate_inode(device);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	mip = get_minode(device, inode_number);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	ip = &mip->ip;

	ip->i_mode = 0100664;
	ip->i_uid  = running->uid;	
 	ip->i_gid  = running->gid;	
  	ip->i_size = 0;		
  	ip->i_links_count = 1;	       
	ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  
	ip->i_blocks = 0;                	
	
	for(i = 0; i < 15; i++)
	{
		ip->i_block[i] = 0;
	}

	mip->dirty = TRUE;
	
	put_minode(mip);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	enter_dir_entry(parent_mip, inode_number, name);

	return inode_number;
}

int js_creat(int argc, char *argv[])
{
	int i, parent_ino, device = running->cwd->dev;
	char *pathname, *parent = NULL, *child = NULL;
	MINODE *mip, *parent_mip;

	if(argc < 2)
	{
		set_error("creat: missing operand");
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		pathname = strdup(argv[i]);
		if(check_null_ptr(pathname))
		{
			return -1;
		}

		if(pathname[0] == '/')
		{
			mip = root;
		}
		else
		{
			mip = running->cwd;
		}
		child = basename(pathname);
		parent = dirname(pathname);
		
		
		
		parent_ino = get_inode_number(device, parent);
		if(parent_ino < 0)
		{
			set_error("File does not exist");
			return -1;
		}

		parent_mip = get_minode(device, parent_ino);
		if(thrown_error == TRUE)
		{
			put_minode(parent_mip);
			free(pathname);
			return -1;
		}

		if(!S_ISDIR(parent_mip->ip.i_mode))
		{
			put_minode(parent_mip);
			free(pathname);
			set_error("Not a dir\n");
			return -1;
		}
		if(get_inode_number(device, argv[i]) > 0)
		{
			put_minode(parent_mip);
			free(pathname);
			set_error("File already exists");
			return -1;
		}

		creat_file(parent_mip, child);

		parent_mip->ip.i_atime = time(0L);
		parent_mip->dirty = TRUE;

		put_minode(parent_mip);
		free(pathname);
	}
	return 0;
}