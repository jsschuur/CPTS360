
#include "../cmd.h"

#include "../utils/error_manager.h"
#include "../utils/readwrite.h"

extern MINODE minodes[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;

extern int fd, dev;
extern int block_bitmap, inode_bitmap, inode_table_block;
extern int nblocks, ninodes, block_size, inode_size, inodes_per_block, iblk;

int mount_root()
{
	char buf[BLOCK_SIZE];
	SUPER *sp;
	GD *gp;

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
  	get_block(dev, SUPER_BLOCK_OFFSET, buf);

  	if(thrown_error == TRUE)
  	{
		return -1;
  	}

  	sp = (SUPER*)buf;

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
	get_block(dev, GD_BLOCK_OFFSET, buf);

	if(thrown_error == TRUE)
	{
		return -1;
	}

	gp = (GD *)buf;

	block_bitmap = gp->bg_block_bitmap;
	inode_bitmap = gp->bg_inode_bitmap;
	inode_table_block = gp->bg_inode_table;
	inodes_per_block = block_size / inode_size;

	root = get_minode(dev, ROOT_INODE);
	root->refCount++;
	if(thrown_error == TRUE)
	{
		return -1;
	}

	running = &proc[0];
	running->status = READY;
	running->uid = SUPER_USER;
	running->cwd = root;
	root->refCount++;


  	return 0;  	
}
