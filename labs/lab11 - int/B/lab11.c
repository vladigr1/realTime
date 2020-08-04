#include <conf.h>
#include <kernel.h>

unsigned long int count1,count2,count3;

xmain()
{
    int Inc(), Inc1(), Pr();

    resume( create(Inc, INITSTK, INITPRIO, "proc 1", 1, &count1) );
    resume( create(Inc, INITSTK, INITPRIO, "proc 2", 1, &count2) );
    resume( create(Inc1, INITSTK, INITPRIO, "proc 3", 1, &count3) );
    resume( create(Pr, INITSTK, INITPRIO + 1, "proc 4", 0) );
}

/*------------------------------------------------------------------------
 *  Inc  --  Increment counter via pointer
 *--
----------------------------------------------------------------------
 */

void junk()
{
  asm {
        MOV BX,64000
        CALL BX
     }
}

Inc1(int ptr)
{
  unsigned long int *ptr1;

  ptr1 = (unsigned long int *)ptr;
  while (1)
   {
     (*ptr1)++;
     if ( (*ptr1) >= 10000000)
       junk();
   } // while

}  /* Inc */
Inc(int ptr)
{
  unsigned long int *ptr1;

  ptr1 = (unsigned long int *)ptr;
  while (1)
     (*ptr1)++;

}  /* Inc */

Pr()
{
  char str[80];

  while(1)
  {
    sleep(3);
    sprintf(str, "count1 = %lu, count2 = %lu, count3 = %lu\n",
                       count1, count2, count3);
    printf(str);
   } /* while */

} /* Pr */
