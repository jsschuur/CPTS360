#define _DEFAULT_SOURCE

#include <string.h>
#include <libgen.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../cmd.h"

#include "../utils/readwrite.h"
#include "../utils/bitmap.h"
#include "../utils/search.h"
#include "../utils/get_put_block.h"
#include "../utils/error_manager.h"

extern MINODE *root;
extern PROC *running;
extern int block_size;

int my_mkdir(MINODE *parent_mip, char *name)
{
	int inode_number, block_number, device = parent_mip->dev, i;
	char buf[BLOCK_SIZE], *current_ptr;
	MINODE *mip; INODE *ip;
	DIR *dp;

	inode_number = allocate_inode(device);
	block_number = allocate_block(device);


	mip = get_minode(device, inode_number);
	ip = &mip->ip;

	mip->ip.i_mode = 0x41ED;
	mip->ip.i_uid  = running->uid;	
 	mip->ip.i_gid  = running->gid;	
  	mip->ip.i_size = BLOCK_SIZE;		
  	mip->ip.i_links_count = 2;	       
	mip->ip.i_atime = mip->ip.i_ctime = mip->ip.i_mtime = time(0L);  
	mip->ip.i_blocks = 2;                	
	mip->ip.i_block[0] = block_number;
	             
	for(i = 1; i < I_BLOCKS; i++)
	{
		mip->ip.i_block[i] = 0;
	}

	mip->dirty = TRUE;
	
	put_minode(mip);

	get_block(device, block_number, buf);

	current_ptr = buf;
	dp = (DIR*)buf;

	dp->inode = inode_number;
	dp->rec_len = 4 * (( 8 + strlen(".") + 3) / 4);
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

	enter_dir_entry(parent_mip, inode_number, name);

	return 0;
}

int js_mkdir(int argc, char *argv[])
{
	int i, parent_ino, device = running->cwd->dev;
	char *basec, *dirc, *parent = NULL, *child = NULL;
	MINODE *parent_mip;

	if(argc < 2)
	{
		set_error("mkdir: missing operand");
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		basec = strdup(argv[i]);
		dirc = strdup(argv[i]);

		child = basename(basec);
		parent = dirname(dirc);


		
		
		
		parent_ino = get_inode_number(parent);
		if(parent_ino < 0)
		{
			set_error("File does not exist");
			free(basec);
			free(dirc);
			return -1;
		}
		parent_mip = get_minode(device, parent_ino);

		if(!S_ISDIR(parent_mip->ip.i_mode))
		{
			put_minode(parent_mip);
			free(basec);
			free(dirc);
			set_error("Not a dir\n");
			return -1;
		}
		if(get_inode_number(argv[i]) > 0)
		{
			put_minode(parent_mip);
			free(basec);
			free(dirc);
			set_error("File already exists\n");
			return -1;
		}

		my_mkdir(parent_mip, child);

		parent_mip->ip.i_links_count++;
		parent_mip->ip.i_atime = time(0L);
		parent_mip->dirty = TRUE;

		put_minode(parent_mip);
		free(basec);
		free(dirc);
	}
	return 0;
}