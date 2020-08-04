#include<stdio.h>

#include <dos.h>


extern void Start_IMUL_Count(); 

extern int Return_IMUL_Count();

extern void interrupt multy_int();




void main(void)
 {
  unsigned int i, j, z, count;

 Start_IMUL_Count();
 z = 0;
 for(i=0; i < 10; i++)
   for(j=0; j < 10; j++)
     z = z + i*j;

 count = Return_IMUL_Count();

 printf("No of imul instructions executed: %u\n", count);
 Start_IMUL_Count();
 z = 0;
 for(i=0; i < 5; i++)
   for(j=0; j < 5; j++)
     z = z + i*j;

 count = Return_IMUL_Count();

 printf("No of imul instructions executed: %u\n", count);

} /* main */
