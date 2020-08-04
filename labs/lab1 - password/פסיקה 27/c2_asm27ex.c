/* c_intr3d.c - Change Ctrl-Break interrupt handler.
   C + ASM version, use DOS INT 21h AH=35h, 25h.   */

#include <stdio.h>
#include <dos.h>

int Ctrl_Break_Flag;
long int Int27Save;  /* 32bit used to hold old ISR address */

void interrupt Ctrl_Break_Handler(void)
   {
     Ctrl_Break_Flag = 1;
     printf("C+InASM 2: Ctrl-Break has been pressed.\n");
   }

void main(void)
 {
   /* Int27Save = getvect(27);    Preserve old pointer */
   asm {
	 PUSH DS          /* Preserve registers  */
	 PUSH ES
	 PUSH AX
	 PUSH BX

	 MOV AH,35h         /* Set DOS interrupt handler */
	 MOV AL,27          /* Set interrupt vector entry number */
	 INT 21h            /* Invoke DOS  */
			    /* Preserve offset of old interrupt handler */
	 MOV WORD PTR Int27Save,BX
	 MOV AX,ES          /* Preserve segment number of old ... */
	 MOV WORD PTR Int27Save+2,AX  /* ... interrupt handler */

   /* setvect(27, Ctrl_Break_Handler); Set new interrupt handler */
			  /* Set offset of new interrupt handler */
	 MOV DX,OFFSET Ctrl_Break_Handler
	 MOV AX,SEG Ctrl_Break_Handler   /* Set segment number of new ... */
	 MOV DS,AX                  /* ... interrupt handler */
	 MOV AH,25h                 /* Set DOS interrupt handler */
	 MOV AL,27                  /* Set interrupt vector entry number */
	 INT 21h                    /* Invoke DOS  */

	 POP BX                     /* Restore registers  */
	 POP AX
	 POP ES
	 POP DS
       }

   printf("C+InASM 2: Press Ctrl-Break to continue.\n");

   Ctrl_Break_Flag = 0;
   while (!Ctrl_Break_Flag)
	; /* Do nothing */

/*   setvect(27,Int27Save);     Restore old pointer */
   asm {
	 PUSH DS          /* Preserve registers  */
	 PUSH AX
			 /* Restore number old interrupt handler */
	 MOV DX,WORD PTR Int27Save
	 MOV AX,WORD PTR Int27Save+2   /* Restore the number ... */
	 MOV DS,AX               /* ...  of old interrupt handler */
	 MOV AH,25h              /* Set DOS interrupt handler */
	 MOV AL,27               /* Set interrupt vector entry number */
	 INT 21h                 /* Invoke DOS  */
	 POP AX            /* Restore registers  */
	 POP DS
       }
 } /* main */

