#include <stdio.h>                                  // Include the following libraries to make program work
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "Main.h"
#include "I2C_Support.h"
#include "I2C_Soft.h"
#include "TFT_ST7735.h"
#include "Interrupt.h"
#include "Main_Screen.h"

#pragma config OSC      =   INTIO67                 // Configurations to program the chip
#pragma config BOREN    =   OFF
#pragma config WDT      =   OFF
#pragma config LVP      =   OFF
#pragma config CCP2MX   =   PORTBE

void Initialize_Screen(void);                       // Function to initialize the screen with proper labels on LCD
void Update_Screen(void);                           // Function to update the screen values
void Do_Init(void);                                 // Function to initialize the chip components
float read_volt();                                  // Function to read the voltage from AN0 (photoresistor)
int get_duty_cycle(int,int);                        // Function to calculate the duty cycle based off difference
                                                    // between the setup_temp and ambience temp
int get_RPM();                                      // Function to get the RPM by using Timer1 as the counter of rotations
void Monitor_Fan();                                 // Function to update the values of the fan onto LCD
void Turn_Off_Fan();                                // Function to turn off the fan and associated flags
void Turn_On_Fan();                                 // Function to turn on the fan and associate flags
unsigned int get_full_ADC();                        // Function to read Analog value from AN0
void Get_Temp(void);                                // Function to get the temperature values in C and F
void Update_Volt(void);                             // Function to update the voltage on the LCD
void Test_Alarm(void);                              // Function to check the alarm and whether or not to activate it
void Activate_Buzzer();                             // Function to activate the buzzer for the alarm
void Deactivate_Buzzer();                           // Function to deactivate the buzzer when alarm turns off

void Main_Screen(void);                             // Function to determine what to do while in the main screen for LCD
void Do_Setup(void);                                // Function to select which feature to setup
void do_update_pwm(char);                           // Function to update pwm based of duty cycle
void Set_RGB_Color(char color);                     // Function to change to color of the RGB during the 

char buffer[31]         = " ECE3301L Fall'20 L12\0";// Buffer Label for the title
char *nbr;                                          // char pointer to a nbr
char *txt;                                          // char pointer to txt
char tempC[]            = "+25";                    // text storage for tempC
char tempF[]            = "+77";                    // text storage for tempF
char time[]             = "00:00:00";               // text storage for time
char date[]             = "00/00/00";               // text storage for the date
char alarm_time[]       = "00:00:00";               // text storage for the alarm time
char Alarm_SW_Txt[]     = "OFF";                    // text storage for the alarm sw
char Fan_SW_Txt[]       = "OFF";                    // text storage for Heater Mode
char Fan_Set_Temp_Txt[] = "075F";                   // text storage for the set temperature 
char Volt_Txt[]         = "0.00V";                  // text storage for Volt     
char DC_Txt[]           = "000";                    // text storage for Duty Cycle value
char RTC_ALARM_Txt[]    = "0";                      // text storage for the actual alarm mode
char RPM_Txt[]          = "0000";                   // text storage for RPM

char setup_time[]       = "00:00:00";               // text storage for setup time
char setup_date[]       = "01/01/00";               // text storage for setup date
char setup_alarm_time[] = "00:00:00";               // text storage for the setup alarm time
char setup_fan_text[]   = "075F";                   // text storage for the setup fan temperature

signed int DS1621_tempC, DS1621_tempF;              // store the value of the temperatures C and 

int INT0_flag, INT1_flag, INT2_flag, Tach_cnt;      // Interrupt software flags
int ALARMEN;                                        // Checks if the alarm is enabled
int FANEN;                                          // Checks if the fan is enabled
int alarm_mode, MATCHED, color;                     // Checks the mode, if the time matches, and the color value of RGB
                                                    // Store time and date
unsigned char second, minute, hour, dow, day, month, year, prev_sec;
                                                    // Stores for alarm time
unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
                                                    // Stores the setting up the alarm time
unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
                                                    // Stores the setup time values
unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
unsigned char setup_fan_temp = 75;                  // Stores the setup fan temp
float volt;                                         // Stores the voltage reading from AN) (photoresistor)
int duty_cycle;                                     // Stores the value of the duty cycle
int rpm;                                            // Stores the value of the rpm

int Tach_cnt = 0;                                   // Stores the rps of the fan


void putch (char c)                                 
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Init_ADC()                                     // Function to initialize the ADC
{
    ADCON0 = 0x01;                                  // Read from channel 0 (AN0)
    ADCON1 = 0x0E;                                  // Make all inputs digital except AN0
    ADCON2 = 0xA9;                                  // Right Justify, 12 TAD, Fosc/8
}

