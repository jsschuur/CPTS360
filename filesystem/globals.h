#include "node.h"
#include <stdio.h>

NODE *root, *cwd; //root and current working directory
char line[128]; //user input
char command[16]; //command
char pathname[64]; //pathname
char dname[64]; //directory name
char bname[64]; //base name
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
"reload", "save", "menu", "quit", 0};
char pwdStr[128];
FILE *treefile;