/* c_intr2d.c - Change Ctrl-Break interrupt handler.
   C + ASM version.   */

#include <stdio.h>
#include <dos.h>

volatile int Ctrl_Break_Flag;
long int Int27Save;  /* 32bit used to hold old ISR address */

void interrupt Ctrl_Break_Handler(void)
   {
     Ctrl_Break_Flag = 1;
     printf("C+InASM 1: Ctrl-Break has been pressed.\n");
   }

void main(void)
 {
   /* Int27Save = getvect(27);    Preserve old pointer */
   asm {
	 PUSH ES          /* Preserve registers  */
	 PUSH AX
			  /* Preserve pointer of old interrupt handler */
	 MOV AX,0         /* Set ES to point to Interrupt vector segment */
	 MOV ES,AX
	 MOV AX,ES:[27*4]    /* Read offset entry of Ctrl-Break ... */
	 MOV WORD PTR Int27Save,AX  /* ... interrupt handler */
	 MOV AX,ES:[27*4+2]    /* Read segment entry of Ctrl-Break ... */
	 MOV WORD PTR Int27Save+2,AX  /* ... interrupt handler */

/*   setvect(27,Ctrl_Break_Handler);     Set new interrupt handler */
	 CLI             /* Disable interrupts while IV is being changed */
	      /* Set offset of new interrupt handler  */
	 MOV WORD PTR ES:[27*4],OFFSET Ctrl_Break_Handler
	      /* Set segment number of new interrupt handler  */
	 MOV WORD PTR ES:[27*4+2],SEG Ctrl_Break_Handler
	 STI             /* Re-enable interrupts (modification finished) */
	 POP AX          /* Restore registers  */
	 POP ES
       }

   printf("C+InASM 1: Press Ctrl-Break to continue.\n");

   Ctrl_Break_Flag = 0;

   while (!Ctrl_Break_Flag)
       ;     /* Do nothing */

/*   setvect(27,Int27Save);     Restore old pointer */
   asm {
	 PUSH ES          /* Preserve registers  */
	 PUSH AX
	 MOV AX,0         /* Set ES to point to Interrupt vector segment */
	 MOV ES,AX
			 /* Restore old interrupt handler */
	 CLI             /* Disable interrupts while IV is being changed */
	 MOV AX,WORD PTR Int27Save  /* Restore the offset ...*/
	 MOV ES:[27*4],AX          /* ... of old interrupt handler */
	 MOV AX,WORD PTR Int27Save+2   /* Restore the segment number ... */
	 MOV ES:[27*4+2],AX  /* ... of old interrupt handler */
	 STI              /* Re-enable interrupts (modification finished) */
	 POP AX           /* Restore registers  */
	 POP ES
       }
 } /* main */

