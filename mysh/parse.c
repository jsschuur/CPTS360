#include "parse.h"

char **parse(char *input, char *delims)
{
	char *tok;
	char **buf;
	int size = 8;
	int i = 0;

	buf = (char**)malloc(size * sizeof(char*));

	if(buf == NULL)
	{
		perror("parce.c: malloc failure for (char**)");
		return NULL;
	}

	if((tok = strtok(input, delims)) == NULL)
		return NULL;

	while(tok)
	{
		int length = strlen(tok) + 1;
		buf[i] = (char*)malloc(length);

		if(buf[i] == NULL)
		{
			perror("parce.c: malloc failure for (char*)");
			return NULL;
		}

		strcpy(buf[i], tok);

		if((i + 1) == size)
		{
			size *= 2;

			buf = (char**)realloc(buf, size * sizeof(char*));
			if(buf == NULL)
			{
				perror("parse.c: realloc failure for (char**)");
				return NULL;
			}
		}
		tok = strtok(NULL, delims);
		i++;
	}
	buf[i] = NULL;

	return buf;
}