void Init_IO()                                      // Function to initialize the input and output of the ports 
{
    TRISA = 0x11;                                   // All inputs excepts bit 0 and 4 are output for TRISA
    TRISB = 0xF7;                                   // All bits as input except bit 3 (for the buzzer) for TRISB
    TRISC = 0x01;                                   // Bit 0 is input, rest are output for TRISC
    TRISD = 0x00;                                   // All pins are output for TRISD
    TRISE = 0x07;                                   // All pins are input for TRISE
}

void Do_Init()                                      // Initialize everything needed
{ 
    init_UART();                                    // Initialize the uart
    OSCCON = 0x70;                                  // Set oscillator to 8 MHz 
    Init_ADC();                                     // Initialize the ADC registers
    Init_IO();                                      // Initialize IO ports
    RBPU = 0;                                       
       
    TMR1L = 0x00;                                   // TMR1L set to 0                           
    T1CON = 0x03;                                   // Enable TMR1 and use to count rps of fan
 
    T0CON = 0x03;                                   // TMR0 with 1:16 prescalar     
    TMR0H = 0x0B;                                   // TMR0H with 0x0B
    TMR0L = 0xDB;                                   // TMR0L with 0xDB
    INTCONbits.TMR0IF = 0;                          // Clear T0 flag
    T0CONbits.TMR0ON = 1;                           // TMR0 to start counting
    INTCONbits.TMR0IE = 1;                          // Enable TMR0
    Init_Interrupt();                               // Initialize the interrupts
        
    I2C_Init(100000);                               // Make the frequency 100000 for the I2C initialization            
    DS1621_Init();                                  // Initialize the DS1621 chip
} 

void main()
{
    Do_Init();                                      // Initialization    
    txt = buffer;                                   // txt now pointers to buffer
    Initialize_Screen();                            // Initialize the LCD screen
    prev_sec = 0xff;                                // 
    Turn_Off_Fan();
//  DS3231_Write_Initial_Alarm_Time();              // uncommented this line if alarm time was corrupted    
    DS3231_Read_Time();                             // Read time for the first time
    DS3231_Read_Alarm_Time();                       // Read alarm time for the first time
    DS3231_Turn_Off_Alarm();                        // Turn off the alarm
    alarm_mode = 0;
    ALARMEN = 0;
    MATCHED = 0;
    
    while(TRUE)                                     // Endless while loop
    { 
        if (enter_setup == 0)                       // If setup switch is LOW...
        {
            Main_Screen();                          // stay on main screen.
        }
        else                                        // Else if setup switch is HIGH
        {
            Do_Setup();                             // Go to setup screen.
        }
    }
}

void Main_Screen()
{
    /* write code to handle INT0 to turn off fan*/
    /* write code to handle INT1 to turn on fan*/
    /* write code to handle INT2 to toggle alarm sw - Use variable ALARMEN*/
    
    if (INT0_flag == 1)                             // Check for the button INT0_flag which turns off fan
    {
        INT0_flag = 0;                              // Clear INT0 flag
        printf("The fan is off\n\r");               // Print the fan is off in teraterm
        Turn_Off_Fan();                             // Turn off th fan
    }
    if (INT1_flag == 1)                             // Check for the button INT1_flag which turns on fan
    {
        INT1_flag = 0;                              // Clear INT1 flag
        printf("The fan is on\n\r");                // Print fan is on in teraterm
        Turn_On_Fan();                              // Turn on the fan
    }
    if (INT2_flag == 1)                             // Setup the time
    {
        INT2_flag = 0;                              // Clear INT0 flag
        ALARMEN = !ALARMEN;                         // Switch the status of Alarm enable
    }   
    DS3231_Read_Time();                             // Read time
    if (prev_sec != second)                         // Do the following when the second changes
    {
        prev_sec = second;                          // Make the previous second be the current second value
        Get_Temp();                                 // Get the temperature in C and F
        volt = read_volt();                         // Read  the voltage
        if (FANEN == 1)                             // If fan is enabled, show the fan, and monitor it
            Monitor_Fan();                          // Monitor the fan
        Test_Alarm();                               // call routine to handle the alarm  function

                                                    // Print the time, date, duty cycle, rpm on teraterm
        printf ("%02x:%02x:%02x %02x/%02x/%02x ",hour,minute,second,month,day,year);
        printf ("duty cycle = %d  RPM = %d ", duty_cycle, rpm); 

        Update_Screen();                            // Update the values for time, date, dc, rpm, and status of the enable signals
    }    
}

