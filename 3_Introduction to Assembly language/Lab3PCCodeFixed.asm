; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF   
; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM  
; TO THE PINS 0 THROUGH 3 OF PORT B 
 
#include <P18F4620.inc> 
 
    config  OSC = INTIO67  
    config  WDT = OFF	   
    config  LVP = OFF  
    config  BOREN = OFF 
    ORG	    0x0000 

			; CODE STARTS FROM THE NEXT LINE 
 
START:  
    
    MOVLW   0xFF	; Load W with 0xFF  
    MOVWF   TRISA	; Set PORT A as all inputs 
 
    MOVLW   0x00	; Load W with 0x00  
    MOVWF   TRISB	; Make PORTB bits 0-7 as outputs 
 
    MOVLW   0x0F	; Load W with 0x0F  
    MOVWF   ADCON1	; Set ADCON1 
 
MAIN_LOOP: 
    BTFSC   PORTA, 0	; If Bit 0 of PORTA = 0 skip the next instruction       
    GOTO    CASE_A0EQ1  ; else go to CASEA0EQ1 (PORTA Bit 0 = 1) 
 
CASE_A0EQ0:      
    BCF	    PORTB, 0	; case PORTB bit 0 = 0, clear bit 0 of PORTB 
    GOTO    MAIN_LOOP	; go back to Main Loop 
 
CASE_A0EQ1:  
    BSF	    PORTB, 0	; case PORTB bit 0 = 1, set bit 0 of PORTB      
    GOTO    MAIN_LOOP	; go back to Loop 

    END



