#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

#define SEC_LED         PORTEbits.RE2

#define _XTAL_FREQ      8000000

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK             1
#define NAK             0

#include "i2c.c"

char DS1621_Read_Temp();                    //Read the temperature
void DS1621_Init();                         //Initialize DS1621
void DS3231_Read_Time();                    //Read the time
void DS3231_Write_Time();
void DS3231_Setup_Time();                   //Set-up the time for DS3231
void Wait_Half_Second();                    //Wait half a second
void Wait_One_Second();                     //Wait 1 second and flash the led
void Print_Temp();                          //Print the temp reading
void Print_Time();

char Device;                                //Select the device
char Cmd;                                   //Select the command
char Address;                               //Select the address
char tempc;                                 //Stores the temperature in degree c
unsigned char second;                       //Stores the second
unsigned char minute;                       //Stores the minute
unsigned char hour;                         //Stores the hour
unsigned char dow;                          //Stores the Day of the Week
unsigned char day;                          //Stores the day
unsigned char month;                        //Stores the month
unsigned char year;                         //Stores the year
unsigned char prev_second;                  //Stores the previous second

int INT0_flag = 0;                          //Set INT0 flag to 0
float DegC;                                 //Degree in C
float DegF;                                 //Degree in F

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF 
        & USART_ASYNCH_MODE & USART_EIGHT_BIT 
        & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Do_Init()                              // Initialize the ports 
{ 
    init_UART();                            // Initialize the UART
    OSCCON=0x70;                            // Set oscillator to 8 MHz 
    ADCON1 = 0x0F;                          // Make all input as digital
    TRISB = 0x01;                           //Set PORTB as all output except bit 0
    TRISC = 0x00;                           //Set PORTC as
    TRISD = 0x00;                           //Set PORTD as
    TRISE = 0x00;                           //Set PORTE as output
    INTCONbits.INT0IF = 0;                  //Clear INT0F
    INTCON2bits.INTEDG0 = 1;                //INT0 Edge falling
    INTCONbits.INT0IE = 1;                  //Enable INT0
    INTCONbits.GIE = 1;                     //Set the Global Interrupt Enable
    I2C_Init(10000);                        //Initializes I2C Master with 100kHz clock
    DS1621_Init();                          // Set up the DS1621 Chip 
}

void interrupt high_priority chkisr()       //Set priority of the interrupts
{
    if (INTCONbits.INT0IF == 1) 
    {
        INT0_flag = 1;                      //Set INT0 flag to 1
        INTCONbits.INT0IF = 0;              //Clear INT0F
    }
}

void main() 
{ 
    Do_Init();                              // Initialization 
    while (1)
    {
        tempc = DS1621_Read_Temp();     //Read the temperature in C and store as char
        DegC = tempc;                   //Get temp in C
        DegF = (DegC*9)/5 + 32;         //Convert the temperature
        Print_Temp();                   //Print the temperature
        Wait_One_Second();
    }
}

char DS1621_Read_Temp()
{
char Data_Ret;
    Device = 0x48;                          //Address for DS1621
    Cmd = READ_TEMP;                        //Command to read temp
    I2C_Start();                            // Start I2C protocol
    I2C_Write((Device << 1) | 0);           // Device address
    I2C_Write(Cmd);                         // Send register address
    I2C_ReStart();                          // Restart I2C
    I2C_Write((Device << 1) | 1);           // Initialize data read
    Data_Ret = I2C_Read(NAK);               // Read Data
    I2C_Stop();                             // Stop
    return Data_Ret;
}

void DS1621_Init()
{
    Device = 0x48;                          //Address of DS1621 device 
                                            //Code to start the device
	I2C_Write_Cmd_Write_Data(Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);
}

void DS3231_Read_Time()
{
    Device = 0x68;                          // Address for DS3231
    Address = 0x00;                         // Register 0x00 pointing to the register 'second'
    I2C_Start();                            // Start I2C protocol
    I2C_Write((Device << 1) | 0);           // Device address
    I2C_Write(Address);                     // Send register address
    I2C_ReStart();                          // Restart I2C
    I2C_Write((Device << 1) | 1);           // Initialize data read  
    second = I2C_Read(ACK);                 // Store the second
    minute = I2C_Read(ACK);                 // Store the minute
    hour = I2C_Read(ACK);                   // Store the hour
    dow = I2C_Read(ACK);                    // Store the day of the week
    day = I2C_Read(ACK);                    // Store the day
    month = I2C_Read(ACK);                  // Store the month
    year = I2C_Read(NAK);                   // Store the year and don't read more
    I2C_Stop();                             // Stop
}

void DS3231_Write_Time()
{
    
}

void DS3231_Setup_Time()
{  
    Device = 0x68;                          //Address of DS3231
    Address = 0x00;                         //Register 0x00 pointing to the register 'second'
    
    second = 0x00;                          //Set the second value
    minute = 0x00;                          //Set the minute value
    hour = 0x01;                            //Set the hour value
    dow = 0x00;                             //Set the day of the week
    day = 0x07;                             //Set the day
    month = 0x11;                           //Set the month
    year = 0x20;                            //Set the year
    
    
    I2C_Start();                            // Start I2C protocol
    I2C_Write((Device << 1) | 0);           // Device address
    I2C_Write(Address);                     // Send register address
    I2C_Write(second);                      // Initialize data read for second
    I2C_Write(minute);                      // Initialize data read for minute
    I2C_Write(hour);                        // Initialize data read for hour
    I2C_Write(dow);                         // Initialize data read for dow
    I2C_Write(day);                         // Initialize data read for day
    I2C_Write(month);                       // Initialize data read for month
    I2C_Write(year);                        // Initialize data read for year
    I2C_Stop(); 
}

void Wait_Half_Second()                     //Routine to wait half second
{
    T0CON = 0x03;                           // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                           // set the lower byte of TMR
    TMR0H = 0x0B;                           // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                  // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                   // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);         // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                   // turn off the Timer 0
}

void Wait_One_Second()
{
    Wait_Half_Second();                     //Wait half second
    SEC_LED = 0;                            //Turn off SEC_LED
    Wait_Half_Second();                     //Wait half second
    SEC_LED = 1;                            //Turn on SEC_LED
}

void Print_Temp()
{
    //PRint the temperature in C and F
    printf("Temperature: %.0f C (%.0f F)\n\r", DegC, DegF);
}

void Print_Time()
{
    //Print the Time
    printf("%02x:%02x:%02x %02x/%02x/%02x\t", hour, minute, second, month, day, year);
}