void Do_Setup()                                     // Function to select which setup screen to go to
{
    /* add code to decode the value of the switch 'setup_sel1' and 'setup_sel0' to call:*/
    /* 00 for Setup_Time()*/
    /* 01 for Setup_Alarm_Time()*/
    /* 10 for Setup_Temp_Fan()*/
    
    if (setup_sel0 == 0 && setup_sel1 == 0)         // Case for setting up the time and date
    {
        Setup_Time();                               // Function to setup the time and date
    }
    if (setup_sel0 == 1 && setup_sel1 == 0)         // Case for setting up the alarm time
    {       
        Setup_Alarm_Time();                         // Function to setup the alarm time
    }
    if (setup_sel0 == 0 && setup_sel1 == 1)         // Case for setting up the fan temp
    {   
        Setup_Temp_Fan();                           // Function to setup the fan temp
    }
}

void Get_Temp(void)                                 // Function to calculate the temperature in C and F
{
    DS1621_tempC = DS1621_Read_Temp();              // Read temp from the chip


    if ((DS1621_tempC & 0x80) == 0x80)              // Case if the temperature in C is negative
    {
        DS1621_tempC = 0x80 - (DS1621_tempC & 0x7f);// Calculate the temp in c
        DS1621_tempF = 32 - DS1621_tempC * 9 /5;    // Calculate the temp in F
                                                    // Print temp in teraterm
        printf ("Temperature = -%dC or %dF\r\n", DS1621_tempC, DS1621_tempF);
        DS1621_tempC = 0x80 | DS1621_tempC;         // Reconfigure tempc value        
    }
    else                                            // If temp in C is positive
    {
        DS1621_tempF = DS1621_tempC * 9 /5 + 32;    // Calculate the temp F
                                                    // Print temp in teraterm
        printf ("Temperature = %dC or %dF\r\n", DS1621_tempC, DS1621_tempF);            
    }
}

void Monitor_Fan()
{
                                                    // Calculate the duty cycle by comparing ambient temp and setup temp
    duty_cycle = get_duty_cycle(DS1621_tempF, setup_fan_temp);
    do_update_pwm(duty_cycle);                      // Update the pwm based on the duty cycle
    rpm = get_RPM();                                // Find the rpm of the fan as it works
}

float read_volt()                                   // Function to get the voltage
{
	int nStep = get_full_ADC();                     // Get the amout of steps
    volt = nStep * 5 /1024.0;                       // Calculate the voltage 
	return (volt);                                  // Return the voltage that was calculated
}

int get_duty_cycle(int temp, int set_temp)          // Function to get the duty cycle
{	
int dc;                                             // store the dc calculation
    dc = 2*(set_temp - temp);                       // calculate the dc based off temperature difference of ambience and setup
    if (dc > 100)                                   // Prevent duty cycle from going over 100
        dc = 100;
    if (dc < 0)                                     // Prevent duty cycle from going under 0
        dc = 0; 
    return dc;                                      // Return the dc calculated value
}

int get_RPM()                                       // Function to get the rpm from TMR1
{
    return Tach_cnt*60;                             // Return the rpm by multiplying the rps stored in Tach_cnt by 60
}


void Turn_Off_Fan()                                 // Function to turn off fan and corresponding signals
{
    duty_cycle = 0;
    do_update_pwm(duty_cycle);                      // based on the new duty cycle = 0
    rpm = 0;                                        // 0 rotations due to duty cycle = 0
    FANEN = 0;                                      // Set variable FANEN to 0
    FANEN_LED = 0;                                  // Turn off FANEN_LED
}

void Turn_On_Fan()                                  // Function to turn on the fan
{
    FANEN = 1;                                      // Set variable FANEN to 1
    FANEN_LED = 1;                                  // Turn on FANEN_LED
}

void do_update_pwm(char duty_cycle)                 // Function to change the pwm
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ;                              // set the frequency for 25 Khz
    T2CON = 0b00000111 ;                            //
    dc_f = ( 4.0 * duty_cycle / 20.0) ;             // calculate factor of duty cycle versus a 25 Khz
                                                    // signal
    dc_I = (int) dc_f;                              // get the integer part
    if (dc_I > duty_cycle) dc_I++;                  // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 
}

