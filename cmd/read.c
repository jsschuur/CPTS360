#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../cmd.h"
#include "../utils/error_manager.h"
#include "../utils/file.h"

extern PROC *running;

int js_read(int argc, char *argv[])
{
	int fd, bytes, n;
	char *buf;
	OFT *ofp;
	if(argc < 3)
	{
		set_error("read <file> <bytes>");
		return -1;
	}


	if(strcmp(argv[1], "0") == 0)
	{
		fd = 0;
	}
	else
	{
		fd = atoi(argv[1]);
		if(fd == 0)
		{
			set_error("Invalid file descriptor");
			return -1;
		}
	}
	
	if(fd < 0 || fd > NFD)
	{
		set_error("Invalid file descriptor");
		return -1;
	}

	ofp = running->fd[fd];

	if(ofp == NULL)
	{
		set_error("Not an open file");
		return -1;
	}
	else if(ofp->refCount == 0)
	{
		set_error("Not an open file");
		return -1;
	}

	bytes = atoi(argv[2]);
	if(bytes == 0)
	{
		set_error("Invalid byte input");
		return -1;
	}


	buf = (char*)malloc(bytes + 1);

	n = myread(fd, buf, bytes);

	printf("Bytes read: %d\nContent: \n\n%s\n", n, buf);

	return 0;
}