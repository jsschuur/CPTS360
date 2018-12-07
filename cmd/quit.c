#include <stdlib.h>

#include "../utils/readwrite.h"
#include "../utils/type.h"

extern MINODE minodes[NMINODE];

int js_quit(int argc, char *argv[])
{
   int i;
   for(i = 0; i < NMINODE; i++)
   {
      if(minodes[i].refCount > 0 && minodes[i].dirty == 1)//takes the memory inodes that have references and arent dirty and makes their ref counts 1 then diposes of them
      {
         minodes[i].refCount = 1;
         put_minode(&minodes[i]);
      }
   }
   exit(0);
}
