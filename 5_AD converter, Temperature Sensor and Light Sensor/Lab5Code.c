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


void D1_None(void);                                     //Prototypes for D1,D2 Output Signals
void D1_Red(void);
void D1_Green(void);
void D1_Yellow(void);
void D1_Blue(void);
void D1_Purple(void);
void D1_Cyan(void);
void D1_White(void);
void D2_None(void);
void D2_Red(void);
void D2_Green(void);
void D2_Blue(void);
void D2_White(void);


#define B0 PORTBbits.RB0                                 //Definitions to simplify coding
#define B1 PORTBbits.RB1
#define B2 PORTBbits.RB2
#define B3 PORTBbits.RB3
#define B4 PORTBbits.RB4
#define B5 PORTBbits.RB5

void main()
{
    init_UART();
    Init_ADC();
    TRISA = 0xFF;                                       //Make PORTA Inputs
    TRISB = 0x00;                                       //Make PORTB all outputs
    TRISC = 0x00;                                       //Make PORTC all outputs
    TRISD = 0x00;                                       //Make PORTD all outputs
    TRISE = 0x00;                                       //Make PORTE all outputs
    
    

int Seg[10] = {0x40, 0x79, 0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
int ChNum,NStep, Temp_F;
float Volt_mV, Volt_V, Deg_C, Deg_F, Light_mV, Light_V; 
char Upper, Lower;
    
    while(1)
    {
        ChNum = 0;                                      //Select Channel 0
        ADCON0 = (ChNum*4)+1;                           //Initialize ADCON1
        NStep = get_full_ADC();                         //Initialize Step
        Volt_mV = NStep * 4.0;                          //Calculate Voltage in mV
        Volt_V = Volt_mV/1000.0;                        //Calculate Voltage in V
        Deg_C = ((1035.0 - Volt_mV)/5.50);              //Convert V to Degree C
        Deg_F = (1.80*Deg_C)+ 32.0;                     //Convert Degree C to Degree F
        Temp_F = (int) Deg_F;                           //Convert F from float to integer
        Upper = Temp_F/10;                              //Get upper byte of F
        Lower = Temp_F%10;                              //Get lower byte of F
        PORTC = Seg[Upper];                             //Display MSB of F on 7Seg
        PORTD = Seg[Lower];                             //Display LSB of F on 7Seg


        if (Temp_F < 10)                                //Cases for D1 RGB LED
            D1_None();
        else if (Temp_F < 20)
            D1_Red();
        else if (Temp_F < 30)
            D1_Green();
        else if (Temp_F < 40)
            D1_Yellow();
        else if (Temp_F < 50)
            D1_Blue();
        else if (Temp_F < 60)
            D1_Purple();
        else if (Temp_F < 70)
            D1_Cyan();
        else
            D1_White();

        
        if (Temp_F < 45)                                    //Cases for D2 RGB LED
            D2_None();
        else if (Temp_F <= 55)
            D2_Red();
        else if (Temp_F <= 65)
            D2_Green();
        else if (Temp_F <= 75)
            D2_Blue();
        else 
            D2_White();
            

        ChNum = 1;                                          //Select Channel for Light Sensor
        ADCON0 = ChNum*4+1;                                 //Set ADCON1
        NStep = get_full_ADC();                             //Set NStep
        Light_mV = NStep * 4.0;                             //Convert to mV
        Light_V = Light_mV/1000;                            //Convert mV to V

        if(Light_V < 2.5)                                   //If statements for the D# RGB LED
            PORTE = 0x01;
        else if(Light_V < 3.5)
            PORTE = 0x02;
        else
            PORTE = 0x03;

        printf("Steps = %d steps \r\n", NStep);             //Print out Step#
        printf("Voltage for temp = %f V \r\n", Volt_V);     //Print voltage for temp
        printf("Temperature = %f F \r\n", Deg_F);           //Print out Temp in F
        printf("Voltage for Light = %f V \r\n\n", Light_V); //Print out Voltage in V
    }
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


void D2_None(void)                                          //Setting output for RGB LED D2
{
    B5 = 0;
    B4 = 0;
    B3 = 0;
}
void D2_Red(void)
{
    B5 = 0;
    B4 = 0;
    B3 = 1;
}
void D2_Green(void)
{
    B5 = 0;
    B4 = 1;
    B3 = 0;
}
void D2_Blue(void)
{
    B5 = 1;
    B4 = 0;
    B3 = 0;
}
void D2_White(void)
{
    B5 = 1;
    B4 = 1;
    B3 = 1;
}


void Init_ADC(void)
{
    ADCON1 = 0x1B;                                          //Select pins AN0 through AN3 as analog signal
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