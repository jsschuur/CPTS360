#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "parse.h"
#include <sys/wait.h>

int my_cd(char *arg1);
void process_command(char *line, char **PATHS, char *env[]);
void free_array(char **arr);

#endif