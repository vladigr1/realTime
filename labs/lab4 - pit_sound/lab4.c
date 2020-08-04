#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>

#define ON (1)
#define OFF (0)

void tsttype(char str[], int t);
void interrupt my9int(void);
void interrupt my8int(void);
void mydelay(int n);
void ChangeSpeaker( int status );
void Sound( int hertz );
void NoSound( void );



volatile int iChar;
char *curStr;
void interrupt (*old9int)(void);
void interrupt (*old8int)(void);
volatile int counter;

int main()
{
 tsttype("To be or not to be", 10);
 return 0;
} // main

void tsttype(char str[], int t){
	int strLen = strlen(str);
	int peach, timeSpent;
	srand ( time(NULL) );
	peach = rand() % 301 +300;
	curStr = str;
	iChar = 0;
	counter = 0;
	printf("Type:%s\nIn %d seconds\n",str,t);
	asm{
		mov al,36h
		out 43h,al
		mov bx,11932
		mov al,bl
		out 40h,al
		mov al,bh 
		out 40h,al
	}
	old8int = getvect(8);
	setvect(8,my8int);
	old9int = getvect(9);
	setvect(9,my9int);
	while(iChar < strLen && counter < 100 * t){
	}
	setvect(9,old9int);
	if(iChar == strLen){	//win
		printf("you did it!\n");
		timeSpent = counter;
		Sound(peach);
		mydelay(timeSpent);
		NoSound( );
	}else{
		char typed[100];
		strncpy(typed,str,iChar);
		typed[iChar] = 0;
		printf("You goofed: you typed: %s\n",typed);
	}
	asm{
		mov al,36h
		out 43h,al
		mov bx,0
		mov al,bl
		out 40h,al
		mov al,bh 
		out 40h,al
	}
	setvect(8,old8int);
}

void interrupt my9int(void){
	unsigned char curCh = 0;
	asm{
		in al,60h
		mov curCh,al
	}
	old9int();
	if(curCh < 128){
		asm{
			mov ah,1
			int 16h
			jz myend9
		}
		//if(curCh != 0){
			asm{		//clear buffer
				mov ah,0
				int 16h
				mov curCh,al
			}
			if(curCh == curStr[iChar]){
				iChar++;
				return;
			}else{
myend9:			
			iChar=0;	
			}
			//printf("c:%c iChar:%d ",curCh,iChar);
		//}
	}
	////ack reading char		//doesnt need because we call old 9
	//asm{                      //we need old for pushing to keyboard buffer
	//	in al,61h
	//	or al,128
	//	out 61h,al
	//	and al,127
	//	out 61h,al	
	//}
	////ack interupt controlen
	//asm{
	//	mov al,20h
	//	out 20h,al
	//}
}

void interrupt my8int(void){
	counter++;
	asm{
		mov al,20h
		out 20h,al
	}
}

void mydelay(int n)
{
    counter = 0;
	//printf("%d",n);
    while(counter < n);

} //mydelay

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
	
    if ( status==ON ){
     portval |= 0x03;
	}else{
       portval &=~ 0x03;
	}
	// outportb( 0x61, portval );
	asm {
	PUSH AX
	MOV AX,portval
	OUT 61h,AL
	POP AX
	} // asm

} /*--ChangeSpeaker( )----------*/
void Sound( int hertz ){
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

void NoSound( void ){
             ChangeSpeaker( OFF );
        } /*--NoSound( )------*/
	