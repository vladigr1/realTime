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
	?debug	S "create.c"
	?debug	C E9ED55B750086372656174652E63
	?debug	C E9D3449E50232E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C636F6E662E68
	?debug	C E9D4449E50252E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C6B65726E656C2E68
	?debug	C E9D5449E50232E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C70726F632E68
	?debug	C E9D4449E50222E2E5C2E2E5C54435C42494E5C58494E553457494E+
	?debug	C 5C4E45575352435C6D656D2E68
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
   ;	SYSCALL create(procaddr,ssize,priority,namep,nargs,args)
   ;	
	assume	cs:_TEXT
_create	proc	near
	push	bp
	mov	bp,sp
	sub	sp,14
   ;	
   ;	int (*procaddr)();		/* procedure address			*/
   ;	word ssize;			/* stack size in words			*/
   ;	short priority;			/* process priority > 0			*/
   ;	char *namep;			/* name (for debugging)			*/
   ;	int nargs;			/* number of args that follow		*/
   ;	int args;			/* arguments (treated like an array)	*/
   ;	{
   ;		int	pid;			/* stores new process id	*/
   ;		struct	pentry	*pptr;		/* pointer to proc. table entry */
   ;		int	i;			/* loop variable		*/
   ;		int	*a;			/* points to list of args	*/
   ;		char	*saddr;			/* start of stack address	*/
   ;		int	*sp;			/* stack pointer		*/
   ;		int	ps;			/* saved processor status	*/
   ;	
   ;		disable(ps);
   ;	
	call	near ptr _sys_disabl
	mov	word ptr [bp-14],ax
   ;	
   ;		ssize = roundew(ssize);
   ;	
	mov	ax,word ptr [bp+6]
	add	ax,3
	and	ax,65532
	mov	word ptr [bp+6],ax
   ;	
   ;		if ( ssize < MINSTK || priority < 1 ||
   ;	
   ;	
   ;			(pid=newpid()) == SYSERR ||
   ;			((saddr=getstk(ssize)) == NULL ) ) {
   ;	
	cmp	word ptr [bp+6],256
	jb	short @1@142
	cmp	word ptr [bp+8],1
	jl	short @1@142
	call	near ptr _newpid
	mov	word ptr [bp-2],ax
	cmp	ax,65535
	je	short @1@142
	push	word ptr [bp+6]
	call	near ptr _getmem
	pop	cx
	mov	word ptr [bp-10],ax
	or	ax,ax
	jne	short @1@198
@1@142:
   ;	
   ;			restore(ps);
   ;	
	push	word ptr [bp-14]
	call	near ptr _sys_restor
	pop	cx
   ;	
   ;			return(SYSERR);
   ;	
	mov	ax,65535
@1@170:
	jmp	@1@534
@1@198:
   ;	
   ;		}
   ;	//mine
   ;	//------------------------------------------------------------------------
   ;		kprintf("nargs, %d , args %d\n",nargs,args);
   ;	
	push	word ptr [bp+14]
	push	word ptr [bp+12]
	mov	ax,offset DGROUP:s@
	push	ax
	call	near ptr _kprintf
	add	sp,6
   ;	
   ;		paTable[pid] = procaddr;
   ;	
	mov	bx,word ptr [bp-2]
	shl	bx,1
	mov	ax,word ptr [bp+4]
	mov	word ptr DGROUP:_paTable[bx],ax
   ;	
   ;	/*------------------------------------------------------------------------
   ;	*/
   ;		numproc++;
   ;	
	inc	word ptr DGROUP:_numproc
   ;	
   ;		pptr = &proctab[pid];
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,29
	imul	dx
	add	ax,offset DGROUP:_proctab
	mov	word ptr [bp-4],ax
   ;	
   ;		pptr->pstate = PRSUSP;
   ;	
	mov	bx,word ptr [bp-4]
	mov	byte ptr [bx],6
   ;	
   ;		for (i=0 ; i<PNMLEN ; i++)
   ;	
	mov	word ptr [bp-6],0
	jmp	short @1@366
@1@226:
   ;	
   ;			pptr->pname[i] = (*namep ? *namep++ : ' ');
   ;	
	mov	bx,word ptr [bp+10]
	cmp	byte ptr [bx],0
	je	short @1@282
	mov	bx,word ptr [bp+10]
	inc	word ptr [bp+10]
	mov	al,byte ptr [bx]
	jmp	short @1@310
@1@282:
	mov	al,32
@1@310:
	mov	bx,word ptr [bp-4]
	add	bx,word ptr [bp-6]
	mov	byte ptr [bx+15],al
	inc	word ptr [bp-6]
@1@366:
	cmp	word ptr [bp-6],9
	jl	short @1@226
   ;	
   ;		pptr->pname[PNMLEN]='\0';
   ;	
	mov	bx,word ptr [bp-4]
	mov	byte ptr [bx+24],0
   ;	
   ;		pptr->pprio = priority;
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp+8]
	mov	word ptr [bx+1],ax
   ;	
   ;		pptr->phasmsg = 0;		/* no message			*/
   ;	
	mov	bx,word ptr [bp-4]
	mov	word ptr [bx+7],0
   ;	
   ;		pptr->pbase = saddr;
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp-10]
	mov	word ptr [bx+11],ax
   ;	
   ;		pptr->plen = ssize;
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp+6]
	mov	word ptr [bx+13],ax
   ;	
   ;		sp = (int *) (saddr+ssize);	/* simulate stack pointer	*/
   ;	
	mov	ax,word ptr [bp-10]
	add	ax,word ptr [bp+6]
	mov	word ptr [bp-12],ax
   ;	
   ;		sp -= 4;			/* a little elbow room		*/
   ;	
	sub	word ptr [bp-12],8
   ;	
   ;		pptr->pargs = nargs;
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp+12]
	mov	word ptr [bx+25],ax
   ;	
   ;		a = (&args) + nargs;		/* point past last argument	*/
   ;	
	mov	ax,word ptr [bp+12]
	shl	ax,1
	lea	dx,word ptr [bp+14]
	add	ax,dx
	mov	word ptr [bp-8],ax
	jmp	short @1@478
