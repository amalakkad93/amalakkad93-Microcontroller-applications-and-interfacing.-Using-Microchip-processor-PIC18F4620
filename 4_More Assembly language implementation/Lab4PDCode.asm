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
    BCF     PORTD, 3	    ; CLear bit 3 of PORTD. NOT USED FOR THIS PART
    CLRF    PORTB	    ; Clear PORTB
    CLRF    PORTE	    ; Clear PORTE
    
MAIN:
    
    MOVF    PORTA, W	    ; Read from PORTA and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_A	    ; Storing in variable Input_A
    
    MOVF    PORTC, W	    ; Read from PORTC and move into W 
    ANDLW   0x0F	    ; Mask with 0x0F
    MOVWF   Input_C	    ; Storing in variable Input_C
    
    ANDWF   Input_A,W	    ; W loaded with Input_C. AND W to Input_A, store in W
    MOVWF   Result	    ; Store Input_A AND Input_C in Result
    MOVFF   Result, PORTB   ; Move Result to PORTB
    
    BZ	    ZFlag_1	    ; If Z = 1, go to ZFlag_1
    BCF	    PORTE,0	    ; Clear Bit0 of PORTE
    GOTO    ZFlag_0	    ; Go to ZFlag_0
    
ZFlag_1:
    BSF	    PORTE,0	    ; Set Bit0 of PORTE to 1
    
ZFlag_0:
    GOTO    MAIN	    ; GO to Main loop
    
    END
