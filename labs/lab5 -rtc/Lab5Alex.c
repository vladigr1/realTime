/* Lab5.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

unsigned long long int numberOfInterrupt70h [1]; // array to save number of 70h interrupts between A pressing
int keystrokeAnumber = 0; // to check number of 70h interrupts was occured
char pressedAflag = 0; // to know that A was pressed at least twice
volatile unsigned long long int count0x70 = 0; //count number of 70h interrupts
unsigned char old_0A1h_mask, old_021h_mask;
unsigned char old_reg_A_mask, old_reg_B_mask;

void interrupt(*int70save)(void);


int  convert_to_binary(int x)
{
 int i;
 int temp, scale, result;
 
  temp =0;
  scale = 1;
  for(i=0; i < 4; i++)
   {
     temp = temp + (x % 2)*scale;
     scale *= 2;
     x = x >> 1;
   } // for

  result = temp;
  temp = 0;

  scale = 1;
  for(i=0; i < 4; i++)
   {
     temp = temp + (x % 2)*scale;
     scale *= 2;
     x = x >> 1;
   } // for

  temp *= 10;
  result = temp + result;
  return result;

} // convert_to_binary


// last part of interrupt handling
void interrupt myISR0x70(void)
{
	// notification to hardware that interrupt has been handling
	asm{
		PUSH AX
		PUSH BX

		IN AL, 70h   // Read existing port 70h
		MOV BX, AX	 // Save old value

		MOV AL, 0Ch  // Set up "Read status register C" (msb off)
		//If you do not read C, the interrupt is not repeated
		OUT 70h, AL  
		MOV AL, 8Ch  // (msb on)
		OUT 70h, AL  
		IN AL, 71h
		MOV AX, BX   //  Restore port 70h
		OUT 70h, AL  

		MOV AL, 20h   // Set up "EOI" value  
		OUT 0A0h, AL  // Notify Secondary PIC (slave)
		OUT 020h, AL  // Notify Primary PIC (master)

		POP BX
		POP AX

	} // asm */
	
	count0x70++;

} // myISR0x70


void readDay(char str[]){
	int day = 0;
	
	asm{
		PUSH AX
		MOV AL, 6
		OUT 70h, AL
		IN AL, 71h
		MOV BYTE PTR day, AL
;
		POP AX
	} // asm
	
	switch(day){
		case 1:
		sprintf(str, "\nToday is Sunday\n");
		break;
		case 2:
		sprintf(str, "\nToday is Monday\n");
		break;
		case 3:
		sprintf(str, "\nToday is Tuesday\n");
		break;
		case 4:
		sprintf(str, "\nToday is Wednesday\n");
		break;
		case 5:
		sprintf(str, "\nToday is Thursday\n");
		break;
		case 6:
		sprintf(str, "\nToday is Friday\n");
		break;
		case 7:
		sprintf(str, "\nToday is Saturday\n");
		break;
		default:
		printf("Error");
	}
	
} // readDay


void readDate(char str[])
{
	int day, month, year;
	day = month = year = 0;

	asm{
		PUSH AX
		MOV AL, 7
		OUT 70h, AL
		IN AL, 71h
		MOV BYTE PTR day, AL
;
		MOV AL, 8
		OUT 70h, AL
		IN AL, 71h
		MOV BYTE PTR month, AL
;
		MOV AL, 9
		OUT 70h, AL
		IN AL, 71h
		MOV BYTE PTR year, AL
;
		POP AX
	} // asm

	day = convert_to_binary(day);
	month = convert_to_binary(month);
	year = convert_to_binary(year);

	sprintf(str, "\nDate: %2d/%2d/%2d\n", day, month, year);	// "print" into string "str"

} // readDate


