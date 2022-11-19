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
    
    COMF    Input_A,W	    ; Complement Input_A and store into W
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Result	    ; Store Complement into Result
    MOVFF   Result,PORTB    ; Move Result to PORTB
    
    END
