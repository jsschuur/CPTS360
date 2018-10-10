#include "input.h"


char *get_input()
{
	int size = 16;
	char *buf = NULL;
	int i = 0;

	buf = (char*)malloc(size * sizeof(char));

	if(buf == NULL)
	{
		perror("input.c: buf malloc error");
	}
	buf[0] = 0;

	while(1)
	{
		int c = getchar();

		if((i + 1) >= size)
		{
			size *= 2;
			buf = (char*)realloc(buf, size * sizeof(char));
			if(buf == NULL)
			{
				perror("input.c: buf realloc error");
			}
		}

		if(c == ' ')
		{
			while(c == ' ')
				c = getchar();

			if(i != 0 && c != '\n')
			{
				buf[i] = ' ';
				i++;
			}
		}

		if(c == '\n')
		{
			if(buf[0] == '\0')
			{
				return NULL;
			}
			buf[i] = 0;
			return buf;
		}

		buf[i] = c;
		i++;
	}
}