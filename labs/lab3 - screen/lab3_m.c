//MODEL large
#include <stdio.h>
#include <dos.h>
#include<stdlib.h>

void start_screen();					//set mode 40 by 25 color image
void end_screen();
void clear_kbuffer();
void SetCursorPos(short location);		//short is sized as reg
void interrupt myInt9();
void interrupt myInt8();



void interrupt (*oldint9)(void);
void interrupt (*oldint8)(void);
volatile short CursorPos;


volatile int count;
volatile char sc_pressed;


int main(){
	
	oldint9 = getvect(9);
	oldint8 = getvect(8);
	count =0;
	setvect(9,myInt9);
	setvect(8,myInt8);
	start_screen();
	clear_kbuffer();
	CursorPos = 40*25-1;
	SetCursorPos(CursorPos);
	while(sc_pressed != 1 && count < 60 * 18){
	}
	end_screen();
	setvect(8,oldint8);
	setvect(9,oldint9);
	return 0;
}

void interrupt myInt8(){
	count++;
	oldint8();
}