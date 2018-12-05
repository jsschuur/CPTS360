#include <time.h>
#include <sys/stat.h>

#include "../cmd.h"
#include "../utils/readwrite.h"
#include "../utils/search.h"


extern PROC *running;
extern int block_size;


void ls_file(MINODE *mip, char *name)
{
	int mode, links, uid, gid, size;
	char *time;
	static const char* Permissions = "rwxrwxrwx";
	INODE *ip = &mip->ip;

	mode   = ip->i_mode;
    links  = ip->i_links_count;
    uid    = ip->i_uid;
    gid    = ip->i_gid;
    size   = ip->i_size;

    time = ctime((time_t*)&ip->i_mtime);
    time[strlen(time) - 1] = 0;
    
    switch(mode & 0xF000) 
    {
        case 0x8000:  putchar('-');     break; // 0x8 = 1000
        case 0x4000:  putchar('d');     break; // 0x4 = 0100
        case 0xA000:  putchar('l');     break; // oxA = 1010
        default:      putchar('?');     break;
    }

    for(int i = 0; i < strlen(Permissions); i++)
    {
        putchar(mode & (1 << (strlen(Permissions) - 1 - i)) ? Permissions[i] : '-');
    }

    printf("%d %d %d %d %26s %s", links, gid, uid, size, time, name);

    if(S_ISLNK(ip->i_mode))
    {
        printf(" -> %s", (char*)ip->i_block);
    }

    printf("\n");
}
void ls_dir(MINODE *mip)
{
	char buf[BLOCK_SIZE], *cp, name[256];
	DIR *dp;
	INODE *ip = &mip->ip;
	MINODE *dir_entry_mip;
	int i, dev = mip->dev;

	for(i = 0; i < NUM_DIRECT_BLOCKS; i++)
	{
		if (ip->i_block[i] == 0)
		{
            break;
		}

        get_block(dev, ip->i_block[i], buf);
        if(thrown_error == TRUE)
		{	
			return;
		}

        cp = buf;
        dp = (DIR*)buf;

        while(cp < buf + block_size)
        {
        	//dont print '.' and '..' entries
        	if(((strncmp(dp->name, ".", dp->name_len)) != 0) && 
        		((strncmp(dp->name, "..", dp->name_len)) != 0))
        	{
        		strncpy(name, dp->name, dp->name_len);
        		name[dp->name_len] = 0;

        		dir_entry_mip = get_minode(dev, dp->inode);
        		ls_file(dir_entry_mip, name);
        		
        		put_minode(dir_entry_mip);
        	    if(thrown_error == TRUE)
				{	
					return;
				}
        	}        	   
        	cp += dp->rec_len;
        	dp = (DIR*)cp;
        }
	}
}

int js_ls(int argc, char *argv[])
{
	int ino, device = running->cwd->dev, i = 1;
	MINODE *mip;

	if(argc < 2)
	{
		ls_dir(running->cwd);
		return 0;
	}

	while(i < argc)
	{
		ino = get_inode_number(device, argv[i]);
		if(ino < 0)
		{	
			set_error("File does not exist");
			return -1;
		}

		mip = get_minode(device, ino);
		if(thrown_error == TRUE)
		{	
			put_minode(mip);
			return -1;
		}

		if(S_ISDIR(mip->ip.i_mode))
		{
			if(argc > 2)
			{
				printf("%s:\n", argv[i]);
			}
			ls_dir(mip);
		}
		else
		{
			ls_file(mip, argv[i]);
		}

		put_minode(mip);
		i++;
	}
	return 0;
}