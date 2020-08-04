/* mysleep1.c - Implement sleep myself, Pure C version  */

#include <stdio.h>
#include <dos.h>

volatile int My_Sleep_Counter;

void interrupt (*Int8Save) (void);  /* Pointer to function */

void interrupt My_Int8_Handler(void)
   {
     asm {             /* Notify hardware:
                         Interrupt has been serviced */
          PUSH AX
          MOV AL,20h
          OUT 20h,AL
          POP AX
         }
         
     My_Sleep_Counter--;
   } /* My_Int8_Handler */

void my_sleep(int secs)
{
Int8Save = getvect(8);               /* Preserve old pointer */
setvect(8, My_Int8_Handler);           /* Set entry to new handler */

My_Sleep_Counter = secs*182/10;

while(My_Sleep_Counter > 0)
    ;
setvect(8,Int8Save);               /* Restore old pointer */

}  /* my_sleep */ 

void main(void)
 {
  int secs;

   printf("Enter sleep time in seconds:\n");
   scanf("%d", &secs);

   system("time");
   printf("sleeping ---\n");
   
   my_sleep(secs);

   system("time");

   printf("Terminating ...\n");

 } /* main */

