#define _DEFAULT_SOURCE

#include <string.h>
#include <libgen.h>
#include <time.h>

#include "../cmd.h"
#include "../utils/search.h"

extern MINODE *root;
extern PROC *running;
extern int block_size;

int my_mkdir(MINODE *parent_mip, char *name)
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

	block_number = allocate_block(device);
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

	ip->i_mode = 0x41ED;
	ip->i_uid  = running->uid;	
 	ip->i_gid  = running->gid;	
  	ip->i_size = BLOCK_SIZE;		
  	ip->i_links_count = 2;	       
	ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  
	ip->i_blocks = 2;                	
	ip->i_block[0] = block_number;
	             
	for(i = 1; i < 15; i++)
	{
		ip->i_block[i] = 0;
	}

	mip->dirty = TRUE;
	
	put_minode(mip);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	get_block(device, block_number, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	current_ptr = buf;
	dp = (DIR*)buf;

	dp->inode = inode_number;
	dp->rec_len = (4 * (( 8 + strlen(".") + 3) / 4));
	dp->name_len = strlen(".");
	dp->file_type = EXT2_FT_DIR;
	dp->name[0] = '.';

	current_ptr += dp->rec_len;
	dp = (DIR*)current_ptr;

	dp->inode = parent_mip->ino;
	dp->rec_len = (block_size - (current_ptr - buf));

	dp->name_len = strlen("..");
	dp->file_type = EXT2_FT_DIR;
	dp->name[0] = '.';
	dp->name[1] = '.';

	put_block(device, block_number, buf);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	enter_dir_entry(parent_mip, inode_number, name);

	return 0;
}

int js_mkdir(int argc, char *argv[])
{
	int i, parent_ino, device = running->cwd->dev;
	char *pathname, *parent = NULL, *child = NULL;
	MINODE *parent_mip;

	if(argc < 2)
	{
		set_error("mkdir: missing operand");
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		pathname = strdup(argv[i]);
		if(check_null_ptr(pathname))
		{
			return -1;
		}

		child = basename(pathname);
		parent = dirname(pathname);
		
		
		
		parent_ino = get_inode_number(device, parent);
		if(thrown_error == TRUE)
		{
			return -1;
		}

		parent_mip = get_minode(device, parent_ino);
		if(thrown_error == TRUE)
		{
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
			set_error("File already exists\n");
			return -1;
		}

		my_mkdir(parent_mip, child);

		parent_mip->ip.i_links_count++;
		parent_mip->ip.i_atime = time(0L);
		parent_mip->dirty = TRUE;

		put_minode(parent_mip);
		free(pathname);
	}
	return 0;
}