#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>
#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF
#pragma config CCP2MX = PORTBE

#define NS_Red      PORTAbits.RA5                   //Definitions for the Lights
#define NS_Green    PORTBbits.RB0
#define NSLT_Red    PORTBbits.RB1
#define NSLT_Green  PORTBbits.RB2
#define EW_Red      PORTBbits.RB4
#define EW_Green    PORTBbits.RB5
#define EWLT_Red    PORTEbits.RE0
#define EWLT_Green  PORTEbits.RE1
#define SEC_LED     PORTEbits.RE2
#define OFF         0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define Seg_OFF     0xFF
#define NSLT_SW     PORTAbits.RA1
#define NSPED_SW    PORTAbits.RA2
#define EWLT_SW     PORTAbits.RA3
#define EWPED_SW    PORTAbits.RA4

void Set_Up_Chip();                                 //Prototypes of routines
void Init_ADC(void);                                
unsigned int get_full_ADC(void);                    
void Wait_Half_Second();
void Wait_One_Second();
void Wait_N_Seconds (char seconds);
void Wait_One_Second_With_Beep();
void Set_NS(char color);
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);
void PED_Control( char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Activate_Buzzer();
void Deactivate_Buzzer();

int Seg[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

void main()
{
int NStep;
float Light_mV, Light_V; 
    Set_Up_Chip();                                          //Set-up Ports for the chip
    while(1)
    {
        NStep = get_full_ADC();                             //Set NStep
        Light_mV = NStep * 5.0;                             //Convert to mV
        Light_V = Light_mV/1024;                            //Convert mV to V
        if(Light_V<= 2.5)
            Day_Mode();
        else 
            Night_Mode();
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
char i;                                             //char for the "for" loop
    for (i = 0; i < seconds; i++)                   //Loop of "Wait_One_Second"
    {
        Wait_One_Second();
    }
}

void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;                                    // First, turn on the SEC LED 
    Activate_Buzzer();                              // Activate the buzzer
    Wait_Half_Second();                             // Wait for half second (or 500 msec) 
    SEC_LED = 0;                                    // then turn off the SEC LED 
    Deactivate_Buzzer();                            // Deactivate the buzzer 
    Wait_Half_Second();                             // Wait for half second (or 500 msec) 
}

void Set_NS(char color)                             //Set-up NS Lights
{
    switch(color)
    {
        case OFF: NS_Red = 0; NS_Green = 0; break;      //Turn OFF Lights for NS
        case RED: NS_Red = 1; NS_Green = 0; break;      //Make lights red for NS
        case GREEN: NS_Red = 0; NS_Green = 1; break;    //Make lights green for NS
        case YELLOW: NS_Red = 1; NS_Green = 1; break;   //Make lights yellow for NS
    }
}
void Set_NSLT(char color)                           //Set-up NSLT Lights
{
    switch(color)
    {
        case OFF: NSLT_Red = 0; NSLT_Green = 0; break;      //Turn OFF lights for NSLT
        case RED: NSLT_Red = 1; NSLT_Green = 0; break;      //Make lights red for NSLT
        case GREEN: NSLT_Red = 0; NSLT_Green = 1; break;    //Make lights green for NSLT
        case YELLOW: NSLT_Red = 1; NSLT_Green = 1; break;   //Make lights yellow for NSLT
    }
}

void Set_EW(char color)                             //Set-up EW Lights
{
    switch(color)
    {
        case OFF: EW_Red = 0; EW_Green = 0; break;      //Turn OFF Lights for EW
        case RED: EW_Red = 1; EW_Green = 0; break;      //Make lights red for EW
        case GREEN: EW_Red = 0; EW_Green = 1; break;    //Make lights green for EW
        case YELLOW: EW_Red = 1; EW_Green = 1; break;   //Make lights yellow for EW
    }
}

void Set_EWLT(char color)                           //Set-up EWLT Lights
{
    switch(color)
    {
        case OFF: EWLT_Red = 0; EWLT_Green = 0; break;      //Turn OFF Lights for EWLT
        case RED: EWLT_Red = 1; EWLT_Green = 0; break;      //Make lights red for EWLT
        case GREEN: EWLT_Red = 0; EWLT_Green = 1; break;    //Make lights green for EWLT
        case YELLOW: EWLT_Red = 1; EWLT_Green = 1; break;   //Make lights yellow for EWLT
    }
}

void PED_Control( char Direction, char Num_Sec)
{
int Upper, Lower, Count;
    for (Count = Num_Sec-1; Count > 0; Count--)     //For loop for the count down
    {
        Upper = Seg[Count];                         //Initialize Both Displays with the Count Number
        Lower = Seg[Count];
        if (Direction == 0)                         //If Direction is NS (0), Turn off Lower
            Lower = 0x7F;
        else if (Direction == 1)                    //If Direction is EW (1), Turn off Upper
            Upper = 0x7F;
        PORTC = Upper;                              //Display Upper
        PORTD = Lower;                              //Display Lower and turn off Dp
        Wait_One_Second_With_Beep();                //Wait with the beep
    }
    PORTC = 0x7F;                                   //Turn off Upper (for when the count is 0)
    PORTD = 0x7F;                                   //Turn off the Lower (when the count is 0)
    Wait_One_Second_With_Beep();                    //Wait with the beep
}

void Day_Mode()
{
    PORTC = 0x7F;                                   //Turn off Segments except dp
    PORTD = 0x7F;                        
    Set_NS(GREEN);                                  //Step 1: NS to Green, others to Red
    Set_NSLT(RED);
    Set_EW(RED);
    Set_EWLT(RED);
    if (NSPED_SW==1)                                //Step 1a: Pedestrian present
        PED_Control(0,9);                           //PED_Control for NS with 9 seconds
    Wait_N_Seconds(8);                              //Step 2: NS still Green and wait 3 seconds
    Set_NS(YELLOW);                                 //Step 3: NS to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_NS(RED);                                    //Step 4: NS to Red
    
    if(EWLT_SW==1)                                  //Step 5: Check EWLT
    {
        Set_EWLT(GREEN);                            //Step 6: EWLT to Green and wait 7 seconds
        Wait_N_Seconds(7);
        Set_EWLT(YELLOW);                           //Step 7: EWLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_EWLT(RED);                              //Step 8: EWLT to Red
    }
    
    Set_EW(GREEN);                                  //Step 9: Turn EW to Green
    if(EWPED_SW == 1)                               //Step 9a: Pedestrian present
        PED_Control(1,8);                           //PED_Control for EW with 8 seconds
    Wait_N_Seconds(7);                              //Step 10: EW stays Green, wait 7 seconds
    Set_EW(YELLOW);                                 //Step 11: EW to Yellow, wait 3 seconds
    Wait_N_Seconds(3);                              
    Set_EW(RED);                                    //Step 12: EW to Red
    
    if (NSLT_SW == 1)                               //Step 13: Check NSLT switch
    {
        Set_NSLT(GREEN);                            //Step 14: NSLT to Green and wait 8 seconds
        Wait_N_Seconds(8);
        Set_NSLT(YELLOW);                           //Step 15: NSLT to Yellow and wait 3
        Wait_N_Seconds(3);
        Set_NSLT(RED);
    }
}

void Night_Mode()
{
    PORTC = Seg_OFF;                                //Turn of Seg if on
    PORTD = Seg_OFF;                                
    Set_NS(GREEN);                                  //Step 1: Turn NS Green, others to Red
    Set_NSLT(RED);
    Set_EW(RED);
    Set_EWLT(RED);
    Wait_N_Seconds(7);                              //Step 2: Wait 7 Seconds
    Set_NS(YELLOW);                                 //Step 3: NS to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_NS(RED);                                    //Step 4: NS to Red
    
    if(EWLT_SW==1)                                  //Step 5: Check EWLT
    {
        Set_EWLT(GREEN);                            //Step 6: EWLT to Green and wait 7 seconds
        Wait_N_Seconds(7);                           
        Set_EWLT(YELLOW);                           //Step 7: EWLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_EWLT(RED);                              //Step 8: EWLT to Red
    }
    
    Set_EW(GREEN);                                  //Step 9: EW to Green and wait 6 seconds
    Wait_N_Seconds(6);
    Set_EW(YELLOW);                                 //Step 10: EW to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_EW(RED);                                    //Step 11: EW to Red
    
    if(NSLT_SW==1)                                  //Step 12: Check NSLT
    {
        Set_NSLT(GREEN);                            //Step 13: NSLT to Green and wait 8 seconds
        Wait_N_Seconds(8);           
        Set_NSLT(YELLOW);                           //Step 14: NSLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_NSLT(RED);                              //Step 15: NSLT to Red
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


