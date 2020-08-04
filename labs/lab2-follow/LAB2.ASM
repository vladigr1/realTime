	ifndef	??version
?debug	macro
	endm
$comm	macro	name,dist,size,count
	comm	dist name:BYTE:count*size
	endm
	else
$comm	macro	name,dist,size,count
	comm	dist name[size]:BYTE:count
	endm
	endif
	?debug	S "lab2.c"
	?debug	C E98C517350066C6162322E63
	?debug	C E94C97284C15433A5C54435C494E434C5544455C737464696F2E68
	?debug	C E94C97284C15433A5C54435C494E434C5544455C5F646566732E68
	?debug	C E94C97284C15433A5C54435C494E434C5544455C5F6E756C6C2E68
	?debug	C E94B97284C13433A5C54435C494E434C5544455C646F732E68
	?debug	C E94C97284C16433A5C54435C494E434C5544455C7374646C69622E+
	?debug	C 68
LAB2_TEXT	segment byte public 'CODE'
LAB2_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:LAB2_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
LAB2_TEXT	segment byte public 'CODE'
   ;	
   ;	void func(void)
   ;	
	assume	cs:LAB2_TEXT
_func	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;	{	
   ;	}
   ;	
	pop	bp
	ret	
_func	endp
LAB2_TEXT	ends
_DATA	segment word public 'DATA'
_followFlag	label	word
	db	0
	db	0
_DATA	ends
LAB2_TEXT	segment byte public 'CODE'
   ;	
   ;	void main(void)
   ;	
	assume	cs:LAB2_TEXT
_main	proc	far
	push	bp
	mov	bp,sp
	sub	sp,8
   ;	
   ;	{
   ;		int i , f1 = 1 , f2 = 1 , fibo = 1;
   ;	
	mov	word ptr [bp-4],1
	mov	word ptr [bp-6],1
	mov	word ptr [bp-8],1
   ;	
   ;		follow(&fibo, "fibo", sleep, "sleep");
   ;	
	push	ds
	mov	ax,offset DGROUP:s@+5
	push	ax
	mov	ax,seg _sleep
	push	ax
	mov	ax,offset _sleep
	push	ax
	push	ds
	mov	ax,offset DGROUP:s@
	push	ax
	push	ss
	lea	ax,word ptr [bp-8]
	push	ax
	call	far ptr _follow
	add	sp,16
   ;	
   ;		sleep(3);
   ;	
	mov	ax,3
	push	ax
	call	far ptr _sleep
	pop	cx
   ;	
   ;		follow(&f1, "f1", func, "func");
   ;	
	push	ds
	mov	ax,offset DGROUP:s@+14
	push	ax
	mov	ax,seg _func
	push	ax
	mov	ax,offset _func
	push	ax
	push	ds
	mov	ax,offset DGROUP:s@+11
	push	ax
	push	ss
	lea	ax,word ptr [bp-4]
	push	ax
	call	far ptr _follow
	add	sp,16
   ;	
   ;		for(i=0; i < 10; i++){
   ;	
	mov	word ptr [bp-2],0
	jmp	short @2@114
@2@58:
   ;	
   ;		sleep(1);
   ;	
	mov	ax,1
	push	ax
	call	far ptr _sleep
	pop	cx
   ;	
   ;		func();
   ;	
	push	cs
	call	near ptr _func
   ;	
   ;		f1 = f2;
   ;	
	mov	ax,word ptr [bp-6]
	mov	word ptr [bp-4],ax
   ;	
   ;		f2 = fibo;
   ;	
	mov	ax,word ptr [bp-8]
	mov	word ptr [bp-6],ax
   ;	
   ;		fibo = f1 + f2;
   ;	
	mov	ax,word ptr [bp-4]
	add	ax,word ptr [bp-6]
	mov	word ptr [bp-8],ax
	inc	word ptr [bp-2]
@2@114:
	cmp	word ptr [bp-2],10
	jl	short @2@58
   ;	
   ;		} // for
   ;		unFollow();
   ;	
	call	far ptr _unFollow
   ;	
   ;		f1=0;
   ;	
	mov	word ptr [bp-4],0
   ;	
   ;		fibo = 0;
   ;	
	mov	word ptr [bp-8],0
   ;	
   ;	 } // main
   ;	
	mov	sp,bp
	pop	bp
	ret	
