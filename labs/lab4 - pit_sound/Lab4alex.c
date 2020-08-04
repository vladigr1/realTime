#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define ON (1)
#define OFF (0)

volatile int cpyT; // copy of typed time
int hertz; // random hertz getting in case of success
int i = 0; // index of typed string
char flag = 1; // to stop main loop 
char typedStr[100] = {'\0'}; // typed string
volatile unsigned long long int	count; // count number of interrupt 8 
char *ptrToStr; // pointer to given string

   
unsigned char KeyboardLayout[128] =	
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

void endProg();
void tsttype(char str[], int t);
void Sound(int hertz);
void ChangeSpeaker(int status);
void mydelay(int n);
void interrupt(*int8Save)(void); /*pointer to save old ISR address */
void interrupt(*int9Save)(void); /*pointer to save old ISR address */

void endProg()
{

	asm {
          CLI
          PUSH AX
          MOV AL,036h
          OUT 43h,AL
          MOV AX,0    // Original count for interrupt 8 (65536)
          OUT 40h,AL
          MOV AL,AH
          OUT 40h,AL 
          POP AX
        } // asm
		
/*Put the Interrupt's address back in interrupt vector*/		
	setvect(8,int8Save);
	setvect(9,int9Save);
	ChangeSpeaker(OFF);
	flag = 0;
///////////////////////////////////////////////////////	
}/*end endProg*/


void interrupt My_Int9_Handler(void)
{
	unsigned char scanCode;
	
	asm{
		IN AL,60h
		MOV	scanCode,AL
		TEST scanCode,80h
		JZ gotKey
		JMP callOriginal
	}
		
gotKey:
    //printf("%c",KeyboardLayout[scanCode]);
	typedStr[i] = KeyboardLayout[scanCode];
	i++;

	asm{
		IN AL,61h
		OR AL,80h
		OUT 61h,AL
		AND AL,7Fh
		OUT 61h,AL
	}
	
callOriginal:
	asm{
		MOV AL,20h  
		OUT 20h,AL
	}
}/*end interrupt My_Int9_Handler*/


void interrupt My_Int8_Handler(void)
{
	count++;
	
	if(count == cpyT*100){
		printf("\nYou goofed: You typed: %s\n",typedStr);
		endProg();
	}
	
	//Call original ISR
	asm{
		PUSHF
		CALL DWORD PTR int8Save
    }
	
}/*end interrupt My_Int8_Handler*/


void tsttype(char str[], int t){
	
	int sizeOfStr;

	count = 0;
	cpyT = t;
	sizeOfStr = strlen(str);
	
	printf("Type: %s\n",str);
	ptrToStr = strlwr(str);
	//printf("Size is: %d\n",sizeOfStr);
	printf("In %d seconds\n",cpyT);
	
/*********Taking control of Interrupt**********************/
	asm {
          CLI
          PUSH AX
          MOV AL,036h
          OUT 43h,AL
          MOV AX,11932 // change count(new latch)
          OUT 40h,AL
          MOV AL,AH
          OUT 40h,AL 
          POP AX
        } // asm

	int8Save = getvect(8);
	setvect(8, My_Int8_Handler);
	asm {STI};
	
	int9Save = getvect(9);
	setvect(9, My_Int9_Handler);
/**********************************************************/
	
	while(flag){
	 if(i == sizeOfStr){
		if (strcmp(typedStr, ptrToStr) == 0){
			printf("You did it!\n");
			srand(time(NULL));
            hertz = (rand() % (600 - 300 + 1)) + 300; // rand()%(upper - lower + 1))+lower; 
			//printf("%d\n",hertz);
			Sound(hertz);
			mydelay(count);
			endProg();
		}    
	 } 
  }
}

void Sound( int hertz )
	{
	 unsigned divisor = 1193180L / hertz;

	  ChangeSpeaker( ON );
	  
	  asm{
		  	MOV AL,0B6h
			OUT 43H,AL
			MOV BX, divisor  // CHANGE COUNT 
			MOV AL,BL        // TRANSFER THE LSB 1ST
			OUT 42H,AL
			MOV AL,BH     // TRANSFER MSB SECOND
			OUT 42H,AL
	  } // asm

} /*--Sound( )-----*/


/*------------------------------------------------
 ChangeSpeaker - Turn speaker on or off. */

void ChangeSpeaker( int status )
 {
  int portval;
//   portval = inportb( 0x61 );

      portval = 0;
   asm {
        PUSH AX
        IN AL,61h
        MOV byte ptr portval,AL
        POP AX
       }

    if ( status==ON )
     portval |= 0x03;
      else
       portval &=~ 0x03;
        // outportb( 0x61, portval );
        asm {
          PUSH AX
          MOV AX,portval
          OUT 61h,AL
          POP AX
        } // asm

} /*--ChangeSpeaker( )----------*/

void mydelay(int n)
{
    count = 0;
	//printf("%d",n);
    while(count <= n);

} //mydelay


int main()
{
 tsttype("To be or not to be", 10);
 return 0;
} // main
