#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "type.h"
#include "readwrite.h"
#include "error_manager.h"
#include "parse.h"
#include <sys/stat.h>

extern int block_size;
extern MINODE *root;
extern PROC *running;

/*-----------------------------------------
Function: search
Use: searches mip for file 'name', returns
	 ino if found, -1 if not
Throws errors?: -native
				-get_block
-----------------------------------------*/
int search(MINODE *mip, const char *name);

/*-----------------------------------------
Function: does_exist
Use: searches mip for file 'name', returns
	 0 if found, 1 if not, identical to search
	 in implementation
Throws errors?: -native
				-get_block
-----------------------------------------*/
int does_exist(MINODE *mip, const char *name);

/*-----------------------------------------
Function: get_inode_number
Use: searches device 'dev' for file 'path'
Throws errors?: -native
				-parse
				-search
-----------------------------------------*/
int get_inode_number(int dev, const char *path);

/*-----------------------------------------
Function: find_my_name
Use: searches 'parent' for dir entry who's
	 inode == my_inode_number, stores it's
	 name in 'my_name'
Throws errors?: -get_block
-----------------------------------------*/
int find_my_name(MINODE *parent, int my_inode_number, char **my_name);

#endif