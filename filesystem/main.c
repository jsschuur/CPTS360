#include "filesystem.h"
#include <stdio.h>



int main()
{

	fflush(stdout);
	if(initialize())
	{	
		while(1)
		{
			fgets(line, 128, stdin);
			line[strlen(line)-1] = 0;
			sscanf(line, "%s %s", command, pathname);
			int index = findCommand(command);

			if(index != -1)
			{
				int r = fptr[index](pathname);
			}
			else
			{
				printf("invalid input\n");
			}
			pathname[0] = '\0';
		}
	}


	return 0;
}