; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF   
; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM  
; TO THE PINS 0 THROUGH 3 OF PORT B 

#include <P18F4620.inc> 
 
    config	OSC = INTIO67  
    config	WDT = OFF	   
    config	LVP = OFF  
    config	BOREN = OFF 
     
Color_Red	equ 0x01	    ; create a red light
Color_Green	equ 0x02	    ; create a green light
Color_Blue	equ 0x04	    ; create a blue light
Color_White	equ 0x07	    ; create a white light
Color_Off       equ 0x00	    ; creates a no light
 
Saved_D1_loc	equ 0x22	    ; memory address for saved Color for D1 
Saved_D2_loc	equ 0x23	    ; memory address for saved Color for D2 
Saved_OV_loc	equ 0x24	    ; memory address for saved Outer Value

OUTER_VALUE	equ 0xF0	    ; create the speed for red light
		
    ORG		0x0000 
 
; CODE STARTS FROM THE NEXT LINE 
 
START: 

    MOVLW	0x0F		    ; Load W with 0x0F0  
    MOVWF	ADCON1		    ; Make ADCON1 to be all digital
    MOVLW	0xFF		    ; Load W with 0xFF  
    MOVWF	TRISA		    ; Set PORT A as all inputs 

    MOVLW	0x00		    ; Load W with 0x00     
    MOVWF	TRISC		    ; Make PORT C bits 0-7 as outputs     
    MOVWF	TRISD		    ; Make PORT D bits 0-7 as outputs 
 
MAIN_LOOP: 
 
    MOVLW	Color_White	    ; Load W with the WHITE color for D1 at PORTC     
    MOVWF	Saved_D1_loc	    ; save desired color into memory location Saved_D1_loc 
 
START_TEST: 
    
    BTFSC	PORTA, 1	    ; If bit 1 is 0, skip the next instruction
    GOTO	CaseBit_1X	    ; If bit 1 is 1, go to case for 1

CaseBit_0X:
    BTFSC	PORTA, 0	    ; If bit 0 is 0, will go to case 00
    GOTO	CaseBit_01	    ; If bit 0 is 1, will go to case 01
    
CaseBit_00:
    MOVLW	Color_Red	    ; Load W with desired RED color for D2
    MOVWF	Saved_D2_loc	    ; Save desired memory location Saved_D2_loc
    MOVLW	OUTER_VALUE	    ; Load W with desired RED speed for D2
    MOVWF	Saved_OV_loc	    ; Save desired speed location Saved_Speed
    GOTO	COLOR_LOOP	    ; Color selected, continuing process
    
CaseBit_01:
    MOVLW	Color_Green	    ; Load W with desired GREEN color for D2
    MOVWF	Saved_D2_loc	    ; Save desired memory location Saved_D2_loc
    MOVLW	OUTER_VALUE/2	    ; Load W with desired GREEN speed for D2
    MOVWF	Saved_OV_loc	    ; Save desired speed location Saved_Speed
    GOTO	COLOR_LOOP	    ; Color selected, continuing process
    
CaseBit_1X:
    BTFSC	PORTA, 0	    ; If bit 0 is 0, will go to case 10
    GOTO	CaseBit_11	    ; If bit 0 is 1, will go to case 11
 
CaseBit_10:
    MOVLW	Color_Blue	    ; Load W with desired BLUE color for D2
    MOVWF	Saved_D2_loc	    ; Save desired memory location Saved_D2_loc
    MOVLW	OUTER_VALUE/3	    ; Load W with desired BLUE speed for D2
    MOVWF	Saved_OV_loc	    ; Save desired speed location Saved_Speed
    GOTO	COLOR_LOOP	    ; Color selected, continuing process
    
CaseBit_11: 
    MOVLW	Color_White	    ; Load W with desired WHITE color for D2
    MOVWF	Saved_D2_loc	    ; Save desired memory location Saved_D2_loc
    MOVLW	OUTER_VALUE/4	    ; Load W with desired WHITE speed for D2
    MOVWF	Saved_OV_loc	    ; Save desired speed location Saved_Speed
    GOTO	COLOR_LOOP	    ; Color selected, continuing process
 
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
 
 






