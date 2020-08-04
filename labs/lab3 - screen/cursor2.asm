;
; Cursor2.asm
;
;This program demonstrates cursor manipulation.
;
;
Stak              SEGMENT PARA STACK 'STACK'
DB                 256 DUP(0)
Stak              ENDS
;
Data               SEGMENT PARA PUBLIC 'Data'
CursorPos          DW      0     ; Number of lines scrolled down
Base               DW      0
;
Data               ENDS
Code               SEGMENT PARA PUBLIC 'Code'
                   .386  ; Enable 386 commands
SetCursorPos          PROC    NEAR
; Set Cursor to position in BX
; Input: BX
; Output: None
;
  MOV              DX,3D4h  ; Point to 3D4H - 3D5H port pair
  MOV              AL,14    ; Address of cursor register pos high byte
  MOV              AH,BH    ; Get desired value of cursor pos high byte
  OUT              DX,AX    ; Port(3D4h) = 14, Port(3D5h) = Value of BH
;
  MOV              AL,15    ; Address of cursor register pos low byte
  MOV              AH,BL    ; Get desired value pf cursor pos low byte
  OUT              DX,AX    ; Port(3D4h) = 15, Port(3D5h) = Value of BL
;
  RET                       ; Return to caller
SetCursorPos          ENDP
;
Start              PROC    FAR
;
;STANDARD PROGRAM PROLOGUE
;
  ASSUME           CS:Code
  PUSH             DS          ; Save PSP segment address
  MOV              AX,0
  PUSH             AX          ; Save INT 20h address offset (PSP+0)
  MOV              AX,Data
  MOV              DS,AX       ; Establish Data segment addressability
  ASSUME           DS:Data
;
;Part1 : Initialize the display adapter
;
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
; Write 'A' in mid screen
;
  MOV              BYTE PTR ES:[2*(12*40+20)],'A'
;
; Set the cursor address registers
;
  MOV              BX,12*40+20
  CALL             SetCursorPos
;
;PART 2 : Wait for key strike
;
; Wait for key
;
NextLoop:
  MOV              AH,0       ; Wait and read key
  INT              16h        ;
  CMP              AH,1       ; Is it Esc?
  JE               ToReturn   ; Yes - Return to DOS
;
;  Not esc key - change cursor
;
  ; 3D4H  Graphics adapter address register port
  ; 3D5H  Graphics adapter data register port
;
    MOV             DX,3D4h  ; Point TO 3D4h - 3D5h port pair
    MOV             AX,000Ah ; Cursor start address (0Ah) - Value 0 (00h)
    OUT             DX,AX    ; Port(3D4h) = 0Ah, Port(3D5h) = 01h
    MOV             AX,0F0Bh ; Cursor end address - Value 15 (0Eh)
    OUT             DX,AX    ; Port(3D4h) = 0Bh, Port(3D5h) = 0Eh
;
; Wait for key
;
  MOV              AH,0
  INT              16h
  CMP              AH,1       ; Is it Esc?
  JE               ToReturn   ; Yes - Return to DOS
;
    MOV             DX,3D4h   ; Point to 3D4H - 3D5H port pair
    MOV             AX,0E0Ah  ; Cursor start address (0Ah) - Value 14 (0Dh)
    OUT             DX,AX     ; Port(3D4h) = 0Ah, Port(3D5h) = 01h
    MOV             AX,0F0Bh  ; Cursor end address - Value 15 (0Eh)
    OUT             DX,AX     ; Port(3D4h) = 0Bh, Port(3D5h) = 0Eh
;
  JMP              NextLoop   ; Repeat main loop
;
ToReturn:
                   MOV  AX,2
                   INT  10h
                   RET
Start              ENDP
Code               ENDS
  END              Start

