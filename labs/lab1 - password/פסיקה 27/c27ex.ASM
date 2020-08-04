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
	?debug	S "c_intr1d.c"
	?debug	C E9A469793B0A635F696E747231642E63
	?debug	C E90018521815443A5C54435C494E434C5544455C737464696F2E68
	?debug	C E90018521815443A5C54435C494E434C5544455C5F646566732E68
	?debug	C E90018521815443A5C54435C494E434C5544455C5F6E756C6C2E68
	?debug	C E90018521813443A5C54435C494E434C5544455C646F732E68
C_INTR1D_TEXT	segment byte public 'CODE'
C_INTR1D_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:C_INTR1D_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
C_INTR1D_TEXT	segment byte public 'CODE'
   ;	
   ;	void interrupt Ctrl_Break_Handler(void)
   ;	
	assume	cs:C_INTR1D_TEXT
_Ctrl_Break_Handler	proc	far
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
   ;	   {
   ;	     Ctrl_Break_Flag = 1;
   ;	
	mov	word ptr DGROUP:_Ctrl_Break_Flag,1
   ;	
   ;	     printf("C: Ctrl-Break has been pressed.\n");
   ;	
	push	ds
	mov	ax,offset DGROUP:s@
	push	ax
	call	far ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	   } /* Ctrl_Break_Handler */
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
_Ctrl_Break_Handler	endp
   ;	
   ;	void main(void)
   ;	
	assume	cs:C_INTR1D_TEXT
_main	proc	far
	push	bp
	mov	bp,sp
   ;	
   ;	 {
   ;	   Int27Save = getvect(27);               /* Preserve old pointer */
   ;	
	mov	ax,27
	push	ax
	call	far ptr _getvect
	pop	cx
	mov	word ptr DGROUP:_Int27Save+2,dx
	mov	word ptr DGROUP:_Int27Save,ax
   ;	
   ;	   setvect(27, Ctrl_Break_Handler);       /* Set entry to new handler */
   ;	
	mov	ax,seg _Ctrl_Break_Handler
	push	ax
	mov	ax,offset _Ctrl_Break_Handler
	push	ax
	mov	ax,27
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;	   printf("C: Press Ctrl-Break to continue.\n");
   ;	
	push	ds
	mov	ax,offset DGROUP:s@+33
	push	ax
	call	far ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	
   ;	   Ctrl_Break_Flag = 0;
   ;	
	mov	word ptr DGROUP:_Ctrl_Break_Flag,0
	jmp	short @2@58
@2@58:
   ;	
   ;	
   ;	   while (Ctrl_Break_Flag == 0)
   ;	
	cmp	word ptr DGROUP:_Ctrl_Break_Flag,0
	je	short @2@58
   ;	
   ;	       ;   /* Do nothing */
   ;	
   ;	   printf("C: Terminating ...\n");
   ;	
	push	ds
	mov	ax,offset DGROUP:s@+67
	push	ax
	call	far ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	
   ;	   setvect(27,Int27Save);               /* Restore old pointer */
   ;	
	push	word ptr DGROUP:_Int27Save+2
	push	word ptr DGROUP:_Int27Save
	mov	ax,27
	push	ax
	call	far ptr _setvect
	add	sp,6
   ;	
   ;	
   ;	 } /* main */
   ;	
	pop	bp
	ret	
_main	endp
C_INTR1D_TEXT	ends
_BSS	segment word public 'BSS'
_Int27Save	label	dword
	db	4 dup (?)
_Ctrl_Break_Flag	label	word
	db	2 dup (?)
	?debug	C E9
_BSS	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'C: Ctrl-Break has been pressed.'
	db	10
	db	0
	db	'C: Press Ctrl-Break to continue.'
	db	10
	db	0
	db	'C: Terminating ...'
	db	10
	db	0
_DATA	ends
C_INTR1D_TEXT	segment byte public 'CODE'
C_INTR1D_TEXT	ends
	public	_main
	public	_Ctrl_Break_Handler
	public	_Int27Save
	public	_Ctrl_Break_Flag
	extrn	_setvect:far
	extrn	_getvect:far
	extrn	_printf:far
_s@	equ	s@
	end
