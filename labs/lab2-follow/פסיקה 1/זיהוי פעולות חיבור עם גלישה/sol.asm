.model large

.data
flag db 0
int1save dw ?
int1saveSeg dw ?
printWarning db 'warning: Overflow on ADD',13,10,'$'
.code

;------------------------------------------------

public _Start_track
_Start_track proc far

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

;------------------setvect (1,add_identify);

cli
mov word ptr es:[4],offset add_identify
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

pop cx
pop bp
ret
_Start_track endp

;-------------------------------------------------

public _end_track
_end_track proc far

push bp
mov bp,sp
;------------turn off the trap flag
pushf 
pop ax
and ax,1111111011111111B
push ax
popf

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

mov ax,0
pop bp
ret
_end_track endp

;-------------------------------------------------


add_identify proc far

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

	jno continue
	mov ax, 0
	cmp flag, al	; if 0, last function wasn't add. if 1, last function was add
	je continue
	
	
	mov ah,9
	mov dx,offset printWarning
	int 21h
	
	
continue:
	mov es,[bp+4] ; es = cs	     ; segment		
	mov bx,[bp+2] ; bx = ip      ; offset
	mov ax,es:[bx]
	cmp al,03h
	jne put_zero
	
	mov al,1		; flag = 1 -> next function is add
	mov flag,al
	jmp label1
	
put_zero:
	mov al,0		; flag = 0 -> next function is not add
	mov flag,al
	
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
add_identify endp

END


