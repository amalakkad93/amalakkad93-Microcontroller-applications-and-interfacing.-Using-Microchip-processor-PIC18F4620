#include <P18F4620.inc>
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF 
    
Input_A	    equ	    0x20
Input_C	    equ     0x21
Result	    equ     0x22
    
    ORG	    0x0000
 
START:
    
    MOVLW   0x0F	    ; Load W with 0x0F
    MOVWF   ADCON1	    ; Make ADCON be all digital
    
    MOVLW   0xFF	    ; Load W with 0xFF
    MOVWF   TRISA	    ; Set PORTA as input
    
    MOVLW   0x00	    ; Load W with 0x00
    MOVWF   TRISB	    ; Set PORTB as output
    
    MOVLW   0xFF	    ; Load W with 0x00
    MOVWF   TRISC	    ; Set PORTC as input
    
    MOVLW   0x07	    ; Load W with 0x07
    MOVWF   TRISD	    ; Set PORTD as output
    
    MOVLW   0x00	    ; Load W with 0xFF
    MOVWF   TRISE	    ; Set PORTE as output
    
    BCF	    PORTD, 5	    ; CLear bit 5 of PORTD. NOT USED FOR THIS PART
    BCF	    PORTD, 4	    ; CLear bit 4 of PORTD. NOT USED FOR THIS PART
    BCF	    PORTD, 3	    ; CLear bit 3 of PORTD. NOT USED FOR THIS PART
    CLRF    PORTB	    ; Clear PORTB
    CLRF    PORTE	    ; Clear PORTE
    
MAIN_LOOP:
    BTFSC   PORTD,2         ; If bit 2 is 0, skip next instruction
    GOTO    DPort_1XX_BCD   ; Bit 2 is 1, go to DPort_1XX
    GOTO    DPort_0XX	    ; Bit 2 is 0, go to DPort_0XX
    
DPort_0XX:
    BTFSC   PORTD,1	    ; If bit 1 is 0, skip next instruction
    GOTO    DPort_01X	    ; BIt 1 is 1, go to DPort_01X
    GOTO    DPort_00X	    ; Bit 1 is 0, go to DPort_00X
 
    
DPort_00X:
    BTFSC   PORTD,0         ; If bit 0 is 0, skip next instruction
    GOTO    DPort_001_ADD   ; Bit 0 is 1, go to DPort_001
    GOTO    DPort_000_COM   ; Bit 0 is 0, go to DPort_000
    
DPort_01X:
    BTFSC   PORTD,0         ; If bit 0 is 0, skip the next instruction
    GOTO    DPort_011_XOR   ; Bit 0 is 1, go to DPort_011
    GOTO    DPort_010_AND   ; Bit 0 is 0, go to DPort_010
 
DPort_000_COM:		    ; 000 Subroutine is 1's COM
    BCF	    PORTD,5	    ; Clear Bit 5 of PORTD
    BCF	    PORTD,4	    ; Clear Bit 4 of PORTD
    BCF	    PORTD,3	    ; Clear Bit 3 of PORTD
    CALL    SUBROUTINE_COM  ; Calling Subroutine for COM
    GOTO    MAIN_LOOP	    ; Go to MAIN_LOOP
    
DPort_001_ADD:		    ; 001 Subroutine is ADD
    BCF	    PORTD,5	    ; Clear Bit 5 of PORTD
    BCF	    PORTD,4	    ; Clear Bit 4 of PORTD
    BSF	    PORTD,3	    ; Set Bit 3 of PORTD to 1
    CALL    SUBROUTINE_ADD  ; Calling Subroutine for ADD
    GOTO    MAIN_LOOP	    ; Go to MAIN_LOOP
    
DPort_010_AND:		    ; 010 Subroutine ir AND
    BCF	    PORTD,5	    ; Clear Bit 5 of PORTD
    BSF	    PORTD,4	    ; Set Bit 4 of PORTD to 1
    BCF	    PORTD,3	    ; Clear Bit 3 of PORTD
    CALL    SUBROUTINE_AND  ; Calling Subroutine for AND
    GOTO    MAIN_LOOP       ; Go to MAIN_LOOP
    
DPort_011_XOR:		    ; 011 Subroutine is XOR
    BCF	    PORTD,5	    ; Clear Bit 5 of PORTD
    BSF	    PORTD,4	    ; Set Bit 4 of PORTD to 1
    BSF	    PORTD,3	    ; Set Bit 3 of PORTD to 1
    CALL    SUBROUTINE_XOR  ; Calling Subroutine for XOR
    GOTO    MAIN_LOOP	    ; Go to MAIN_LOOP
    
