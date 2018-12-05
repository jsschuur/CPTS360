#include "../cmd.h"
#include "../utils/search.h"

extern PROC *running;

int js_chown(int argc, char *argv[])
{
	int device = running->cwd->dev, ino;
	MINODE *mip;
	INODE *ip;
	long int new_own;

	if(argc < 2)
	{
		set_error("chown: missing operand\n");
		return -1;
	}

	ino = get_inode_number(device, argv[1]);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	mip = get_minode(device, ino);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	if(running->uid != mip->ip.i_uid && running->uid != SUPER_USER)
	{
		set_error("You don't have permission to do that\n");
		return -1;
	}

	ip = &mip->ip;

	new_own = strtol(argv[2], NULL, 10);
	ip->i_uid = new_own;

	mip->dirty = TRUE;

	put_minode(mip);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	return 0;
}