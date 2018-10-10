#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int initialize()
{
	root = (NODE*)malloc(sizeof(NODE));

	if(root)
	{
		strcpy(root->name, "/");
		root->parentPtr = root->siblingPtr = root;
		root->type = 'D';
		cwd = root;
		return 1;
	}
	return -1;
}

int findCommand(char *command)
{
	int i = 0;
	while(cmd[i]) {
		if(!strcmp(command, cmd[i])) return i;
		i++;
	}
	return -1;
}

int splitPathNames(char *pathname)
{
	char *split = strrchr(pathname, '/');
 	
 	if(split == "/")
 	{
 		printf("Cannot use name '/' for new directory\n");
 		return -1;
 	}

 	if(split == NULL)
 	{
 		strcpy(bname, pathname);
 		dname[0] = '\0';
 	}
 	else
 	{
 		strcpy(bname, (split + 1));
 		*split = 0;
 		strcpy(dname, pathname);
 	}
 	return 1;
}

NODE *findNode(char *pathname, NODE *start, int needDirectory)
{
	NODE *current = start;
	char *buf = (char*)malloc(strlen(pathname) + 1);
	strcpy(buf, pathname);

	char *tok = strtok(buf, "/");

	if(tok == NULL) return current;

	while(tok)
	{
		current = current->childPtr;
		while(current)
		{
			if(strcmp(current->name, tok) == 0)
			{
				if(needDirectory == 1 && current->type == 'F')
				{
					printf("[%c  %s] is a file, no such directory exits\n", current->type, current->name);
					return NULL;
				}
				break;
			}
			current = current->siblingPtr;
		}

		if(current == NULL)
		{
			return NULL;
		}
		tok = strtok(NULL, "/");
	}
	return current;
}

NODE *findLastSibling(NODE *parent)
{
	NODE *n = parent->childPtr;
	if(!n) return NULL;
	while(n->siblingPtr)
	{
		n = n->siblingPtr;
	}
	return n;
}

