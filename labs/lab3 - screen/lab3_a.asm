;
; lab3_a support lab3_m
;
;This program Assembly procedures required to the lab
;
;
			.MODEL LARGE
			.STACK 100h
			.DATA
		Sname1 			   DB	   'v',4h,'l',4h ,'a',4h ,'d',4h,' ',4h
		Sname2 			   DB	   'a',4h,'l',4h ,'e',4h ,'x',4h
			.CODE
                   .386  ; Enable 386 commands
;				   
;
		_clear_kbuffer				PROC FAR
		PUBLIC _clear_kbuffer
;clear keyboard buffer
L1:
			cli
			mov ah,1h 			; check keyboard buffer
			int 16h 			; any key pressed?
			jz  finish_clear 			; no: exit now
			mov ah,10h 			; yes: remove from buffer
			int 16h
			jmp L1 				; no: check buffer again				
finish_clear:
			sti
			RET
		_clear_kbuffer				ENDP
;
;		
		_SetCursorPos				PROC    FAR
		PUBLIC _SetCursorPos
			PUSH BP
			MOV BP,SP
			MOV              DX,3D4h  ; Point to 3D4H - 3D5H port pair
			MOV              AL,14    ; Address of cursor register pos high byte
			MOV              AH,[BP+7]    ; Get desired value of cursor pos high byte
			OUT              DX,AX    ; Port(3D4h) = 14, Port(3D5h) = Value of BH
			;
			MOV              AL,15    ; Address of cursor register pos low byte
			MOV              AH,[BP+6]    ; Get desired value pf cursor pos low byte
			OUT              DX,AX    ; Port(3D4h) = 15, Port(3D5h) = Value of BL
			;
			POP BP
			RET                       ; Return to caller	
		_SetCursorPos          ENDP
;
;
		_start_screen			PROC    FAR
		PUBLIC _start_screen
; set mode 40 by 25 color image
			MOV              AH,0          ; Select function = 'Set mode'
			MOV              AL,1          ; 40 by 25 color image
			INT              10h           ; Adapter initialized. Page 0 displayed
;
			MOV              AX,0B800h     ; Segment address of memory on color adapter
;
			MOV              ES,AX         ; Set up extra segment register
			MOV              DI,0          ; Initial offset address into segment
			MOV              AL,' '        ; Character space to fill adapter memory
			MOV              AH,0Eh        ; Attribute byte : Intense yellow
			MOV              CX,1000       ; Initialize count, 1 Screen
			CLD                            ; Write forward
			REP                                           ; Write
;
; Write 'vlad' in mid screen
;
			MOV SI, offset Sname1
			MOV DI, 2*(12*40+20)
;
			MOV              AX,0B800h     ; Segment address of memory on color adapter
;
			MOV              ES,AX         ; Set up extra segment register
			MOV CX,18
			REP MOVSB
			RET
		_start_screen ENDP
;
;			
		_end_screen				PROC    FAR
		PUBLIC _end_screen
; set back to regular screen
			MOV  AX,2
			INT  10h
			RET
		_end_screen ENDP
;
;
		_myInt9					PROC FAR
		PUBLIC _myInt9
			EXTRN _oldint9
			EXTRN _CursorPos
			EXTRN _sc_pressed

			push bp							;
			mov bp,sp						;    
			push    ax						;    
			push    bx						;    start
			push    cx						; regular interrupt pre-set  
			push    dx						;    
			push    es						;    
			push    ds						;    
			push    si						;    
			push    di						;       
;		
			CLI
			PUSHF
			CALL dword ptr _oldint9
			in 	al,60h    					;input
			test Al,128						;mute press/unpress
			jnz finish9
;
			mov byte ptr [_sc_pressed],al		;sc_pressed = cur scan code relent only esc pressed exit the program
;
			mov ah,2                     	;recognize shift
			int 16h                         ;
			test al,3
			jz notShift
			pushf
			push ds
;
			mov ax,0b800h
			mov es,ax
			mov ds,ax
			mov si,1918						;959*2 = 1918 one before last line
			mov di,1998						;999*2 1998 last line
			mov cx,1920						;960*2 =1920
			pushf
			pop ax
			or ax,1024
			push ax
			popf
			rep movsb 
;			
			POP ds
			POPF
;fixing first line
			mov cx,40
			mov ax,0700h					;set blink and no letter
			mov bx,0
firstLine:
			mov es:[bx],ax
			add bx,2
			loop firstLine
			jmp finish9
notShift:
			mov ah,1                     	;recognize charcter
			int 16h                         ;
			cmp al,0
			jne haveAS
;no ascii test arrow then letters
			cmp ah,75						;leftarrow
			jne elseUp
			mov bx,_CursorPos
			dec bx
			cmp bx,999
			ja finish9
			mov word ptr [_CursorPos],bx
			jmp finish9
elseUp:
			cmp ah,72						;uparrow
			jne elseRight
			mov bx,_CursorPos
			sub bx,40
			cmp bx,999
			ja finish9
			mov word ptr [_CursorPos],bx
			jmp finish9
elseRight:
			cmp ah,77						 ;rightarrow
			jne elseDown
			mov bx,_CursorPos
			inc bx
			cmp bx,999
			ja finish9
			mov word ptr [_CursorPos],bx
			jmp finish9
elseDown:	
			cmp ah,80						 ;downarrow
			jne elseF1
			mov bx,_CursorPos
			add bx,40
			cmp bx,999
			ja finish9
			mov word ptr [_CursorPos],bx
			jmp finish9
elseF1:	
			cmp ah,59						 ;F1
			jne elseF2
			mov dx,3d4h
			mov ax,000Ah
			out dx,ax
			mov ax,0F0Bh
			out dx,ax
			jmp finish9	
elseF2:	
			cmp ah,60						 ;F2
			jne elseStartRow
			mov dx,3d4h
			mov ax,0E0Ah
			out dx,ax
			mov ax,0F0Bh
			out dx,ax
			jmp finish9				
elseStartRow:
			mov ax,_CursorPos				;start row is all no ascii which arent arrows				
			mov bl,40                       
			div bl							;ah reminder
			mov al,ah                       ;
			xor ah,ah                       ;
			mov bx,_CursorPos               ;
			sub bx,ax                       ;                        
			mov _CursorPos, bx
			jmp finish9
haveAS:
;last change		
;			mov ah,11h                      ;recognize charcter
;			int 16h                         ;
;			xor dx,dx						;
;			mov dl,al						;
;

			cmp al,'a'
			jb finish9
			cmp al,'z'
			ja finish9
			mov bx,0b800h					;letter
			mov es,bx
			mov bx,_CursorPos				
			shl bx,1                       	;2*cur exact location
			mov es:bx,al
;
			
finish9:	
			mov ax,_CursorPos
			push ax
			call _SetCursorPos
			pop AX
			call _clear_kbuffer
			STI
			pop di							;
			pop si							;
			pop ds							;
			pop es							;
			pop dx							;	end
			pop cx							; regular interrupt pre-set
			pop bx							;
			pop ax							;
			pop bp							;
											;
			iret							;
		_myInt9 ENDP
;		
;		


			END
				  