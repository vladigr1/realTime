#include<stdio.h>

#include <dos.h>

extern void Start_track(); 

extern void end_track();

extern void interrupt add_identify();

void main(void)
          {

            int i1, i2, result;
            Start_track();

            do{
               printf("Enter two ints, ( 0 0 ) to signal end: \n");
               scanf("%d %d", &i1, &i2);
               result = i1 + i2;
               printf("%d + %d = %d\n", i1, i2, result);

            } while((i1 != 0) && (i2 != 0));

             end_track();
} /* main */


