#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

char *disk = "mydisk";

#define BLKSIZE 1024

char buf[BLKSIZE];
int fd;

void get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

void gd()
{
	get_block(fd, 1, buf);
	
	gp = (GD*)buf;
	sp = (SUPER *)buf;

	if(sp->s_magic != 0xEF53)
	{
		printf("Not an EXT2 FS\n");
		exit(1);
	}

	printf("bmap       =  %u\n", gp->bg_block_bitmap);
    printf("imap       =  %u\n", gp->bg_inode_bitmap);
    printf("inode_table        =  %u\n", gp->bg_inode_table);
    printf("free_blocks_count  =  %u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count  =  %u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count    =  %u\n", gp->bg_used_dirs_count);

}


int main(int argc, char* argv[])
{
	if (argc > 1)
	{
    	disk = argv[1];
	}
  	fd = open(disk, O_RDONLY);
  	if (fd < 0)
  	{
  		printf("open failed\n");
    	exit(1);
  	}
  gd();
}