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
    
    BCF     PORTD, 5	    ; CLear bit 5 of PORTD. NOT USED FOR THIS PART
    BCF     PORTD, 4	    ; CLear bit 4 of PORTD. NOT USED FOR THIS PART
    BCF     PORTD, 3	    ; CLear bit 3 of PORTD. NOT USED FOR THIS PART
    CLRF    PORTB	    ; Clear PORTB
    CLRF    PORTE	    ; Clear PORTE
    
MAIN:
    
    MOVF    PORTA, W	    ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVLW   0x0A	    ; Load W with 0x09
    CPFSLT  Input_A,1	    ; Skip Next Instruction if Input_A < 0x09
    GOTO    BCD_GREATERTHAN ; Go to GREATERTHAN if Input_A < 0x09
    GOTO    BCD_LESSEQUAL   ; Keep value and GOTO CheckZFlag
    
BCD_GREATERTHAN:	    ; Still Part of Subroutine for BCD 
    MOVLW   0x06            ; Load Input_A into W
    ADDWF   Input_A, 0      ; Add 0x06 to Input_A, Store in W
    MOVWF   Result          ; Store 0x06 + Input_A into Result
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
    GOTO    MAIN	    ; Go Back to Main
    
    END
