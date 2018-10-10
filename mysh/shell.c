#include "shell.h"


void free_array(char **arr)
{
	for(int i = 0; arr[i]; i++)
	{
		free(arr[i]);
	}
	free(arr);
}

void redirect_io(char **my_argv)
{
	int i = 1;
	while(my_argv[i])
	{
		if(strcmp(my_argv[i], "<") == 0)
		{
			close(0);
			if(open(my_argv[i + 1], O_RDONLY) < 0)
				perror("shell.c:redirect_io: open() failed");
			my_argv[i] = NULL;
			return;
		}

		if(strcmp(my_argv[i], ">") == 0)
		{
			close(1);
			if(open(my_argv[i + 1], O_WRONLY | O_CREAT, 0644) < 0)
				perror("shell.c:redirect_io: open() failed");
			my_argv[i] = NULL;
			return;
		}

		if(strcmp(my_argv[i], ">>") == 0)
		{
			close(1);
			if(open(my_argv[i + 1], O_WRONLY | O_APPEND | O_CREAT, 0644) < 0)
				perror("shell.c:redirect_io: open() failed");
			my_argv[i] = NULL;
			return;
		}

		i++;
	}
}

int my_cd(char *arg1)
{
	char *new_cwd;

	if(*arg1) new_cwd = arg1;
	else new_cwd = getenv("HOME");

 	printf("Proc %d cd to %s ", getpid(), new_cwd);

	if(chdir(new_cwd) != 0)
	{
		perror("shell.c:my_cd: chdir() failed");
		return -1;
	}

	printf("succeeded\n");
	return 0;
}

void execute_command(char *line, char **PATHS, char *env[])
{
	char **my_argv = parse(line, " ");
	char *cmd = NULL;

	int i = 0;
	while(PATHS[i])
	{
		int length = strlen(PATHS[i]) + 1 + strlen(my_argv[0]);

		free(cmd);
		
		cmd = (char*)malloc(length + 1);

		strcat(cmd, PATHS[i]);
		strcat(cmd, "/");
		strcat(cmd, my_argv[0]);

		if(access(cmd, X_OK) < 0)
		{
			i++;
			continue;
		}
		else
		{
			redirect_io(my_argv);
			break;
		}

	}

	execve(cmd, my_argv, env);

	printf("Invalid Command: %s\n", my_argv[0]);
	free(cmd);
	free_array(my_argv);

	exit(0);
}

void process_command(char *line, char **PATHS, char *env[])
{
	if(strcmp(line, "") == 0)
		exit(0);
	printf("%d to process command: %s\n", getpid(), line);

	int pd[2];
	int pid, status;

	int len = strlen(line);
	char *head = strtok(line, "|");
	char *tail;

	if(strlen(head) < len)
	{
		tail = line + strlen(head) + 1;
	}
	else
	{
		tail = line + len;
	}

	if(strcmp(tail, "") == 0)
		execute_command(head, PATHS, env);

	if(pipe(pd) != 0)
		perror("shell.c:process_command: pipe() failed\n");

	pid = fork();
	if(pid < 0)
		perror("shell.c:process_command: fork() failed\n");

	if(pid)
	{
		pid = wait(&status);

		close(pd[1]);
		close(0);
		dup(pd[0]);

		process_command(tail, PATHS, env);
	}
	else
	{
		close(pd[0]);
		close(1);
		dup(pd[1]);

		execute_command(head, PATHS, env);
	}
}

