#include <stdlib.h>

#include "../cmd.h"

#include "../utils/search.h"
#include "../utils/readwrite.h"
#include "../utils/error_manager.h"


extern PROC *running;

int js_chmod(int argc, char *argv[])
{
	int ino, device = running->cwd->dev;
	MINODE *mip;
	INODE *ip;
	long int new_mode;

	if(argc < 2)
	{
		set_error("chmod: missing operand\n");
		return -1;
	}

	ino = get_inode_number(argv[1]);
	if(ino < 0)
	{       
		set_error("File does not exist"); 	
		return -1;
	}

	mip = get_minode(device, ino);

	if(running->uid != mip->ip.i_uid && running->uid != SUPER_USER)
	{
		put_minode(mip);
		set_error("You don't have permission to do that\n");
		return -1;
	}

	ip = &mip->ip;

	new_mode = strtol(argv[2], NULL, 8);

	ip->i_mode = (ip->i_mode & 0xF000) | new_mode;

	mip->dirty = TRUE;
	put_minode(mip);

	return 0;
}