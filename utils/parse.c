#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "error_manager.h"


int parse(const char *input, char *delimiters, char ***buf)
{
	char *tok, *copy;
	int size = INITIAL_BUF_SIZE;
	int i = 0;

	(*buf) = (char**)malloc(size * sizeof(char*));
	if(check_null_ptr((*buf)))
	{
		return -1;
	}

	copy = (char *)malloc((strlen(input) + 1) * sizeof(char));
	if(check_null_ptr(copy))
	{
		free_array(*buf);
		return -1;
	}

	strcpy(copy, input);

	tok = strtok(copy, delimiters);

	if(tok == NULL)
	{
		set_error("invalid input\n");
		free(copy);
		free_array(*buf);
		return -1;
	}

	while(tok)	
	{
		int length = strlen(tok) + 1;
		(*buf)[i] = (char*)malloc(length);

		if(check_null_ptr((*buf)[i]))
		{
			return -1;
		}

		strcpy((*buf)[i], tok);

		if((i + 1) == size)
		{
			size *= 2;

			(*buf) = (char**)realloc((*buf), size * sizeof(char*));
			if(check_null_ptr(*buf))
			{
				free(copy);
				return -1;
			}
		}
		tok = strtok(NULL, delimiters);
		i++;
	}
	(*buf)[i] = NULL;

	free(copy);
	return i;
}


void free_array(char **arr)
{
	int i = 0;
	
	if(!arr)
	{
		return;
	}

	while(arr[i])
	{
		free(arr[i]);
		i++;
	}

	free(arr);
}