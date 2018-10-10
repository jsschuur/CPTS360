#include "filesystem.c"

int initialize();

int mkdir(char *pathname);
int rmdir();
int ls();
int cd();
int pwd();
int creat();
int rm();
int reload();
int save();
int menu();
int quit();

int (*fptr[])(char*) = {(int (*) ())mkdir, rmdir, ls, cd, pwd, creat, rm, reload, save, menu, quit};
