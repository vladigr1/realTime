#include <stdio.h>

volatile long long int counter;
unsigned char old_0A1h_mask, old_021h_mask;
unsigned char old_reg_A_mask, old_reg_B_mask;

void interrupt (*int70save)(void);

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

void readclk(char str[])
{
  int i;
  int hour, min, sec;


  hour = min = sec = 0;

  asm {
   PUSH AX
   MOV AL,0
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR sec,AL
;
   MOV AL,2
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR min,AL
;
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

  sprintf(str,"%2d:%2d:%2d", hour, min, sec);

} // readclk

void readWeek(char str[]){
	char *days[7] = {"Sun","Mon","Tue","Wed","Thu","Fir","Sat"};
	int day = 0;
		asm{
		push ax
;
		mov al,6
		out 70h,al
		in al,71h
		mov BYTE PTR day,al
;		
		pop ax
		}
	day = convert_to_binary(day);
	sprintf(str,"\nToday is %s\n", days[day-1]);
}

void readDate(char str[]){
	int day, month, year;
	day = month = year = 0;
	asm{
		push ax
;
		mov al,7
		out 70h,al
		in al,71h
		mov BYTE PTR day,al
;
;
		mov al,8
		out 70h,al
		in al,71h
		mov BYTE PTR month,al
;
;
		mov al,9
		out 70h,al
		in al,71h
		mov BYTE PTR year,al
;
		pop ax
	}
	day = convert_to_binary(day);
	month = convert_to_binary(month);
	year = convert_to_binary(year);
	sprintf(str,"\n%2d:%2d:%2d\n", day, month, year);
}
void interrupt myISR0x70(void)
{
	counter++;
  asm {
   PUSH AX
   PUSH BX
   IN AL,70h   // Read existing port 70h
   MOV BX,AX

   MOV AL,0Ch  // Set up "Read status register C"
   OUT 70h,AL  //
   MOV AL,8Ch  //
   OUT 70h,AL  //
   IN AL,71h
   MOV AX,BX   //  Restore port 70h
   OUT 70h,AL  // 

   MOV AL,20h   // Set up "EOI" value  
   OUT 0A0h,AL  // Notify Secondary PIC
   OUT 020h,AL  // Notify Primary PIC

   POP BX
   POP AX

  } // asm */


} // new0x70isr

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


int main()
{	
	char str[16],ascii;
  int flag = 1;
  char old_0A1h_mask, old_70h_A_mask;
		counter =0;
	interrupt112Activate(); // activare interrupr 112 instead of interrupt 8
  while(1)
  {
    putchar(13);
    readclk(str);
    printf(str);
    asm {
        PUSH AX
        MOV AH,1
        INT 16h
        PUSHF
        POP AX
        AND AX,64 /* isolate zf */
        MOV flag,AX
        POP AX

    } // asm
	if (flag == 0){
		asm{
			push ax
			mov ah,0
			int 16h
			mov ascii,al
			pop ax
		}
		if(ascii == 27) break;
		switch(ascii){
			case 'D':
				readDate(str);
				printf("\nD%s",str);
			break;
			case 'T':
				readWeek(str);
				printf("\nT%s",str);
			break;
			case 'A':
				printf("\nA   A\nfrom A to A:%d\n",counter/1024);
				counter =0;
			break;
		}
	}
  } // while
	interrupt112Restore();
}  // main

