#include <time.h>

#include "../cmd.h"
#include "../utils/search.h"
#include "../utils/readwrite.h"
#include "../utils/error_manager.h"

extern PROC *running;

int js_touch(int argc, char *argv[])
{
	int device = running->cwd->dev, i, target_inode;
	MINODE *mip;

	for(i = 1; i < argc; i++)
	{
		target_inode = get_inode_number(argv[i]);
		if(target_inode < 0)
		{
			char *my_argv[] = { "creat", argv[i], NULL };
			js_creat(2, my_argv);
			if(thrown_error == TRUE)
			{
				return -1;
			}
		}
		else
		{
			mip = get_minode(device, target_inode);
			if(thrown_error == TRUE)
			{
				return -1;
			}

			mip->ip.i_atime = time(0L);
			mip->dirty = TRUE;

			put_minode(mip);
			if(thrown_error == TRUE)
			{
				return -1;
			}
		}
	}
	return 0;
}