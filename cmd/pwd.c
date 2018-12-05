#include "../cmd.h"
#include "../utils/search.h"

extern PROC *running;
extern MINODE *root;

int rpwd(MINODE *mip, char **path, int size)
{
	int device = mip->dev, i, my_ino, parent_ino;
	DIR *dp;
	MINODE *parent_mip;
	char buf[BLOCK_SIZE], *cp, *my_name;

	if(mip == root)
	{
		if(thrown_error == TRUE)
		{        	
			return -1;
		}
		strcat(*path, "/");
		return 0;
	} 

	get_block(device, mip->ip.i_block[0], buf);
	if(thrown_error == TRUE)
	{        	
		return -1;
	}

	cp = buf;
	dp = (DIR*)buf;

	my_ino = dp->inode;

	cp += dp->rec_len;
	dp = (DIR*)cp;

	parent_ino = dp->inode;

	parent_mip = get_minode(device, parent_ino);
	if(thrown_error == TRUE)
	{        	
		return -1;
	}

	rpwd(parent_mip, path, size);
	if(thrown_error == TRUE)
	{
		put_minode(mip);
		return -1;
	}

	find_my_name(parent_mip, my_ino, &my_name);
	if(thrown_error == TRUE)
	{        	
		return -1;
	}

	while(strlen(*path) + strlen(my_name) + 3 > size)
	{
		size *= 2;

		(*path) = (char*)realloc(*path, size * sizeof(char));
		if(check_null_ptr(*path))
		{
			put_minode(mip);
			free(my_name);
			return -1;
		}
	}

	strcat(*path, my_name);
	free(my_name);

	if(mip != running->cwd)
	{
		strcat(*path, "/");
	}

	put_minode(parent_mip);
	if(thrown_error == TRUE)
	{        	
		return -1;
	}
	return 0;
}

int js_pwd(int argc, char *argv[])
{
	int device = running->cwd->dev, size = 64;
	char *buf;

	buf = (char*)malloc(size * sizeof(char*));
	if(check_null_ptr(buf))
	{
		return -1;
	}

	buf[0] = '\0';


	rpwd(running->cwd, &buf, size);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	printf("%s\n", buf);
	return 0;
}