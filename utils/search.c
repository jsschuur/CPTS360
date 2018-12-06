#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include "search.h"

#include "get_put_block.h"
#include "readwrite.h"
#include "error_manager.h"
#include "parse.h"

extern int block_size;
extern MINODE *root;
extern PROC *running;

int search(MINODE *mip, const char *name)
{
	int i, dev = mip->dev;
	char buf[BLOCK_SIZE], entry_name_buf[BLOCK_SIZE], *current_ptr;
	INODE *ip = &mip->ip;
	DIR *dp;

	if(!S_ISDIR(ip->i_mode))
	{
		set_error("Tried to search a regular file\n");
		return -1;
	}

	//check direct block entries
	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(!ip->i_block[i])
		{
			break;
		}

		get_block(dev, mip->ip.i_block[i], buf);
		if(thrown_error == TRUE)
		{	
			return -1;
		}

		current_ptr = buf;
		dp = (DIR *)buf;

		while(current_ptr < (buf + block_size))
		{
			strncpy(entry_name_buf, dp->name, dp->name_len);
			entry_name_buf[dp->name_len] = 0;

			if(strcmp(entry_name_buf, name) == 0)
			{
				return dp->inode;
			}

			current_ptr += dp->rec_len;
			dp = (DIR *)current_ptr;
		}
	}
	return -1;
}

int does_exist(MINODE *mip, const char *name)
{
	int i, dev = mip->dev;
	char buf[BLOCK_SIZE], entry_name_buf[BLOCK_SIZE], *current_ptr;
	INODE *ip = &mip->ip;
	DIR *dp;

	if(!S_ISDIR(ip->i_mode))
	{
		set_error("Tried to search a regular file\n");
		return -1;
	}

	//check direct block entries
	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(!ip->i_block[i])
		{
			break;
		}

		get_block(dev, mip->ip.i_block[i], buf);
		if(thrown_error == TRUE)
		{	
			return -1;
		}

		current_ptr = buf;
		dp = (DIR *)buf;

		while(current_ptr < (buf + block_size))
		{
			strncpy(entry_name_buf, dp->name, dp->name_len);
			entry_name_buf[dp->name_len] = 0;

			if(strcmp(entry_name_buf, name) == 0)
			{
				return 1;
			}

			current_ptr += dp->rec_len;
			dp = (DIR *)current_ptr;
		}
	}
	return 0;
}

int get_inode_number(const char *path)
{
	int i = 1, ino, num_tokens, device = running->cwd->dev;
	char **parsed_path_tokens = NULL;

	if(strcmp(path, "/") == 0)
	{
		return ROOT_INODE;
	}

	num_tokens = parse(path, "/", &parsed_path_tokens);
	if(thrown_error == TRUE)
	{
		return -1;
	}

	//Begin search at absolute path
	if(path[0] == '/')
	{
		ino = search(root, parsed_path_tokens[0]);
	}
	//begin search at relative path
	else
	{
		ino = search(running->cwd, parsed_path_tokens[0]);
	}

	if(thrown_error == TRUE)
	{
		free_array(parsed_path_tokens);
		return -1;
	}
	
	while(i < num_tokens)
	{
		ino = search(get_minode(device, ino), parsed_path_tokens[i]);
		if(ino < 0)
		{
			free_array(parsed_path_tokens);
			return -1;
		}
		i++;
	}

	free_array(parsed_path_tokens);
	return ino;
}

int find_my_name(MINODE *parent, int my_inode_number, char **my_name)
{
	int i, device;
	char buf[BLOCK_SIZE], *current_ptr;
	INODE *ip;
	DIR *dp;

	device = parent->dev;
	ip = &parent->ip;


	//search direct block entries
	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if(ip->i_block[i] == 0)
		{
			return -1;
		}

		get_block(device, ip->i_block[i], buf);
		if(thrown_error == TRUE)
		{
			return -1;
		}

		current_ptr = buf;
		dp = (DIR*)buf;

		while(current_ptr < (buf + block_size))
		{
			if(dp->inode == my_inode_number)
			{
				*my_name = (char*)malloc((dp->name_len + 1) * sizeof(char));
				if(check_null_ptr(*my_name))
				{
					return -1;
				}
				strncpy(*my_name, dp->name, dp->name_len);
				(*my_name)[dp->name_len] = 0;
				return 0;
			}
			current_ptr += dp->rec_len;
			dp = (DIR*)current_ptr;
		}
	}
	return -1;
}