int mkdir(char *pathname)
{

	if(pathname[0] == '\0')
	{
		printf("mkdir: missing operand\n");
		return -1;
	}



	NODE *directory;

	if(splitPathNames(pathname) == -1)
	{
		return -1;
	}


	//ABSOLUTE
	if(pathname[0] == '/')
	{
		directory = findNode((dname + 1), root, 1);
	}
	//RELATIVE
	else
	{
		directory = findNode(dname, cwd, 1);
	}	
	if(directory == NULL)
	{
		return -1;
	}

	//Go through each sibling of the directory node and check for duplicate naming
	NODE *duplicateCheck = directory->childPtr;

	while(duplicateCheck)
	{
		if(strcmp(duplicateCheck->name, bname) == 0)
		{
			printf("Duplicate directory names\n");
			return -1;
		}
		duplicateCheck = duplicateCheck->siblingPtr;
	}


	NODE *newNode = (NODE*)malloc(sizeof(NODE));

	strcpy(newNode->name, bname);
	newNode->type = 'D';

	NODE *lastSibling = findLastSibling(directory);

	newNode->parentPtr = directory;

	if(lastSibling == NULL)
	{
		directory->childPtr = newNode;
	}
	else
	{
		lastSibling->siblingPtr = newNode;
	}	
	


	newNode->childPtr = NULL;
	newNode->siblingPtr = NULL;
	dname[0] = '\0';
	bname[0] = '\0';

	return 1;
}
int rmdir(char *pathname)
{
	if(pathname[0] == '\0')
	{
		printf("rmdir: missing operand\n");
		return -1;
	}

	NODE *n;
	if(pathname[0] == '/')
	{
		n = findNode((pathname + 1), root, 0);
	}
	else
	{
		n = findNode(pathname, cwd, 0);
	}

	if(n)
	{
		if(n->type == 'D')
		{
			if(n->childPtr == NULL)
			{
				//if n is direct descendant, just move parent's child ptr
				if(n->parentPtr->childPtr == n) n->parentPtr->childPtr = n->siblingPtr;

				//if n is not direct descendant, must move closest sibling's sibling pointer over one
				else
				{
					NODE *closestSibling = n->parentPtr->childPtr;
					while(closestSibling->siblingPtr != n)
					{
						closestSibling = closestSibling->siblingPtr;
					}
					closestSibling->siblingPtr = n->siblingPtr;
				}

				free(n);
			}
			else
			{
				printf("directory not empty\n");
				return -1;
			}
		}
		else
		{
			printf("Node not a directory\n");
			return -1;
		}
	}
	else
	{
		printf("Node does not exist\n");
		return -1;
	}
}
int ls(char *pathname)
{
	NODE *n;
	if(*pathname)
	{
		if(pathname[0] == '/')
		{
			n = findNode((pathname + 1), root, 0);
		}
		else
		{
			n = findNode(pathname, cwd, 0);
		}
		if(n)
		{
			if(n->type == 'F')
			{
				printf("Node not a directory\n");
				return -1;
			}
			n = n->childPtr;
		}
		else
		{
			printf("Node not found\n");
			return -1;
		}
	}
	else n = cwd->childPtr;

	int i = 0;


	while(n)
	{
		printf("[%c  %s]", n->type, n->name);
		n = n->siblingPtr;
	}
	printf("\n");
}
int cd(char *pathname)
{
	NODE *n;
	if(strcmp(pathname, "..") == 0)
	{ 
		cwd = cwd->parentPtr;
	}
	else if(*pathname)
	{
		if(pathname[0] == '/')
		{
			n = findNode((pathname + 1), root, 0);
		}
		else
		{
			n = findNode(pathname, cwd, 0);
		}
		if(n)
		{
			if(n->type == 'D')
			{
				cwd = n;
			}
			else
			{
				printf("Node not a directory\n");
				return -1;
			}
		}
		else
		{
			printf("directory not found\n");
			return -1;
		}
	}
	else cwd = root;
}
int pwdRecurs(NODE *node)
{
	if(strcmp(node->parentPtr->name, "/")) pwdRecurs(node->parentPtr);
	strcat(pwdStr, "/");
	strcat(pwdStr, node->name);
}
int pwd()
{
	pwdStr[0] = '\0';
	if(cwd == root)
	{
		printf("/\n");
		return 1;
	}
	pwdRecurs(cwd);
	printf("%s\n", pwdStr);
}
int creat(char *pathname)
{
	if(pathname[0] == '\0')
	{
		printf("creat: missing operand\n");
		return -1;
	}



	NODE *directory;

	if(splitPathNames(pathname) == -1)
	{
		return -1;
	}


	//ABSOLUTE
	if(pathname[0] == '/')
	{
		directory = findNode((dname + 1), root, 0);
	}
	//RELATIVE
	else
	{
		directory = findNode(dname, cwd, 0);
	}	
	if(directory == NULL)
	{
		printf("Node not found\n");
		return -1;
	}

	//Go through each sibling of the directory node and check for duplicate naming
	NODE *duplicateCheck = directory->childPtr;

	while(duplicateCheck)
	{
		if(strcmp(duplicateCheck->name, bname) == 0)
		{
			printf("Duplicate file names\n");
			return -1;
		}
		duplicateCheck = duplicateCheck->siblingPtr;
	}


	NODE *newNode = (NODE*)malloc(sizeof(NODE));

	strcpy(newNode->name, bname);
	newNode->type = 'F';

	NODE *lastSibling = findLastSibling(directory);

	newNode->parentPtr = directory;

	if(lastSibling == NULL)
	{
		directory->childPtr = newNode;
	}
	else
	{
		lastSibling->siblingPtr = newNode;
	}	
	


	newNode->childPtr = NULL;
	newNode->siblingPtr = NULL;
	dname[0] = '\0';
	bname[0] = '\0';
}
int rm(char *pathname)
{
	if(pathname[0] == '\0')
	{
		printf("rm: missing operand\n");
		return -1;
	}

	NODE *n;
	if(pathname[0] == '/')
	{
		n = findNode((pathname + 1), root, 0);
	}
	else
	{
		n = findNode(pathname, cwd, 0);
	}

	if(n)
	{
		if(n->type == 'F')
		{
			//if n is direct descendant, just move parent's child ptr
			if(n->parentPtr->childPtr == n) n->parentPtr->childPtr = n->siblingPtr;

			//if n is not direct descendant, must move closest sibling's sibling pointer over one
			else
			{
				NODE *closestSibling = n->parentPtr->childPtr;
				while(closestSibling->siblingPtr != n)
				{
					closestSibling = closestSibling->siblingPtr;
				}
				closestSibling->siblingPtr = n->siblingPtr;
			}
			free(n);
		}
		else
		{
			printf("Node not a file\n");
			return -1;
		}
	}
	else
	{
		printf("Node does not exist\n");
		return -1;
	}
}

int reload()
{
	treefile = fopen("treefile.txt", "r");
	char n[128];
	while(fgets(n, sizeof(n), treefile))
	{
		n[strlen(n) - 1] = 0;
		switch(n[0])
		{
			case 'D':
				mkdir(n + 3);
			break;
			case 'F':
				creat(n + 3);
			break;
		}
	}


}

void saveRecurs(NODE *n)
{
	if(n == NULL) return;
	if(n != root)
	{
		pwdStr[0] = '\0';
		pwdRecurs(n);
		fputc(n->type, treefile);
		fputs("  ", treefile);
		fputs(pwdStr, treefile);
		fputs("\n", treefile);
		saveRecurs(n->childPtr);
		saveRecurs(n->siblingPtr);
	}
}
int save()
{
	treefile = fopen("treefile.txt", "w");
	saveRecurs(root->childPtr);
	fclose(treefile);
}
int menu()
{

}
int quit()
{
	save();
	exit(0);
}