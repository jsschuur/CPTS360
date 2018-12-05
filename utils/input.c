#include "input.h"

int get_input(char **buf)
{
	int c, i = 0, size = INITIAL_BUF_SIZE;;

	(*buf) = (char *)malloc(size * sizeof(char));
	
	if(check_null_ptr((*buf)) == TRUE)
	{
		return -1;
	}

	(*buf)[0] = 0;

	while(TRUE)
	{
		c = getchar();

		if((i + 1) >= size)
		{
			size *= 2;
			(*buf) = (char *)realloc((*buf), size * sizeof(char));

			if(check_null_ptr((*buf)))
			{
				return -1;
			}
		}

		if(c == ' ')
		{
			while(c == ' ')
			{
				c = getchar();
			}
			
			if(i != 0 && c != '\n')
			{
				(*buf)[i] = ' ';
				i++;
			}
		}

		if(c == '\n')
		{
			(*buf)[i] = 0;
			if((*buf)[0] == '\0')
			{
				(*buf) = NULL;
			}
			return 0;
		}
		(*buf)[i] = c;
		i++;
	}
}