unsigned int get_full_ADC()                         // Function to get the full ADC
{
    unsigned int result;                            // Store the result from the ADC
    ADCON0bits.GO=1;                                // Start Conversion
    while(ADCON0bits.DONE==1);                      // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;             // combine result of upper byte and
                                                    // lower byte into result
    return result;                                  // return the result.
}

void Activate_Buzzer()                              // Function to activate the buzzer
{       
    PR2 = 0b11111001 ;                              // Set the PR2 to have 0b11111001
    T2CON = 0b00000101 ;                            // T2CON filled with 0x05
    CCPR2L = 0b01001010 ;                           // Set up the CCPR2L
    CCP2CON = 0b00111100 ;                          // Set up the CCP2CON
}

void Deactivate_Buzzer()                            // Function to deactivate the buzzer
{
    CCP2CON = 0x0;                                  // CCP2CON with 0
    PORTBbits.RB3 = 0;                              // Turn off the signal to the buzzer
}

void Test_Alarm()
{
    // ALARMEN, alarm_mode, MATCHED, RTC_ALARM_NOT
    // put code to detect whether the alarm is activated, or deactivated or is waiting for the alarm to 
    // happen
    
    // the variable ALARMEN is used as the switch that is toggled by the user
    // the variable alarm_mode stored the actual mode of the alarm. If 1, the alarm is activated. If 0, no
    // alarm is enabled yet
    
    // the RTC_ALRAM_NOT is the signal coming from the RTC to show whether the alarm time does match with
    // the actual time. This signal is active low when the time matches.
    
    // Use a variable MATCHED to register the event that the time match has occurred. This is needed
    // to change the color of the RGB LED
    
    // this routine should perform the different conditions:
    // Case 1: switch is turned on but alarm_mode is not on
    // Case 2: switch is turned off but alarm mode is already on
    // Case 3: switch is on and alarm_mode is on. In this case, more checks are to be performed.
    // Use the provided function DS3231_Turn_On_Alarm() and DS3231_Turn_Off_Alarm() to activate or deactivate
    // the alarm.
    
    if (ALARMEN == 1 && alarm_mode == 0)            // Case 1: switch is turned on but alarm_mode is not on
    {
        color = 0;                                  // color set to 0 (no color)
        Set_RGB_Color(color);                       // Set RGB to designated color
        MATCHED = 0;                                // MATCHED = 0
        alarm_mode = 0;                             // alarm_mode was turned off
        Deactivate_Buzzer();                        // Deactivate the buzzer
        if (RTC_ALARM_NOT == 0)                     // If the time matches
        {
            alarm_mode = 1;                         // Go into alarm_mode
        }
    }
    else if (ALARMEN == 0 && alarm_mode == 1)       // Case 2: switch is turned off but alarm mode is already on
    {
        alarm_mode = 0;                             // Turn off the alarm mode
        color = 0;                                  // Set the color to 0 (no color)
        MATCHED = 0;                                // It no longer matches the time because we left the alarm enable
        Set_RGB_Color(color);                       // Show the designated color
        DS3231_Turn_Off_Alarm();                    // Turn off the alarm
        Deactivate_Buzzer();                        // Deactivate the buzzer for the alarm
    }
    else if (ALARMEN == 1 && alarm_mode == 1)       // Case 3: switch is on and alarm_mode is on. In this case, more checks are to be performed.
    {
        if (RTC_ALARM_NOT == 0 && MATCHED == 0)      // IF the alarm mode is on and the switch is on
        {
            MATCHED = 1;                            // MATCHED = 1
            Activate_Buzzer();                      // Activate the buzzing noise
        }
        if (RTC_ALARM_NOT == 0 && MATCHED == 1)      // RTC_ALARM is active low
        {
            Set_RGB_Color(color);                   // Display Color
            color++;                                // Modify the color
            if (volt > 3.5)                         // If the photoresistor is covered
            {
                alarm_mode = 0;                     // alarm_mode is 0
                color = 0 ;                         // color to 0 (No Color)
                Set_RGB_Color(color);               // Display the color 
                Deactivate_Buzzer();                // Deactivate the buzzer
                DS3231_Turn_Off_Alarm();            // Turn off the alarm
                MATCHED = 0;                        // No longer matched
            }
        }
    }
    else
    {
        DS3231_Turn_Off_Alarm();                    // Turn off the alarm
    }
}

void Set_RGB_Color(char color)                      // Function to set the color of the RGB
{
    PORTD = (PORTD & 0x1F)|(color<<5&0xE0);         // Only change the pins affecting the RGB and display designated color
}

