#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>
#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF
#pragma config CCP2MX = PORTBE

void putch(char c)
{
    while(!TRMT);
    TXREG = c;
}


void init_UART()                                        //Code to Initalize UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
        USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
        USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}


void Init_ADC(void);                                    //Prototype for Init_ADC
void Get_Seg_Digits(float n, int Values[]);
unsigned int get_full_ADC(void);                        //Prototype for ADC
void D1_None(void);                                     //Prototypes for D1 Output Signals
void D1_Red(void);
void D1_Green(void);
void D1_Yellow(void);
void D1_Blue(void);
void D1_Purple(void);
void D1_Cyan(void);
void D1_White(void);
void Activate_Buzzer();
void Deactivate_Buzzer();
void (*D1Select[])() = {D1_None, D1_Red, D1_Green, 
    D1_Yellow, D1_Blue, D1_Purple, D1_Cyan, D1_White};   


#define B0 PORTBbits.RB0                                 //Definitions to simplify coding
#define B1 PORTBbits.RB1
#define B2 PORTBbits.RB2
#define B3 PORTBbits.RB3
#define B5 PORTBbits.RB5


void main()
{
    
int Seg[11] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF};
float Rref[3] = {0.98,10.02,99.7};
int SegDigits[3];
int ChNum,NStep,RNum,EnDp,D1Range;
float Volt_mV, Volt_V, VL, IR, RL; 
    
    init_UART();
    Init_ADC();
    TRISA = 0xFF;                                       //Make PORTA Inputs
    TRISB = 0x00;                                       //Make PORTB all outputs
    TRISC = 0x00;                                       //Make PORTC all outputs
    TRISD = 0x00;                                       //Make PORTD all outputs
    TRISE = 0x00;                                       //Make PORTE all outputs
    ChNum = 1;                                          //Select Channel Number
    RNum = 1;
    
    while(1)
    {
        ADCON0 = ((ChNum*4)+1);                         //Initialize ADCON1
        NStep = get_full_ADC();                         //Initialize Step
        Volt_mV = NStep * 4.0;                          //Calculate Voltage in mV
        Volt_V = ((Volt_mV)/(1000.0));                  //Calculate Voltage in V
        VL = ((4.096) - (Volt_V));                      //Calculate Voltage in Rref
        IR = ((VL)/(Rref[RNum-1]));                     //Calculate Current
        RL = ((Volt_V)/(IR));                           //Calculate Resistance in RL
        
        Get_Seg_Digits(RL, SegDigits);                  //Get Segement Digits
        
        if (SegDigits[1]>9)                             //If RL > 100k, turn off
        {
            PORTC = Seg[10];
            PORTD = Seg[10];
        }
        else                                            //Display Resistance Value
        {
            EnDp = SegDigits[0];                        //Enable or Disable Dp
            PORTC = Seg[(SegDigits[1])];                //Display Upper Digit
            PORTD = (Seg[(SegDigits[2])])|(EnDp);       //Display Lower Digit
        }
        
        if (EnDp == 0)                                  //If RL < 10k Range for D1
            D1Range = 0;
        else 
        {
            if (SegDigits[1]>7)                         //Range > 70k for D1
                D1Range = 7;
            else 
                D1Range = SegDigits[1];                 //Intermediate Ranges for D1
        }
        (*D1Select[D1Range])();                         //Select color based on range
        
        if(RL < 0.020)                                  //Resistance < 20
        {
            B5 = 1;                                     //Turn LED on
            Activate_Buzzer();                          //Activate Buzzer
        }
        else 
        {
            B5 = 0;                                     //Turn LED off
            Deactivate_Buzzer();                        //Buzzer Off
        }
        
        
        printf("The resistance: %f\r\n", RL);
        printf("The resistance (upper): %d\r\n", SegDigits[1]);
        printf("The resistance (lower): %d\r\n\n", SegDigits[2]);
    }
}


void Init_ADC(void)
{
    ADCON1 = 0x1B;                                          //Select pins AN0 through AN3 as analog signal, set up Vref+/-
    ADCON2 = 0xA9;                                          //Right justify the result. Set the bit conversion
                                                            //TAD and acquisition time
}


unsigned int get_full_ADC(void)
{
int result;                                                 //Where the result is stored
    ADCON0bits.GO = 1;                                      //Start Conversion
    while(ADCON0bits.DONE==1);                              //Wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                     //Combine result and upper byte and lower byte
                                                            //Into the result
    return result;                                          //Return the result
}


void Get_Seg_Digits(float n, int Values[])                  //Pass by Ref for Dp and Digits via "Values"
{
    int Upper, Lower;                                       
    if (n < 10)                                             //Get Digits if R < 10k
    {
        Values[0] = 0x00;                                   //Will enable Dp in main function
        Upper = (int)n;                                     //Upper Digit of Display
        Lower = (((int)(n*10))%10);                         //Lower Digit of Display
    }
    else                                                    //Get Digits if R > 10k
    {
        Values[0] = 0x80;                                   //Disable Dp in main function
        Upper = ((int)(n/10));                              //Upper Digit of Display
        Lower = ((int)n%10);                                //Lower Digit of Display
    }
    Values[1] = Upper;                                      //Store Upper in Values[1]
    Values[2]= Lower;                                       //Store Lower in Values[2]
}


void D1_None(void)                                          //Setting output for RGB LED D1
{
    B2 = 0;
    B1 = 0;
    B0 = 0;
}
void D1_Red(void)
{
    B2 = 0;
    B1 = 0;
    B0 = 1;
}
void D1_Green(void)
{
    B2 = 0;
    B1 = 1;
    B0 = 0;
}
void D1_Yellow(void)
{
    B2 = 0;
    B1 = 1;
    B0 = 1;
}
void D1_Blue(void)
{
    B2 = 1;
    B1 = 0;
    B0 = 0;
}
void D1_Purple(void)
{
    B2 = 1;
    B1 = 0;
    B0 = 1;
}
void D1_Cyan(void)
{
    B2 = 1;
    B1 = 1;
    B0 = 0;
}
void D1_White(void)
{
    B2 = 1;
    B1 = 1;
    B0 = 1;
}


void Activate_Buzzer()                                      //Activate Buzzer
{
    PR2 = 0b11111001 ; 
    T2CON = 0b00000101 ; 
    CCPR2L = 0b01001010 ; 
    CCP2CON = 0b00111100 ; 
}


void Deactivate_Buzzer()                                    //Deactivate Buzzer
{
    CCP2CON = 0x0; 
    PORTBbits.RB3 = 0;
}

