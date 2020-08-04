#include <stdio.h>
#include <dos.h>
#include <stdlib.h>



typedef struct node {
	char *strPreSet;
	char *fstrPreSet;
	int *iptrPreSet;
	volatile int iValuePreSet;
	short csfunptr;
	short ipfunptr;
	volatile int count;
	struct node *next;
}node;

void interrupt (*int1Save) (void);
void interrupt myInt1(void);
void interrupt (*int8Save) (void);
void interrupt myInt8(void);
void follow(int *iptr,char str[],void (*funptr)(), char fstr[]);
void unFollow();
void printList();
void func(void)
{	
}

//queue elements
node *head = NULL,*tail = NULL;

//test variables
//volatile short csTEST;
//volatile short ipTEST;



void main(void)
{
	int i , f1 = 1 , f2 = 1 , fibo = 1;
	follow(&fibo, "fibo", sleep, "sleep");
	sleep(3);
	follow(&f1, "f1", func, "func");
	for(i=0; i < 10; i++){
	sleep(1);
	func();
	f1 = f2;
	f2 = fibo;
	fibo = f1 + f2;
	} // for
	unFollow();
	f1=0;
	fibo = 0;
 } // main




void interrupt myInt8(void){	//timer
	node *pnode = head;
	while(pnode != NULL){
		pnode->count++;
		pnode = pnode->next;
	}
	if(head->count == 5*18){
		pnode = head;
		//printf("free:%s, %s\n",pnode->strPreSet,pnode->strPreSet);
		head = head->next;
		if(head == NULL){	//special case the follow list is ended
			tail = NULL;
			//printf("all free\n");
			//printList();
			unFollow();		//their we can set back to original iv when the queue is empty
		}
		free(pnode);
	}
	asm{	//must handle the isr
		PUSHF
		CALL DWORD PTR int8Save
	}
}

void interrupt myInt1(void){
	node *pnode = head;
	while(pnode != NULL){
		short ipfunptr = pnode->ipfunptr,csfunptr = pnode->csfunptr;
		asm{								//check funptr
			MOV AX,WORD PTR ipfunptr
			MOV DX,WORD PTR csfunptr		
			CMP AX,[BP+18]                  //check ip == funptrPreSet
			JNE NoPrintfunptr				
			CMP DX,[BP+20]					//check CS == funptrPreSet
			JNE NoPrintfunptr		               
		}
		printf("procedure %s has been called\n",pnode->fstrPreSet);
		
		/*
		//START:TEST TO SEE WHAT HAPPENS
		//conclusion no segmentations
		csTEST = 0;
		ipTEST = 0;
		asm{
			MOV BX,WORD PTR ipfunptr
			MOV AX,WORD PTR csfunptr
			MOV WORD PTR csTEST,AX
			MOV WORD PTR ipTEST,BX
		}
		printf("cs in funptr = %d, ip  in funptr = %d \n",csTEST,ipTEST);
		asm{
			MOV AX,[BP+18]
			MOV ipTEST,AX
			MOV AX,[BP+20]
			MOV csTEST,AX
		}
		printf("cs in stack = %d, ip in stack = %d \n",csTEST,ipTEST);
		//END:TEST TO SEE WHAT HAPPENS
		*/
		
		NoPrintfunptr:		//label for jumping if the funptr didnt called
		if(pnode->iValuePreSet != *(pnode->iptrPreSet)){
			printf("variable %s has changed from %d to %d\n",pnode->strPreSet,pnode->iValuePreSet,*(pnode->iptrPreSet));
			pnode->iValuePreSet = *(pnode->iptrPreSet);
		}
		pnode = pnode->next;
	}
}

void follow(int *iptr,char str[],void (*funptr)(), char fstr[]){
	node *pnode= (node *)malloc(sizeof(node));
	short csfunptr,ipfunptr;
	if (pnode == NULL) exit(1);	//can set get_Lost	
	asm{
		push word ptr funptr[2]
		pop word ptr csfunptr
		push word ptr funptr
		pop word ptr ipfunptr
	}
	asm{			//must turn of TF because adding to the queue generate a change we must turn off myInt1
		pushf
		pop ax
		and ax,1111111011111111B	//2^16 - 1 - 256 = 1111 1110 1111 1111
		push ax
		popf
	}	
	
	pnode->iValuePreSet = *iptr;	//generate node
	pnode->iptrPreSet = iptr;
	pnode->strPreSet = str;
	pnode->fstrPreSet = fstr;
	pnode->csfunptr = csfunptr;
	pnode->ipfunptr = ipfunptr;
	pnode->count = 0;
	pnode->next = NULL;
	if(head == NULL) head = pnode;
	else tail->next = pnode;
	tail = pnode;

	if(head == pnode){
		int1Save = getvect(1);
		setvect(1,myInt1);
		int8Save = getvect(8);
		setvect(8,myInt8);
	}
	asm{						//in any case you after follow you must turn TF
			pushf
			pop ax
			or ax,100000000B	 //1 0000 0000B -> 256 = 2^8
			push ax
			popf
		}
}

void unFollow(){
	asm{
		pushf
		pop ax
		and ax,1111111011111111B	//2^16 - 1 - 256 = 1111 1110 1111 1111
		push ax
		popf
	}
	setvect(1,int1Save);
	setvect(8,int8Save);
	//printf("unfollow\n");
	while(head !=NULL){
		node *pnode = head;
		head = head->next;
		free(pnode);
	}
}

/*
//check queue elements
void printList(){
	node *pnode = head;
	while (pnode != NULL) {
		printf("%s, %s\n",pnode->strPreSet,pnode->strPreSet);
		pnode = pnode->next;
	}
}
*/