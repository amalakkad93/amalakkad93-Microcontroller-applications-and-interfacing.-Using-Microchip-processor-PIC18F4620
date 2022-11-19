#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>
#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF
#pragma config CCP2MX = PORTBE

#define NS_Red      PORTAbits.RA5                   //Definitions for lights
#define NS_Green    PORTBbits.RB0
#define NSLT_Red    PORTBbits.RB1
#define NSLT_Green  PORTBbits.RB2
#define EW_Red      PORTBbits.RB4
#define EW_Green    PORTBbits.RB5
#define EWLT_Red    PORTEbits.RE0
#define EWLT_Green  PORTEbits.RE1
#define SEC_LED     PORTEbits.RE2
#define Seg_OFF     0xFF

void Set_Up_Chip();                                 //Prototypes
void Init_ADC(void);                               
unsigned int get_full_ADC(void);                    
void Wait_Half_Second();
void Wait_One_Second();
void Wait_N_Seconds (char seconds);
void Activate_Buzzer();
void Deactivate_Buzzer();

int Seg[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

void main()
{
char Count;                                         //Char to count seconds
    Set_Up_Chip();                                  //Set-up the ports of the chip
    Count = 3;                                      //Change value of count
    while(1)
    {
        Wait_N_Seconds(Count);                      //Wait N Seconds equal to Count
    }
    
}

void Set_Up_Chip()
{
    OSCCON = 0x60;
    Init_ADC();                                     //Initialize ADC Values
    TRISA = 0xDF;                                   //Make all PORTA input except bit 5                                       
    TRISB = 0x00;                                   //Make PORTB outputs
    TRISC = 0x00;                                   //Make PORTC outputs
    TRISD = 0x00;                                   //Make PORTD outputs
    TRISE = 0x00;                                   //Make PORTE outputs
    NS_Red = 0;                                     //Clear the NS_Lights
    PORTB = 0x00;                                   //Clear PORTB
    PORTC = Seg_OFF;                                //Make sure segments are off
    PORTD = Seg_OFF;                                //Make sure segments are off
    PORTE = 0x00;                                   //Clear PORTE
    Deactivate_Buzzer();                            //Deactivate Buzzer
}

void Init_ADC(void)
{
    ADCON0 = 0x01;                                  //Setting up ADCON0
    ADCON1 = 0x0E;                                  //Setting up ADCON1
    ADCON2 = 0xA9;                                  //Right justify the result. Set the bit conversion
                                                    //TAD and acquisition time
}

unsigned int get_full_ADC(void)
{
int result;                                         //Where the result is stored
    ADCON0bits.GO = 1;                              //Start Conversion
    while(ADCON0bits.DONE==1);                      //Wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;             //Combine result and upper byte and lower byte
                                                    //Into the result
    return result;                                  //Return the result
}

void Wait_Half_Second()
{
    T0CON = 0x02;                                   // Timer 0, 16-bit mode, prescaler 1:8 
    TMR0L = 0xDB;                                   // set the lower byte of TMR 
    TMR0H = 0x0B;                                   // set the upper byte of TMR 
    INTCONbits.TMR0IF = 0;                          // clear the Timer 0 flag 
    T0CONbits.TMR0ON = 1;                           // Turn on the Timer 0 
    while (INTCONbits.TMR0IF == 0);                 // wait for the Timer Flag to be 1 for done 
    T0CONbits.TMR0ON = 0;                           // turn off the Timer 0 
}

void Wait_One_Second()
{
    SEC_LED = 0;                                    // First, turn off the SEC LED 
    Wait_Half_Second();                             // Wait for half second (or 500 msec) 
    SEC_LED = 1;                                    // then turn on the SEC LED 
    Wait_Half_Second();                             // Wait for half second (or 500 msec) 
}

void Wait_N_Seconds (char seconds)
{
char i;                                             //Char for the "for" loop
    for (i = 0; i < seconds; i++)                   //Loop of Wait_One_Second
    {
        Wait_One_Second();
    }
}

void Activate_Buzzer()                              //Activate Buzzer
{
    PR2 = 0b11111001 ; 
    T2CON = 0b00000101 ; 
    CCPR2L = 0b01001010 ; 
    CCP2CON = 0b00111100 ; 
}


void Deactivate_Buzzer()                            //Deactivate Buzzer
{
    CCP2CON = 0x0; 
    PORTBbits.RB3 = 0;
}

