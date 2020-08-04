.model large

.data
count dw ?
int1save dw ?
int1saveSeg dw ?
.code

;------------------------------------------------

public _Start_IMUL_Count
_Start_IMUL_Count proc far

push bp
mov bp,sp
push cx
push es
push ax

;-------------Int1Save = getvect (1);

mov ax,0
xor ax,ax
mov es,ax
mov ax,es:[4]
mov word ptr int1save,ax
mov ax,es:[6]
mov word ptr int1saveSeg,ax

;------------------setvect (1,multy_int);

cli
mov word ptr es:[4],offset multy_int
mov word ptr es:[6],cs
sti
pop ax
pop es
;------------------------------------


;------------turn on the trap flag
pushf
pop ax
or ax,100000000B
push ax
popf
;--------------------count = 0
mov cx,0
mov count,cx


pop cx
pop bp
ret
_Start_IMUL_Count endp

;-------------------------------------------------

public _Return_IMUL_Count
_Return_IMUL_Count proc far

push bp
mov bp,sp
;------------turn off the trap flag
pushf 
pop ax
and ax,1111111011111111B
push ax
popf
;--------------------return count


;----------------------------setvect (1,Int1Save);
push es
push ax
mov ax,0
mov es,ax
cli
mov ax,word ptr int1save
mov es:[4],ax
mov ax,word ptr int1saveSeg
mov es:[6],ax
sti
pop ax
pop es
;-------------------------------------------

mov ax,count
pop bp
ret
_Return_IMUL_Count endp

;-------------------------------------------------


multy_int proc far

	push bp
	mov bp,sp
	push    ax
	push    bx
	push    cx
	push    dx
	push    es
	push    ds
	push    si
	push    di

	mov es,[bp+4] ; es = cs
	mov bx,[bp+2] ; bx = ip
	mov ax,es:[bx]
	cmp al,247
	jne label1
	inc count ; count++
label1:     	
	
	pop di
	pop si
	pop ds
	pop es
	pop dx
	pop cx
	pop bx
	pop ax
pop bp

	iret
multy_int endp

END


