#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 

#pragma config OSC  =   INTIO67   
#pragma config WDT  =   OFF 
#pragma config LVP  =   OFF 
#pragma config BOREN  =   OFF 

void Delay_One_Sec();

void main(void)
{
    TRISA = 0xff;                       //Initialize Port A as an input
    TRISB = 0x00;                       //Initialize Port B as an output
    TRISC = 0x00;                       //Initialize Port C as an output
    ADCON1 = 0x0f;                      //Converter
    while(1)                            //Endless while loop for different colors
    {
        for(char i = 0; i < 8; i++)     //For loop to change values
        {
            PORTC = i;                  //Set value of output port to the cycling number
            Delay_One_Sec();            //Creates the Delay
        }
    }
}

void Delay_One_Sec()
{
    for(int I=0; I<17000;I++);
}