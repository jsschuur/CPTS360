#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <limits.h>

#include "input.h"
#include "shell.h"

int main(int argc, char* argv[], char *env[]) 
{
	char **PATHS = parse(getenv("PATH"), ":");
	char *line = NULL;

	while(1)
	{

		int pid, status;
		free(line);

		do{

			printf("jssh %c : ", '%');

		} while((line = get_input()) == NULL);

		if(strncmp(line, "exit", strlen("exit")) == 0)
			break;

		if(strncmp(line, "cd", strlen("cd")) == 0 && (line[2] == ' ' || line[2] == 0))
		{
			my_cd(line + strlen("cd "));
			continue;
		}

		pid = fork();

		if(pid < 0)
		{
			perror("main.c: fork failed");
		}

		if(pid)
		{
			pid = wait(&status);
			printf("%d died: exit status: %04x\n", pid, status);
		}

		else
		{
			process_command(line, PATHS, env);
		}
	}
	free(line);
	free_array(PATHS);
	return 0;
}
