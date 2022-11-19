#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 

#pragma config OSC  =   INTIO67   
#pragma config WDT  =   OFF 
#pragma config LVP  =   OFF 
#pragma config BOREN  =   OFF 

void main(void)
{
    char input;
    TRISA = 0xff;                       //Initialize Port A as an input
    TRISB = 0x00;                       //Initialize Port B as an output
    TRISC = 0x00;                       //Initialize Port C as an output
    ADCON1 = 0x0f;                      //Converter
    
    while(1)                            //Infinite while loop
    {
        input = PORTA;                  //Storing port values into "input"
        input = input & 0x07;           //Only takes in three bits
        PORTC = input;                  //Setting PortB to be the same as Port A
    }
}