void readclk(char str[])
{
  int i;
  int hour, min, sec;

  hour = min = sec = 0;

  asm {
	  
//get seconds of real time
	  
   PUSH AX
   MOV AL,0
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR sec,AL
   
//get minuts of real time

   MOV AL,2
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR min,AL
   
//get hours of real time

   MOV AL,4
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR hour,AL
;
   POP AX
  } // asm

  sec = convert_to_binary(sec);
  min = convert_to_binary(min);
  hour = convert_to_binary(hour);

  sprintf(str,"Time: %2d:%2d:%2d", hour, min, sec);

} // readclk


//Interrupt 70h programming
void interrupt112Activate()
{
	int70save = getvect(0x70); // saving old interrupt 70h
	setvect(0x70, myISR0x70); // set new interrupt 70h

	asm{
		CLI         // disable interrupts
		PUSH AX     // interrupt may occur while updating

//checking bit number 0 (IRQ8 - 70H) of Mask Register  equal to 0
		IN AL, 0A1h					// Make sure IRQ8 is not masked
		MOV old_0A1h_mask, AL		// Save old value
		AND AL, 0FEh				// Set bit 0 of port A1h to zero (IRQ8 - 70H)
		OUT 0A1h, AL
		
//Mask Saving - old_0A1h_mask
		IN AL, 021h  				// Make sure IRQ2 is not masked
		MOV old_021h_mask, AL		// Save old value
		AND AL, 0FBh 				// Write 0 to bit 3 of port 21h (IRQ2 - PIC Slave Control)
		OUT 021h, AL				// Write to port 21h

//Choosing frequency of 70h interrupt
		/*----STATUS REGISTER A----*/
		IN AL, 70h					// Set up "Write into status register A"
		MOV AL, 0Ah
		OUT 70h, AL
		MOV AL, 8Ah
		OUT 70h, AL
		IN AL, 71h
		MOV old_reg_A_mask, AL
		AND AL, 1000000b			// Change only Rate & Base
		OR AL, 00100110b			// Set Base=010, Rate=0110(1024Hz)
		OUT 71h, AL					// Write into status register A
		IN AL, 71h					// Read to confirm write

//Choosing type of the interrupt 70h
		/*----STATUS REGISTER B----*/
		IN AL, 70h  // Set up "Write into status register B"
		MOV AL, 0Bh //
		OUT 70h, AL //
		MOV AL, 8Bh //
		OUT 70h, AL //
		IN AL, 71h  //
		MOV old_reg_B_mask, AL // Save Old value
		AND AL, 8Fh // Mask out PI,AI,UI
		OR AL, 40h  // Enable periodic interrupts 
		OUT 71h, AL // Write into status register  B
		IN AL, 71h  // Read to confirm write

//WE must to turn to C and D after A and B programming
		/*----STATUS REGISTER C----*/
		IN AL, 70h  // Set up "Read from status resister C"
		MOV AL, 0Ch // Required for for "Write into port 71h"
		OUT 70h, AL
		IN AL, 70h
		MOV AL, 8Ch // 
		OUT 70h, AL
		IN AL, 71h  // Read status register C 
		// (we do nothing with it)

		/*----STATUS REGISTER D----*/
		IN AL, 70h  // Set up "Read from status resister D"
		MOV AL, 0Dh // Required for for "Write into port 71h"
		OUT 70h, AL
		IN AL, 70h
		MOV AL, 8Dh
		OUT 70h, AL
		IN AL, 71h  // Read status register D 
		// (we do nothing with it)

		STI
		POP AX
	} // asm
	
}// interrupt112Activate