@1@422:
   ;	
   ;		for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
   ;			*(--sp) = *(--a);	/*  onto created process' stack	*/
   ;	
	sub	word ptr [bp-12],2
	sub	word ptr [bp-8],2
	mov	bx,word ptr [bp-8]
	mov	ax,word ptr [bx]
	mov	bx,word ptr [bp-12]
	mov	word ptr [bx],ax
	dec	word ptr [bp+12]
@1@478:
	cmp	word ptr [bp+12],0
	jg	short @1@422
   ;	
   ;		*(--sp) = (int)INITRET;		/* push on return address	*/
   ;	
	sub	word ptr [bp-12],2
	mov	bx,word ptr [bp-12]
	mov	word ptr [bx],offset _userret
   ;	
   ;		*(--sp) = (int)procaddr;	/* simulate a context switch	*/
   ;	
	sub	word ptr [bp-12],2
	mov	bx,word ptr [bp-12]
	mov	ax,word ptr [bp+4]
	mov	word ptr [bx],ax
   ;	
   ;		--sp ;				/* 1 word for bp		*/
   ;	
	sub	word ptr [bp-12],2
   ;	
   ;		*(--sp) = INITF;		/* FLAGS value			*/
   ;	
	sub	word ptr [bp-12],2
	mov	bx,word ptr [bp-12]
	mov	word ptr [bx],512
   ;	
   ;		sp -= 2;			/* 2 words for si and di	*/
   ;	
	sub	word ptr [bp-12],4
   ;	
   ;		pptr->pregs = (char *)sp;	/* save for context switch	*/
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp-12]
	mov	word ptr [bx+9],ax
   ;	
   ;		pptr->paddr = procaddr;
   ;	
	mov	bx,word ptr [bp-4]
	mov	ax,word ptr [bp+4]
	mov	word ptr [bx+27],ax
   ;	
   ;		restore(ps);
   ;	
	push	word ptr [bp-14]
	call	near ptr _sys_restor
	pop	cx
   ;	
   ;		return(pid);
   ;	
	mov	ax,word ptr [bp-2]
	jmp	@1@170
@1@534:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_create	endp
   ;	
   ;	LOCAL	newpid()
   ;	
	assume	cs:_TEXT
newpid	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		int	pid;			/* process id to return		*/
   ;		int	i;
   ;	
   ;		for (i=1 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
   ;	
	mov	word ptr [bp-2],1
	jmp	short @2@170
@2@58:
   ;	
   ;			if (proctab[i].pstate == PRFREE)
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,29
	imul	dx
	mov	bx,ax
	cmp	byte ptr DGROUP:_proctab[bx],2
	jne	short @2@142
   ;	
   ;				return i;
   ;	
	mov	ax,word ptr [bp-2]
@2@114:
	jmp	short @2@226
@2@142:
	inc	word ptr [bp-2]
@2@170:
	cmp	word ptr [bp-2],30
	jl	short @2@58
   ;	
   ;		}
   ;		return(SYSERR);
   ;	
	mov	ax,65535
	jmp	short @2@114
@2@226:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
newpid	endp
	?debug	C E9
_TEXT	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'nargs, %d , args %d'
	db	10
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	extrn	_kprintf:near
_newpid	equ	newpid
	public	_create
	extrn	_userret:near
	extrn	_paTable:word
	extrn	_getmem:near
	extrn	_numproc:word
	extrn	_proctab:word
	extrn	_sys_restor:near
	extrn	_sys_disabl:near
_s@	equ	s@
	end
