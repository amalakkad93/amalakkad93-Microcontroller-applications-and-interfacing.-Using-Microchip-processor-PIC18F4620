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
    TRISD = 0x00;                       //Initialize Port D as an output
    ADCON1 = 0x0f;                      //Converter
    
                                        //Create the order for the lights in array
    char RandomOrder[8] = {4,7,5,0,1,6,2,3};
    
    while(1)
    {
        for(char i = 0; i < 8; i++)
        {
            PORTD = RandomOrder[i];     //Sets the value of Port according to RandomOrder
            PORTC = i;                  //Set the value to go through the original cycle
            Delay_One_Sec();            //Delay
        }
    }
    
}

void Delay_One_Sec()
{
    for(int I=0; I<17000;I++);
}