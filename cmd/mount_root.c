#include <stdlib.h>

#include "../cmd.h"

#include "../utils/error_manager.h"
#include "../utils/readwrite.h"
#include "../utils/get_put_block.h"
#include "../utils/error_manager.h"


extern MINODE minodes[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;

extern int block_bitmap, inode_bitmap, inode_table_block;
extern int nblocks, ninodes, block_size, inode_size, inodes_per_block, iblk;

extern SUPER *sp;
extern GD *gp;

int mount_root(int dev)
{
	char buf[BLOCK_SIZE];
	
	int i;
	error_message = "";
  	MINODE *mip;
  	PROC   *p;

  	for (i=0; i<NMINODE; i++)
  	{
      	mip = &minodes[i];
      	mip->refCount = 0;
      	mip->dirty = FALSE;
      	mip->dev = 0;
      	mip->ino = 0;
  	}
  	for (i=0; i<NPROC; i++)
  	{
       	p = &proc[i];
      	p->pid = i;
      	p->uid = i;
      	p->gid = 0;
      	p->cwd = NULL;
      	p->next = NULL;
      	p->cwd = 0;
  	}

  	//get and record super block
  	sp = (SUPER *)malloc(sizeof(SUPER));
  	get_block(dev, SUPER_BLOCK_OFFSET, (char*)sp);
  	if(sp->s_magic != 0xEF53)
	{
		set_error("Not ext2 filesystem\n");
		return -1;
	}

	ninodes = sp->s_inodes_count;
	nblocks = sp->s_blocks_count;
	block_size = 1024 << sp->s_log_block_size;
	inode_size = sp->s_inode_size;

	//get and record group descriptor block
	gp = (GD *)malloc(BLOCK_SIZE);
	get_block(dev, GD_BLOCK_OFFSET, (char*)gp);

	block_bitmap = gp->bg_block_bitmap;
	inode_bitmap = gp->bg_inode_bitmap;
	inode_table_block = gp->bg_inode_table;
	inodes_per_block = block_size / inode_size;

	root = get_minode(dev, ROOT_INODE);
	root->refCount++;

	running = &proc[0];
	running->status = READY;
	running->uid = SUPER_USER;
	running->cwd = root;

  	return 0;  	
}