DPort_1XX_BCD:		    ; 1XX Subroutine is BCD
    BSF	    PORTD,5	    ; Set Bit 5 of PORTD to 1
    BCF     PORTD,4	    ; Clear Bit 4 of PORTD
    BCF     PORTD,3	    ; Clear Bit 3 of PORTD
    CALL    SUBROUTINE_BCD  ; Calling Subroutine for BCD
    GOTO    MAIN_LOOP	    ; Go to MAIN_LOOP
    
    
SUBROUTINE_COM:		    ; Where the subroutine for COM Starts
    MOVF    PORTA, W        ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    COMF    Input_A,W	    ; Complement Input_A and store into W
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Result	    ; Store Complement into Result
    MOVFF   Result, PORTB   ; Move Result to PORTB
    BCF	    PORTE,0	    ; Clear bit 0 of PORTE
    BNZ	    COM_RETURN	    ; Return if not 
    BSF	    PORTE,0	    ; Set Bit 0 to 1 of PORTE
COM_RETURN:  
    RETURN
    
SUBROUTINE_ADD:		    ; Where the subroutine for ADD Starts
    MOVF    PORTA, W        ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVF    PORTC, W	    ; Read from PORTC and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_C	    ; Storing in variable Input_C
    
    ADDWF   Input_A,W       ; W loaded with Input_C. Add W to Input_A, store in W
    MOVWF   Result	    ; Store Input_A + Input_C in Result
    MOVFF   Result, PORTB   ; Move Result to PORTB
    BCF	    PORTE,0	    ; Clear bit 0 of PORTE
    BNZ	    ADD_RETURN	    ; Return if not 
    BSF	    PORTE,0	    ; Set Bit 0 to 1 of PORTE
ADD_RETURN:
    RETURN
    
SUBROUTINE_AND:		    ; Where the subroutine for AND Starts
    MOVF    PORTA, W	    ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVF    PORTC, W	    ; Read from PORTC and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_C	    ; Storing in variable Input_C
    
    ANDWF   Input_A,W       ; W loaded with Input_C. AND W to Input_A, store in W
    MOVWF   Result	    ; Store Input_A + Input_C in Result
    MOVFF   Result, PORTB   ; Move Result to PORTB
    BCF	    PORTE,0	    ; Clear bit 0 of PORTE
    BNZ	    AND_RETURN      ; Return if not 0
    BSF	    PORTE,0	    ; Set Bit 0 to 1 of PORTE
AND_RETURN:
    RETURN
    
SUBROUTINE_XOR:		    ; Where the subroutine for XOR Starts
    MOVF    PORTA, W	    ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVF    PORTC, W	    ; Read from PORTC and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_C	    ; Storing in variable Input_C
    
    XORWF   Input_A,W	    ; W loaded with Input_C. XOR W to Input_A, store in W
    MOVWF   Result	    ; Store Input_A + Input_C in Result
    MOVFF   Result, PORTB   ; Move Result to PORTB
    BCF	    PORTE,0	    ; Clear bit 0 of PORTE
    BNZ	    XOR_RETURN	    ; Return if not 
    BSF	    PORTE,0	    ; Set Bit 0 to 1 of PORTE
XOR_RETURN:
    RETURN
    
SUBROUTINE_BCD:		    ; Where the subroutine for BCD Starts
    MOVF    PORTA, W	    ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVLW   0x0A	    ; Load W with 0x09
    CPFSLT  Input_A,1	    ; Skip Next Instruction if Input_A < 0x09
    GOTO    BCD_GREATERTHAN ; Go to GREATERTHAN if Input_A < 0x09
    GOTO    BCD_LESSEQUAL   ; Keep value and GOTO CheckZFlag
    
BCD_GREATERTHAN:	    ; Still Part of Subroutine for BCD 
    MOVLW   0x06	    ; Load Input_A into W
    ADDWF   Input_A, 0	    ; Add 0x06 to Input_A, Store in W
    MOVWF   Result	    ; Store 0x06 + Input_A into Result
    GOTO    BCD_CheckZFlag  ; Go to CheckZFlag
    
BCD_LESSEQUAL:
    MOVFF   Input_A,Result  ; Move Input_A to Result
    GOTO    BCD_CheckZFlag  ; Go to zflag
    
BCD_CheckZFlag:		    ; All Subroutines come here to check Z flags
    MOVFF   Result, PORTB   ; Move Result to PORTB
    BCF	    PORTE,0	    ; Clear bit 0 of PORTE
    MOVF    Result,W	    ; Load Input_A into 
    BNZ	    BCD_RETURN	    ; Return if not 
    BSF	    PORTE,0	    ; Set Bit 0 to 1 of PORTE
BCD_RETURN:
    RETURN		    ; Go Back to Main
    
    END