_main	endp
   ;	
   ;	void interrupt myInt8(void){	//timer
   ;	
	assume	cs:LAB2_TEXT
_myInt8	proc	far
	push	ax
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	bp,DGROUP
	mov	ds,bp
	mov	bp,sp
   ;	
   ;		count++;
   ;	
	inc	word ptr DGROUP:_count
   ;	
   ;		if(count/18 == 5){
   ;	
	mov	ax,word ptr DGROUP:_count
	mov	bx,18
	cwd	
	idiv	bx
	cmp	ax,5
	jne	short @3@86
   ;	
   ;			unFollow();
   ;	
	call	far ptr _unFollow
@3@86:
   ;	
   ;		}
   ;		asm{	//must handle the isr
   ;			PUSHF
   ;	
	PUSHF	
   ;	
   ;			CALL DWORD PTR int8Save
   ;	
	CALL	 DWORD PTR DGROUP:_int8Save
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	pop	di
	pop	si
	pop	ds
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	iret	
_myInt8	endp
   ;	
   ;	void interrupt myInt1(void){
   ;	
	assume	cs:LAB2_TEXT
_myInt1	proc	far
	push	ax
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	bp,DGROUP
	mov	ds,bp
	mov	bp,sp
   ;	
   ;		//check funptr
   ;		flagFstr = 0;
   ;	
	mov	word ptr DGROUP:_flagFstr,0
   ;	
   ;		asm{
   ;			MOV AX,WORD PTR funptrPreSet	//
   ;	
	MOV	 AX,WORD PTR DGROUP:_funptrPreSet	
   ;	
   ;			CMP AX,[BP+18]                  //check ip == funptrPreSet
   ;	
	CMP	 AX,[BP+18]                  
   ;	
   ;			JNE NoPrintfunptr              	//
   ;	
	jne	short @4@170
   ;	
   ;			MOV flagFstr,1                  //
   ;	
	MOV	 DGROUP:_flagFstr,1                  
@4@170:
   ;	
   ;		}
   ;		NoPrintfunptr:
   ;		if(flagFstr == 1){	//xor for same results return 0
   ;	
	cmp	word ptr DGROUP:_flagFstr,1
	jne	short @4@506
   ;	
   ;			printf("procedure %s has been called\n",fstrPreSet);
   ;	
	push	word ptr DGROUP:_fstrPreSet+2
	push	word ptr DGROUP:_fstrPreSet
	push	ds
	mov	ax,offset DGROUP:s@+19
	push	ax
	call	far ptr _printf
	add	sp,8
   ;	
   ;			
   ;			//START:TEST TO SEE WHAT HAPPENS
   ;			//conclusion no segmentations
   ;			csTEST = 0;
   ;	
	mov	word ptr DGROUP:_csTEST,0
   ;	
   ;			ipTEST = 0;
   ;	
	mov	word ptr DGROUP:_ipTEST,0
   ;	
   ;			asm{
   ;				MOV BX,WORD PTR funptrPreSet
   ;	
	MOV	 BX,WORD PTR DGROUP:_funptrPreSet
   ;	
   ;				MOV AX,WORD PTR BX+2
   ;	
	MOV	 AX,WORD PTR BX+2
   ;	
   ;				MOV csTEST,WORD PTR AX
   ;	
	MOV	 DGROUP:_csTEST,WORD PTR AX
   ;	
   ;				MOV ipTEST,WORD PTR BX
   ;	
	MOV	 DGROUP:_ipTEST,WORD PTR BX
   ;	
   ;			}
   ;			printf("cs in funptr = %d, ip  in funptr = %d \n",csTEST,ipTEST);
   ;	
	push	word ptr DGROUP:_ipTEST
	push	word ptr DGROUP:_csTEST
	push	ds
	mov	ax,offset DGROUP:s@+49
	push	ax
	call	far ptr _printf
	add	sp,8
   ;	
   ;			asm{
   ;				MOV AX,[BP+18]
   ;	
	MOV	 AX,[BP+18]
   ;	
   ;				MOV ipTEST,AX
   ;	
	MOV	 DGROUP:_ipTEST,AX
   ;	
   ;				MOV AX,[BP+20]
   ;	
	MOV	 AX,[BP+20]
   ;	
   ;				MOV csTEST,AX
   ;	
	MOV	 DGROUP:_csTEST,AX
   ;	
   ;			}
   ;			printf("cs in stack = %d, ip in stack = %d \n",csTEST,ipTEST);
   ;	
	push	word ptr DGROUP:_ipTEST
	push	word ptr DGROUP:_csTEST
	push	ds
	mov	ax,offset DGROUP:s@+89
	push	ax
	call	far ptr _printf
	add	sp,8
@4@506:
   ;	
   ;			//END:TEST TO SEE WHAT HAPPENS
   ;			
   ;		}
   ;		if(iValuePreSet != *iptrPreSet){
   ;	
	les	bx,dword ptr DGROUP:_iptrPreSet
	mov	ax,word ptr es:[bx]
	cmp	ax,word ptr DGROUP:_iValuePreSet
	je	short @4@562
   ;	
   ;			printf("variable %s has changed from %d to %d\n",strPreSet,iValuePreSet,*iptrPreSet);
   ;	
	les	bx,dword ptr DGROUP:_iptrPreSet
	push	word ptr es:[bx]
	push	word ptr DGROUP:_iValuePreSet
	push	word ptr DGROUP:_strPreSet+2
	push	word ptr DGROUP:_strPreSet
	push	ds
	mov	ax,offset DGROUP:s@+126
	push	ax
	call	far ptr _printf
	add	sp,12
   ;	
   ;			iValuePreSet = *iptrPreSet;
   ;	
	les	bx,dword ptr DGROUP:_iptrPreSet
	mov	ax,word ptr es:[bx]
	mov	word ptr DGROUP:_iValuePreSet,ax
@4@562:
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	pop	di
	pop	si
	pop	ds
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	iret	
_myInt1	endp
   ;	
   ;	void follow(int *iptr,char str[],void (*funptr)(), char fstr[]){
   ;	
	assume	cs:LAB2_TEXT
_follow	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;		if(followFlag == 1){	//restoring the system then setting it as requested
   ;	
	cmp	word ptr DGROUP:_followFlag,1
	jne	short @5@86
   ;	
   ;			unFollow();
   ;	
	call	far ptr _unFollow
@5@86:
   ;	
   ;		}
   ;		iValuePreSet = *iptr;
   ;	
	les	bx,dword ptr [bp+6]
	mov	ax,word ptr es:[bx]
	mov	word ptr DGROUP:_iValuePreSet,ax
   ;	
   ;		iptrPreSet = iptr;
   ;	
	mov	ax,word ptr [bp+8]
	mov	dx,word ptr [bp+6]
	mov	word ptr DGROUP:_iptrPreSet+2,ax
	mov	word ptr DGROUP:_iptrPreSet,dx
   ;	
   ;		funptrPreSet = funptr;
   ;	
	mov	ax,word ptr [bp+16]
	mov	dx,word ptr [bp+14]
	mov	word ptr DGROUP:_funptrPreSet+2,ax
	mov	word ptr DGROUP:_funptrPreSet,dx
   ;	
   ;		strPreSet = str;
   ;	
	mov	ax,word ptr [bp+12]
	mov	dx,word ptr [bp+10]
	mov	word ptr DGROUP:_strPreSet+2,ax
	mov	word ptr DGROUP:_strPreSet,dx
   ;	
   ;		fstrPreSet = fstr;
   ;	
	mov	ax,word ptr [bp+20]
	mov	dx,word ptr [bp+18]
	mov	word ptr DGROUP:_fstrPreSet+2,ax
	mov	word ptr DGROUP:_fstrPreSet,dx
   ;	
   ;		//printf("funptr = %d\n",funptr);
   ;		count = 0;
   ;	
	mov	word ptr DGROUP:_count,0
   ;	
   ;		followFlag = 1;
   ;	
	mov	word ptr DGROUP:_followFlag,1
   ;	
   ;		
   ;		
   ;		int1Save = getvect(1);
   ;	
	mov	ax,1
	push	ax
	call	far ptr _getvect
	pop	cx
	mov	word ptr DGROUP:_int1Save+2,dx
	mov	word ptr DGROUP:_int1Save,ax
   ;	
   ;		setvect(1,myInt1);
   ;	
	mov	ax,seg _myInt1
	push	ax
	mov	ax,offset _myInt1
	push	ax
	mov	ax,1
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;		int8Save = getvect(8);
   ;	
	mov	ax,8
	push	ax
	call	far ptr _getvect
	pop	cx
	mov	word ptr DGROUP:_int8Save+2,dx
	mov	word ptr DGROUP:_int8Save,ax
   ;	
   ;		setvect(8,myInt8);
   ;	
	mov	ax,seg _myInt8
	push	ax
	mov	ax,offset _myInt8
	push	ax
	mov	ax,8
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;		asm{
   ;			pushf
   ;	
	pushf	
   ;	
   ;			pop ax
   ;	
	pop	 ax
   ;	
   ;			or ax,100000000B	 //1 0000 0000B -> 256 = 2^8
   ;	
	or	 ax,100000000B	 
   ;	
   ;			push ax
   ;	
	push	 ax
   ;	
   ;			popf
   ;	
	popf	
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_follow	endp
   ;	
   ;	void unFollow(){
   ;	
	assume	cs:LAB2_TEXT
_unFollow	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;		asm{
   ;			pushf
   ;	
	pushf	
   ;	
   ;			pop ax
   ;	
	pop	 ax
   ;	
   ;			and ax,1111111011111111B	//2^16 - 1 - 256 = 1111 1110 1111 1111
   ;	
	and	 ax,1111111011111111B	
   ;	
   ;			push ax
   ;	
	push	 ax
   ;	
   ;			popf
   ;	
	popf	
   ;	
   ;		}
   ;		setvect(1,int1Save);
   ;	
	push	word ptr DGROUP:_int1Save+2
	push	word ptr DGROUP:_int1Save
	mov	ax,1
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;		setvect(8,int8Save);
   ;	
	push	word ptr DGROUP:_int8Save+2
	push	word ptr DGROUP:_int8Save
	mov	ax,8
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;		//printf("unfollow\n");
   ;		followFlag = 0;
   ;	
	mov	word ptr DGROUP:_followFlag,0
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_unFollow	endp
LAB2_TEXT	ends
_BSS	segment word public 'BSS'
_ipTEST	label	word
	db	2 dup (?)
_csTEST	label	word
	db	2 dup (?)
_count	label	word
	db	2 dup (?)
_flagFstr	label	word
	db	2 dup (?)
_funptrPreSet	label	dword
	db	4 dup (?)
_iValuePreSet	label	word
	db	2 dup (?)
_iptrPreSet	label	dword
	db	4 dup (?)
_fstrPreSet	label	dword
	db	4 dup (?)
_strPreSet	label	dword
	db	4 dup (?)
_int8Save	label	dword
	db	4 dup (?)
_int1Save	label	dword
	db	4 dup (?)
	?debug	C E9
_BSS	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'fibo'
	db	0
	db	'sleep'
	db	0
	db	'f1'
	db	0
	db	'func'
	db	0
	db	'procedure %s has been called'
	db	10
	db	0
	db	'cs in funptr = %d, ip  in funptr = %d '
	db	10
	db	0
	db	'cs in stack = %d, ip in stack = %d '
	db	10
	db	0
	db	'variable %s has changed from %d to %d'
	db	10
	db	0
_DATA	ends
LAB2_TEXT	segment byte public 'CODE'
LAB2_TEXT	ends
	public	_main
	public	_ipTEST
	public	_csTEST
	public	_count
	public	_flagFstr
	public	_followFlag
	public	_funptrPreSet
	public	_iValuePreSet
	public	_iptrPreSet
	public	_fstrPreSet
	public	_strPreSet
	public	_func
	public	_unFollow
	public	_follow
	public	_myInt8
	public	_int8Save
	public	_myInt1
	public	_int1Save
	extrn	_setvect:far
	extrn	_getvect:far
	extrn	_sleep:far
	extrn	_printf:far
_s@	equ	s@
	end
