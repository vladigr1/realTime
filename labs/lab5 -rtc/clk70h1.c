/* clk70.c */

#include <stdio.h>
#include <dos.h>



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

volatile int global_flag;

void interrupt (*old0x70isr)(void);

void interrupt new0x70isr(void)
{



  global_flag = 1;
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
//
   MOV AL,2
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR min,AL
//
   MOV AL,4
   OUT 70h,AL
   IN AL,71h
   MOV BYTE PTR hour,AL
//
   POP AX
  } // asm

  sec = convert_to_binary(sec);
  min = convert_to_binary(min);
  hour = convert_to_binary(hour);

  sprintf(str,"%2d:%2d:%2d", hour, min, sec);

} // readclk

int main()
{
  char str[16];
  int local_flag = 1, x71h1=0, x71h2=0, x71h3;
  char old_0A1h_mask, old_70h_A_mask;


  old0x70isr = getvect(0x70);
  setvect(0x70, new0x70isr);

  asm {
   CLI         // Disable interrupts
   PUSH AX     // Interrupt may occur while updating

   IN AL,0A1h  // Make sure IRQ8 is not masked
   MOV old_0A1h_mask,AL
   AND AL,0FEh // Set bit 0 of port 0A1 to zero
   OUT 0A1h,AL //

   IN AL,70h   // Set up "Write into status register A"
   MOV AL,0Ah  //
   OUT 70h,AL  //
   MOV AL,8Ah  //
   OUT 70h,AL  //
   IN AL,71h   //
   MOV BYTE PTR x71h1,AL  // Save old value
   MOV old_70h_A_mask,AL
   AND AL,11110000b // Change only Rate
   OR AL,0110b // Make sure it is Rate =0110 (1Khz)
   OUT 71h,AL  // Write into status register A
   IN AL,71h   // Read to confirm write



   IN AL,70h  // Set up "Write into status register B"
   MOV AL,0Bh //
   OUT 70h,AL //
   MOV AL,8Bh //
   OUT 70h,AL //
   IN AL,71h  //
   MOV BYTE PTR x71h2,AL // Save Old value
   AND AL,8Fh // Mask out PI,AI,UI
   OR AL,10h  // Enable update interrupts (UI=1) only
   OUT 71h,AL // Write into status register  B
   IN AL,71h  // Read to confirm write
   MOV byte ptr x71h3,AL // Save old value

   IN AL,021h  // Make sure IRQ2 is not masked
   AND AL,0FBh // Write 0 to bit 2 of port 21h
   OUT 021h,AL // Write to port 21h

   IN AL,70h  // Set up "Read into status resister C"
   MOV AL,0Ch // Required for for "Write into port 71h"
   OUT 70h,AL
   IN AL,70h
   MOV AL,8Ch // 
   OUT 70h,AL
   IN AL,71h  // Read status register C 
              // (we do nothing with it)

   IN AL,70h  // Set up "Read into status resister C"
   MOV AL,0Dh // Required for for "Write into port 71h"
   OUT 70h,AL
   IN AL,70h
   MOV AL,8Dh
   OUT 70h,AL
   IN AL,71h  // Read status register D 
              // (we do nothing with it)


   STI
   POP AX

  } // asm

  printf("x71h1 = %x, x71h2 = %x, x72h3  = %x\n", x71h1, x71h2, x71h3);

  while(local_flag)
  {


    putchar(13);
    global_flag = 0;
    while(global_flag == 0) // Wait for interrupt
         ;                  // Do not print all the time
    readclk(str);
    printf(str);
    asm {
        PUSH AX   // Press any key to terminate
        MOV AH,1  // Check If key has been pressed
        INT 16h   //
        PUSHF
        POP AX
        AND AX,64 // isolate zf 
        MOV local_flag,AX
        POP AX

    } // asm

  } // while


    asm {
          PUSH AX   // Cancel key pressed
          MOV AH,0
          INT 16h
          POP AX
        }  // asm

    asm {
        MOV AL,old_0A1h_mask
        OUT 0A1h,AL

        IN AL,70h  // restore A status register
        MOV AL,0Ah
        OUT 70h,AL
        MOV AL,8Ah
        OUT 70h,AL
        IN AL,71h
        MOV AL,old_70h_A_mask
        OUT 71h,AL
        IN AL,71h

    } // asm
    setvect(0x70, old0x70isr);



}  // main

