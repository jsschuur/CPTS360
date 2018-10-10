


typedef struct node
{
	char name[64]; //name of node
	char type; //'D' for directory, 'F' for file
	struct node *childPtr, *siblingPtr, *parentPtr;

}NODE;