void interrupt112Restore()
{
	asm{
		CLI
		PUSH AX

		MOV AL, old_0A1h_mask	// restore port A1h mask
		OUT 0A1h, AL
		MOV AL, old_021h_mask	// restore port 21h mask
		OUT 021h, AL

		IN AL, 70h	// restore status register A
		MOV AL, 0Ah
		OUT 70h, AL
		MOV AL, 8Ah
		OUT 70h, AL
		IN AL, 71h
		MOV AL, old_reg_A_mask
		OUT 71h, AL
		IN AL, 71h

		IN AL, 70h	// restore status register B
		MOV AL, 0Bh
		OUT 70h, AL
		MOV AL, 8Bh
		OUT 70h, AL
		IN AL, 71h
		MOV AL, old_reg_B_mask
		OUT 71h, AL
		IN AL, 71h

		/*----STATUS REGISTER C----*/
		IN AL, 70h  // Set up "Read from status resister C"
		MOV AL, 0Ch // Required for for "Write into port 71h"
		OUT 70h, AL
		IN AL, 70h
		MOV AL, 8Ch // 
		OUT 70h, AL
		IN AL, 71h  // Read status register C 
		// (we do nothing with it)

		/*----STATUS REGISTER D----*/
		IN AL, 70h  // Set up "Read from status resister D"
		MOV AL, 0Dh // Required for for "Write into port 71h"
		OUT 70h, AL
		IN AL, 70h
		MOV AL, 8Dh
		OUT 70h, AL
		IN AL, 71h  // Read status register D 
		// (we do nothing with it)

		POP AX
		STI
	}// asm
	
	setvect(0x70, int70save);
}


void main()
{
  char str[16];
  int flag = 1;
  
  interrupt112Activate(); // activare interrupr 112 instead of interrupt 8

Loop:

  while(1)
  {  
    putchar(13); // shift left
    readclk(str); // getting real time (hours, minuts and seconds)
    printf(str);
    asm {
		//if any button was pressed => flag get zero
        PUSH AX
        MOV AH,1
        INT 16h
        PUSHF
        POP AX
        AND AX,64 /* isolate zf */
        MOV flag,AX
        POP AX
    } // asm
	
		if( flag==0 )
	{
		asm{
			PUSH AX
			MOV AH,0
			INT 16h
			CMP AH,1  // ESC was pressed?
			JE EndFunc
			CMP AL,65 // A was pressed?
			JE NumberOfInterrupt70hPrint
			CMP AL,77 // M was pressed?
			JE MaxNumberOfInterrupt70hPrint
			CMP AL,68 // D was pressed?
			JE DatePrint
			CMP AL,84 // T was pressed?
			JE DayOfTheWeekPrint
			POP AX
		}/* reading the button*/
	}
  }// while
  
EndFunc:  // ESC was pressed

	asm{
		POP AX
	}

	interrupt112Restore(); // restore interrupt 112 
  
	exit(0); // end of the main
  
MaxNumberOfInterrupt70hPrint:  // M was pressed
  
    asm{
	  POP AX
	}
	
	if(pressedAflag >= 2){
		if(numberOfInterrupt70h[0] >= numberOfInterrupt70h[1])
			printf("\nMax. number from A to A: %llu sec.\n", numberOfInterrupt70h[0]);
		else
			printf("\nMax. number from A to A: %llu sec.\n", numberOfInterrupt70h[1]);
	}
	
  	  asm{
		  JMP Loop
	  }
	  
DayOfTheWeekPrint: // D was pressed

	asm{
		POP AX
	}
	
	readDay(str);
	printf(str);
	
	asm{
		  JMP Loop
	 }
  
DatePrint: // T was pressed

	asm{
		POP AX
	}
	
	readDate(str);
	printf(str);
	
	asm{
		  JMP Loop
	  }
  
NumberOfInterrupt70hPrint: // A was pressed
 
	  asm{
		  POP AX
	  }
  
	  pressedAflag++;
	  keystrokeAnumber = keystrokeAnumber % 2;
      numberOfInterrupt70h[keystrokeAnumber] = count0x70/1024; //Base=010, Rate=0110 of A register(1024Hz)
	  printf("\nFrom A to A: %llu sec.\n", count0x70/1024);
	  keystrokeAnumber++;
	  count0x70 = 0;
	  
	  asm{
		  JMP Loop
	  }
	 
} // main