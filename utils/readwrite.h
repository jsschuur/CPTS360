#ifndef _READWRITE_H_
#define _READWRITE_H_

#include <unistd.h>
#include <stdlib.h>

#include "type.h"
#include "error_manager.h"
#include "../utils/bitmap.h"



/*-----------------------------------------
Function: get_block
Use: loads data from block 'block' from 
	 device 'dev.' Stores results in 'buf' 
Throws errors?: -native
-----------------------------------------*/
int get_block(int dev, int block, char buf[]);

/*-----------------------------------------
Function: put_block
Use: writes data in 'buf' to block 'block'
	 on device 'dev' 
Throws errors?: -native
-----------------------------------------*/
int put_block(int dev, int block, char buf[]);


/*-----------------------------------------
Function: get_minode
Use: searches memory inodes, returns memory
	 inode or loads inode from disk
	 into memory if inode not found
Throws errors?: -get_block
-----------------------------------------*/
MINODE *get_minode(int dev, int ino);


/*-----------------------------------------
Function: put_minode
Use: checks if mip is dirty/refCount < 0,
	 if so writes mip information back to
	 disk
Throws errors?: -put_block
-----------------------------------------*/
int put_minode(MINODE *mip);

/*-----------------------------------------
Function: enter_dir_entry
Use: enters a new directory entry into the
	 file pointed at by 'parent_mip'
Throws errors?: -put_block
				-get_block
				-allocate_inode
-----------------------------------------*/
int enter_dir_entry(MINODE *parent_mip, int inode_number, char *name);

int remove_dir_entry(MINODE *parent_mip, int inode_number);

int remove_dir_entry_by_name(MINODE *parent_mip, char *name);

#endif