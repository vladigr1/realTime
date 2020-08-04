/*in 60 - catch scan code and inform pic */
#include <stdio.h>
#include <dos.h>

void interrupt (*Int9Save) (void);

unsigned char scan;
unsigned int scan_code;
unsigned int pressed;

void interrupt h9(void)
{
	asm{
	in 	al,60h    		//input
	mov 	scan, al 	// scan code
	test 	al,80h
	jnz   	rls_key
	}
	pressed = 1;		//scan code when pressed 7th bit == 0, release bit 1
rls_key: 				//if key rls
	asm{				//inform end interrupt
	in   al,61h			//inform device: read 61h
	or   al,80h			//inform device: turn on 7th bit (80h = 128)
	out  61h,al			//inform device: output 61h
	and  al,7Fh			//inform device: turn of 7th bit (7fh = 127)
	out  61h,al			//inform device: output 61h
	mov   al,20h 		//inform MASTER PIC:
	out   20h,al		//inform MASTER PIC:
	}
}
//main
void main(void)
{
	clrscr();
	pressed=0;
	Int9Save = getvect(9); // save old func
	setvect(9, h9);    // new func

	printf("C: Press Ctrl-Break to continue.\n");
	printf("\nPress ESC to exit program\n");

	do {
		printf("Press any key (almost)\n: ");

		while(!pressed);
		pressed=0;
		scan_code = (unsigned int) scan;
		if (scan_code == 1)
		printf("You pressed ESC, \n");
		printf("You pressed key assigned: scan code = %d ", scan_code);
	} while(!(scan_code == 1));
	setvect(9,Int9Save);  // old func
}
