#include <stdio.h>
#include <stdlib.h>

int *FP;
typedef unsigned int u32;

int BASE = 10;
int BASE_OCTAL = 8;
int BASE_HEX = 16;
char *ctable = "0123456789ABCDEF";

//c, 

int rpu(u32 x)
{  
    char c;
    if (x){
       c = ctable[x % BASE];
       rpu(x / BASE);
       putchar(c);
    }
}
int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

void prints(char *s)
{
  for(int i = 0; s[i] != '\0'; i++)
  {
    putchar(s[i]);
  }
}
void printd(int x)
{
  if(x == 0) putchar('0');
  else{
    if(x < 0) { putchar('-'); x *= -1; }
    rpu(x);
  }
}

int rpuhex(u32 x)
{
    char c;
    if (x){
       c = ctable[x % BASE_OCTAL];
       rpu(x / BASE_OCTAL);
       putchar(c);
    }
}
void printx(u32 x)
{
  (x==0)? putchar('0') : rpuhex(x);
  putchar(' ');
}

int rpuoctal(u32 x)
{
    char c;
    if (x){
       c = ctable[x % BASE_HEX];
       rpu(x / BASE_HEX);
       putchar(c);
    }
}
void printo(u32 x)
{
  (x==0)? putchar('0') : rpuoctal(x);
  putchar(' ');
}

void myprintf(char *fmt, ...)
{
  
}


int main(int argc, char *argv[ ], char *env[ ])
{
  int a,b,c;
  printf("enter main\n");
  
  printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
  printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);

//(1). Write C code to print values of argc and argv[] entries CHECK
  printf("argc value is: %d\n", argc);
  printf("argv[] values are: ");

  for(int i = 0; i < argc; i++)
  {
    printf("%s, ",argv[i]);
  }
  printf("\n");
  a=1; b=2; c=3;



  A(a,b);
  printf("exit main\n");
}

int A(int x, int y)
{
  int d,e,f;
  printf("enter A\n");
  // PRINT ADDRESS OF d, e, f
  d=4; e=5; f=6;
  B(d,e);
  printf("exit A\n");
}

int B(int x, int y)
{
  int g,h,i;
  printf("enter B\n");
  // PRINT ADDRESS OF g,h,i
  g=7; h=8; i=9;
  C(g,h);
  printf("exit B\n");
}

int C(int x, int y)
{
  int u, v, w, i, *p;

  printf("enter C\n");
  // PRINT ADDRESS OF u,v,w,i,p;
  u=10; v=11; w=12; i=13;


  FP = (int *)getebp();

//(2). Write C code to print the stack frame link list.
  p = FP;
  printf("FP -> ");
  while(p != 0)
  {
    printf("%8x -> ", p);
    p = *p;
  }
  printf("%d\n", p);

  p = (int *)&p;
  
//(3). Print the stack contents from p to the frame of main()
     //YOU MAY JUST PRINT 128 entries of the stack contents.
  FP = p;
  i = 0;
  while(i < 10 && FP != 0) //not 128 lol
  {
    printf("%8x\n", FP);
    FP++;
    i++;
  }

//(4). On a hard copy of the print out, identify the stack contents
     //as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
}

