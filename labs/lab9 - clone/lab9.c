#include <conf.h>
#include <kernel.h>


extern  SYSCALL clone(char name[], int no_of_params,   int param1, ...);

void prA(int n, int ch, ...)
{
  int i;
  int *iptr;
  char name[20];
  int count = 0;
  int a, b, c;

  iptr = &ch;

        for(i=0; i< n ; i++)
        {
                putc(CONSOLE, *iptr);
                iptr++;
        } // for
        putc(CONSOLE, '\n');
        count++;
        sprintf(name,"proc %c", ch);
        a = ch+1;
        b = ch+2;
        c = ch+3;
        sleep(3);
        clone(name, 4, 3, a, b, c);
} // PrA
   
int xmain()
{
        resume( create(prA, INITSTK, INITPRIO, "proc 1", 2, 1, 'A') );
        return 0;
} // xmain
