#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>
#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF

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
unsigned int get_full_ADC(void);                        //Prototype for ADC

void main()
{
    
int Seg[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
int ChNum,NStep, Vint, Vdec;
float Volt_mV, Volt_V;
    
    init_UART();
    Init_ADC();
    TRISA = 0xFF;                                       //Make PORTA Inputs
    TRISB = 0x00;                                       //Make PORTB all outputs
    TRISC = 0x00;                                       //Make PORTC all outputs
    TRISD = 0x00;                                       //Make PORTD all outputs
    TRISE = 0x00;                                       //Make PORTE all outputs
    
    
    while(1)
    {
        ChNum = 0;                                      //Select Channel 0
        ADCON0 = ((ChNum*4)+1);                         //Initialize ADCON1
        NStep = get_full_ADC();                         //Initialize Step
        Volt_mV = NStep * 4.0;                          //Calculate Voltage in mV
        Volt_V = (Volt_mV/1000.0);                      //Calculate Voltage in V
        Vint = ((int)(Volt_V));                         //Calculate the upper value
        Vdec = (((int)(Volt_V*10))%10);                 //Calculate the decimal value for lower LED
        
        PORTC = Seg[Vint];                              //Seven Segment code for upper LED
        PORTD = Seg[Vdec];                              //Seven Segment code for lower LED OR with 0x80 to enable DP
        
        printf("The voltage (float): %f\r\n", Volt_V);  //Print Voltage Readings
        printf("The voltage (upper): %d\r\n", Vint);
        printf("The voltage (lower): %d\r\n\n", Vdec);
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
