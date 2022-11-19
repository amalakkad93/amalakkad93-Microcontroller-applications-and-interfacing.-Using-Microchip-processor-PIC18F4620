; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF   
; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM  
; TO THE PINS 0 THROUGH 3 OF PORT B 
     
#include <P18F4620.inc> 
 
    config	OSC = INTIO67  
    config	WDT = OFF	   
    config	LVP = OFF  
    config	BOREN = OFF 
 
Color_PORTC	equ 0x07	    ; create white light at port C 
Color_PORTD     equ 0x01	    ; create red light at port D 
Color_Off       equ 0x00	    ; creates a no light
 
OUTER_VALUE	equ 0xF0	    ;<- value of outer loop 
 
Saved_D1_loc	equ 0x22	    ; memory address for saved Color for D1 
Saved_D2_loc	equ 0x23	    ; memory address for saved Color for D2 
Saved_OV_loc	equ 0x24	    ; memory address for saved Outer Value 
 
    ORG		0x0000 
 
; CODE STARTS FROM THE NEXT LINE 
 
START: 
 
    MOVLW	0x00		    ; Load W with 0x00     
    MOVWF	TRISC		    ; Make PORT C bits 0-7 as outputs     
    MOVWF	TRISD		    ; Make PORT D bits 0-7 as outputs 
 
MAIN_LOOP: 
 
    MOVLW	Color_PORTC	    ; Load W with the WHITE color for D1 at PORTC     
    MOVWF	Saved_D1_loc	    ; save desired color into memory location Saved_D1_loc 
 
START_TEST: 
    
    MOVLW	Color_PORTD	    ; Load W with the desired RED color for D2 at PORTD     
    MOVWF	Saved_D2_loc	    ; save desired color into memory location Saved_D2_loc     
    MOVLW	OUTER_VALUE	    ; Load OUTER_VALUE into W      
    MOVWF	Saved_OV_loc	    ; save it to the memory location Saved_OV_loc 
 
COLOR_LOOP: 
    MOVFF	Saved_D1_loc,PORTC  ; Get saved color of PORTC and output to that Port     
    MOVFF	Saved_D2_loc ,PORTD ; Get saved color of PORTD and output to that Port    
    MOVFF	Saved_OV_loc,0x21   ; Copy saved outer loop value  to 0x21  
 
; NESTED DELAY LOOP TO HAVE THE FIRST HALF OF WAVEFORM 
 
LOOP_OUTER_1:     
    NOP				    ; Do nothing 
    MOVLW	0x80     
    MOVWF	0x20		    ; Load saved inner loop value to 0x20 
 
 
LOOP_INNER_1:      
    NOP				    ; Do nothing     
    DECF	0x20,F		    ; Decrement memory location 0x20     
    BNZ		LOOP_INNER_1	    ; If value not zero, go back to LOOP_INNER_1 
 
    DECF	0x21,F		    ; Decrement memory location 0x21      
    BNZ		LOOP_OUTER_1	    ; If value not zero, go back to LOOP_OUTER_1 
 
    MOVLW	Color_Off	    ; Load W with the second desired color      
    MOVWF	PORTC		    ; Output to PORT C to turn off the RGB LED D1     
    MOVWF	PORTD		    ; Output to PORT D to turn off the RGB LED D2     
    MOVFF	Saved_OV_loc,0x21   ; Copy saved outer loop value  to 0x21  
 
; NESTED DELAY LOOP TO HAVE THE FIRST HALF OF WAVEFORM BEING LOW 
 
LOOP_OUTER_2:     
    NOP				    ; Do nothing     
    MOVLW	0x80     
    MOVWF	0x20		    ; Load saved inner loop value to 0x20 
 
LOOP_INNER_2:      
    NOP				    ; Do nothing     
    DECF	0x20,F		    ; Decrement memory location 0x20     
    BNZ		LOOP_INNER_2	    ; If value not zero, go back to LOOP_INNER_2 
 
    DECF	0x21,F		    ; Decrement memory location 0x21      
    BNZ		LOOP_OUTER_2	    ; If value not zero, go back to LOOP_OUTER_2 
 
 
; START ALL OVER AGAIN 
 
   GOTO		MAIN_LOOP	    ; Go back to main loop   
   END 
 
 



