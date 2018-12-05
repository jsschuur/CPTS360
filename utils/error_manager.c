#include "error_manager.h"

int check_null_ptr(void *ptr)
{
	if(ptr == NULL)
	{
		thrown_error = TRUE;
		error_message = strerror(errno);
		return TRUE;
	}
	return FALSE;
}

void set_error(char *message)
{
	thrown_error = TRUE;
	error_message = message;
}

void print_error()
{
	printf("%s", error_message);
	thrown_error = FALSE;
}

void ignore_error()
{
	error_message = NULL;
	thrown_error = FALSE;
}