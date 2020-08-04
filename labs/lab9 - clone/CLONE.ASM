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
	?debug	S "clone.c"
	?debug	C E97459B75007636C6F6E652E63
	?debug	C E9D3449E50232E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C636F6E662E68
	?debug	C E9D4449E50252E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C6B65726E656C2E68
	?debug	C E9D5449E50232E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C70726F632E68
	?debug	C E95A44B7500A6F726967696E616C2E68
_TEXT	segment byte public 'CODE'
_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	SYSCALL clone(char name[], int no_of_params,   int param1, ...){
   ;	
	assume	cs:_TEXT
_clone	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
   ;	
   ;		//		[bp+4], 		[bp+6],				[bp+8], ...
   ;		int ps;
   ;		int newPid;
   ;		disable(ps);
   ;	
	call	near ptr _sys_disabl
	mov	word ptr [bp-2],ax
   ;	
   ;		kprintf("clone\n");
   ;	
	mov	ax,offset DGROUP:s@
	push	ax
	call	near ptr _kprintf
	pop	cx
   ;	
   ;		param1 = 1;
   ;	
	mov	word ptr [bp+8],1
   ;	
   ;		asm{
   ;			mov cx,no_of_params
   ;	
	mov	 cx,[bp+6]
   ;	
   ;			mov newPid,cx
   ;	
	mov	 [bp-4],cx
   ;	
   ;			mov si,6
   ;	
	mov	 si,6
   ;	
   ;			add si,no_of_params
   ;	
	add	 si,[bp+6]
   ;	
   ;			add si,no_of_params
   ;	
	add	 si,[bp+6]
   ;	
   ;			dec si
   ;	
	dec	 si
   ;	
   ;			dec si
   ;	
	dec	 si
@1@254:
   ;	
   ;		}
   ;	start_loop:
   ;		kprintf("%d \n",newPid);
   ;	
	push	word ptr [bp-4]
	mov	ax,offset DGROUP:s@+7
	push	ax
	call	near ptr _kprintf
	pop	cx
	pop	cx
   ;	
   ;		asm{
   ;			mov ax, word ptr [bp+si]
   ;	
	mov	 ax, word ptr [bp+si]
   ;	
   ;			mov newPid,ax
   ;	
	mov	 [bp-4],ax
   ;	
   ;			push word ptr [bp+si]
   ;	
	push	 word ptr [bp+si]
   ;	
   ;			dec si
   ;	
	dec	 si
   ;	
   ;			dec si
   ;	
	dec	 si
   ;	
   ;			LOOP start_loop
   ;	
	loop	short @1@254
   ;	
   ;		}
   ;		newPid  = create(paTable[currpid],proctab[currpid].plen,
   ;	
   ;	
   ;		proctab[currpid].pprio,name,no_of_params,&param1);
   ;	
	lea	ax,word ptr [bp+8]
	push	ax
	push	word ptr [bp+6]
	push	word ptr [bp+4]
	mov	ax,word ptr DGROUP:_currpid
	mov	dx,29
	imul	dx
	mov	bx,ax
	push	word ptr DGROUP:_proctab[bx+1]
	mov	ax,word ptr DGROUP:_currpid
	mov	dx,29
	imul	dx
	mov	bx,ax
	push	word ptr DGROUP:_proctab[bx+13]
	mov	bx,word ptr DGROUP:_currpid
	shl	bx,1
	push	word ptr DGROUP:_paTable[bx]
	call	near ptr _create
	add	sp,12
	mov	word ptr [bp-4],ax
   ;	
   ;		if (newPid == SYSERR){
   ;	
	cmp	word ptr [bp-4],65535
	jne	short @1@534
   ;	
   ;			restore(ps);
   ;	
	push	word ptr [bp-2]
	call	near ptr _sys_restor
	pop	cx
   ;	
   ;			return SYSERR;
   ;	
	mov	ax,65535
@1@506:
	jmp	short @1@562
@1@534:
   ;	
   ;		}
   ;		resume(newPid);	//newPid will be the pid of new proc
   ;	
	push	word ptr [bp-4]
	call	near ptr _resume
	pop	cx
   ;	
   ;		restore(ps);
   ;	
	push	word ptr [bp-2]
	call	near ptr _sys_restor
	pop	cx
   ;	
   ;		return OK;
   ;	
	mov	ax,1
	jmp	short @1@506
@1@562:
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_clone	endp
	?debug	C E9
_TEXT	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'clone'
	db	10
	db	0
	db	'%d '
	db	10
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	extrn	_resume:near
	extrn	_create:near
	extrn	_kprintf:near
	public	_clone
	extrn	_paTable:word
	extrn	_currpid:word
	extrn	_proctab:word
	extrn	_sys_restor:near
	extrn	_sys_disabl:near
_s@	equ	s@
	end
