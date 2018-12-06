#define _DEFAULT_SOURCE

#include <string.h>
#include <libgen.h>
#include <time.h>
#include <sys/stat.h>

#include "../cmd.h"
#include "../utils/search.h"
#include "../utils/readwrite.h"
#include "../utils/error_manager.h"

extern PROC *running;

int js_link(int argc, char *argv[])
{
	int old_inode, parent_inode, i, device = running->cwd->dev;
	MINODE *old_mip, *parent_mip;

	char *path, *child, *parent;

	if(argc != 3)
	{
		set_error("link <oldfile> <newfile>");
		return -1;
	}


	old_inode = get_inode_number(argv[1]);
	if(old_inode < 0)
	{
		set_error("File does not exist");
		return -1;
	}	

	old_mip = get_minode(device, old_inode);

	if(S_ISDIR(old_mip->ip.i_mode))
	{
		put_minode(old_mip);
		set_error("Cannot link to directory");
		return -1;
	}

	path = strdup(argv[2]);
	if(check_null_ptr(path))
	{
		return -1;
	}

	child = basename(path);
	parent = dirname(path);

	parent_inode = get_inode_number(parent);
	if(parent_inode < 0)
	{
		put_minode(old_mip);
		set_error("File does not exist");
		return -1;
	}

	parent_mip = get_minode(device, parent_inode);	

	if(!S_ISDIR(parent_mip->ip.i_mode))
	{
		put_minode(parent_mip);
		put_minode(old_mip);
		set_error("Not a directory");
		return -1;
	}

	if(get_inode_number(argv[2]) > 0)
	{
		put_minode(parent_mip);
		put_minode(old_mip);
		set_error("File already exists");
		return -1;
	}

	enter_dir_entry(parent_mip, old_inode, child);

	old_mip->ip.i_links_count++;
	old_mip->dirty = TRUE;

	put_minode(old_mip);
	put_minode(parent_mip);


	return 0;
}