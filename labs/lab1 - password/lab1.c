#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

#define TIME_PASS_FOR_NEW_PASS 15



long silence_keyboard(char password [] , int size, long  time);
char time_limited_getch ( int secs );
void interrupt (*int8Save) (void);
void interrupt myInt8(void);
//int 9 allow only small latter and 1 key too


volatile int count;
volatile char flagChangePass;
volatile char curPassword[101];
int passwordSize;



int main(){
	char str[] = "azddz";
	int keyTime = 5;
	long time = silence_keyboard(str, sizeof(str), keyTime);
	printf("\n total time : %ld\n", time);
	return 0;
} // main



void interrupt myInt8(void){
	count++;
	if(count % (18*TIME_PASS_FOR_NEW_PASS) == 0 ) flagChangePass = 1;
	asm{
		PUSHF
		CALL DWORD PTR int8Save
	}
}

//getch() that answer the requested assignment
char time_limited_getch ( int secs )
{
	volatile int flag;
	char c1;
	int startTime = count/18;
  while(1)
  {
	asm{
	PUSH AX
	MOV AH,1
	INT 16h
	PUSHF
	POP AX
	AND AX,64  //zero flag
	MOV flag,AX
	POP AX

	} /* checking if butten was presed*/
	if( flag==0 )
	{
		asm{
		PUSH AX
		MOV AH,0
		INT 16h
		MOV c1,AL
		POP AX
		}/* reading the butten*/
		return c1;
	}
	if(flagChangePass == 1){
		int i;
		for(i=0;i<passwordSize;++i){
			curPassword[i] = 'a' + (curPassword[i] - 'a' + 1) % 26; 
		}
		flagChangePass = 0;
	}
	if( count/18 > startTime + secs)
		return '0';
  } /* while */
}/*end time_limited_getch*/


long silence_keyboard(char password [] , int size, long  time){
	//it easier too check that sequence of chars
	//will be the same as the password then comparing strings
	int i;
	char curChar;
	strcpy(curPassword,password);
	passwordSize = size -1;
	flagChangePass = 0;
	count = 0;
	int8Save = getvect(8);
	setvect(8,myInt8);
	i =0;
	//end init
	
	while(i < passwordSize){
		curChar = time_limited_getch(time);
		if(curChar == '1'){
			printf("\ntime is %d and %d\n",count/18,(count/18)% TIME_PASS_FOR_NEW_PASS);
			printf("\npass: %s\n",curPassword);	//the password changes every 15 seconds
												//we want to simplify the check
		}else if(curChar == curPassword[i]){
			printf("%c",curChar);
			i++;
		}else{
			printf("\nstart over\n");
			i =0;
		}
	}
	
	//password enter correctly
	setvect(8,int8Save);
	return count/18;
}
