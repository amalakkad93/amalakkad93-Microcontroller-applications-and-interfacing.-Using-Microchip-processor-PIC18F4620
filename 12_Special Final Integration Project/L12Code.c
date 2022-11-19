#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>


#pragma config OSC      =   INTIO67
#pragma config BOREN    =   OFF
#pragma config WDT      =   OFF
#pragma config LVP      =   OFF
#pragma config CCP2MX   =   PORTBE

#define SEC_LED         PORTEbits.RE0                   //Pin for SEC_LED



#define ACCESS_CFG      0xAC                            //Code to ACCES_CFG
#define START_CONV      0xEE                            //Code to START_CONV
#define READ_TEMP       0xAA                            //Code to READ_TEMP
#define CONT_CONV       0x02                            //Code to CONT_CONV

#define ACK             1                               //Definition to acknowledge
#define NAK             0                               //Definition of not anknowledge

#define TFT_DC          PORTDbits.RD0                   //Pin for TFT_DC
#define TFT_CS          PORTDbits.RD1                   //Pin for TFT_CS
#define TFT_RST         PORTDbits.RD2                   //Pin for TFT_RST

#define _XTAL_FREQ      8000000
#define TMR_CLOCK       _XTAL_FREQ/4        

void Initialize_Screen(void);                           //Prototype for function to intialize LCD screen
void Update_Screen(void);                               //Prototype for function to update values on LCD screen
void Do_Init(void);                                     //Prototype for function to start all initialization of ports
void Init_Timer_1(); 
void Set_RPM_RGB(int );                                 //Prototype for function to set the color of RGB for RPM
void Set_DC_RGB(int);                                   //Prototype for function to set the color of RGB for DC
float read_volt();                                      //Prototype for function to read the voltage
int get_duty_cycle();                                   //Prototype for function to the duty cycle based on voltage
int get_RPM();                                          //Prototype for function to output the RPM of the fan
void Monitor_Fan();                                     //Prototype for function to monitor fan and update values regarding fan
void Show_Fan_On();                                     //Prototype for function to show fan is on in the LCD
void Show_Fan_Off();                                    //Prototype for function to show fan is off in the LCD
void Turn_Off_Fan();                                    //Prototype for function to turn off the fan
void draw_bar_graph_dc(int);                            //Prototype for function to draw the text for dc
void draw_bar_graph_rpm(int);                           //Prototype for function to draw the text for rpm
unsigned int get_full_ADC();                            //Prototype for function to get the adc value
void Wait_Half_Second();                                //Prototype for function to wait half a second
void Update_Volt();                                     //Prototype for function to update the voltage readings
void DS1621_Init();                                     //Prototype for function to initialize the DS1621
char DS1621_Read_Temp();                                //Prototype for function to read the temperature
void DS3231_Read_Time();                                //Prototype for function to read the time
void DS3231_Write_Time();                               //Prototype for function to write the time
void DS3231_Setup_Time();                               //Prototype for function to setup the time fot the device
void Print_Time();                                      //Prototype for function to print the time
void Print_Temp();                                      //Prototype for function to print the temperature

void INT0_ISR();                                        //Prototype for function to INT0_ISR
void INT1_ISR();                                        //Prototype for function to INT1_ISR
void INT2_ISR();                                        //Prototype for function to INT2_ISR
void do_update_pwm(char);                               //Prototype for function to update the pwm


#define  SCL_PIN  PORTDbits.RD3                         //PORT location for SCL pin
#define  SCL_DIR  TRISDbits.RD3                         //TRISD location for SCL
#define  SDA_PIN  PORTDbits.RD4                         //PORT location for SDA pin
#define  SDA_DIR  TRISDbits.RD4                         //TRISD location for SDA
#include "softi2c.c"                                    //Include the library for software I2C

#define start_x             1                           // X position of starting
#define start_y             1                           // Y position of starting 
#define temp_x              28                          // X position of temp label
#define temp_y              13                          // Y position of temp label
#define circ_x              40                          // X position of circle
#define circ_y              25                          // Y position of circle
#define data_tempc_x        15                          // X position of temp c data
#define data_tempc_y        25                          // Y position of temp c data
#define tempc_x             45                          // X position of temp c label
#define tempc_y             25                          // Y position of temp c label
#define cirf_x              95                          // X position of cirf
#define cirf_y              25                          // Y position of cirf
#define data_tempf_x        70                          // X position of temp f data
#define data_tempf_y        25                          // Y position of temp f data
#define tempf_x             100                         // X position of temp f label
#define tempf_y             25                          // Y position of temp f label
#define time_x              50                          // X position of time label
#define time_y              43                          // Y position of time label
#define data_time_x         15                          // X position of time data
#define data_time_y         55                          // Y position of time data
#define date_x              50                          // X position of date label
#define date_y              73                          // Y position of date label
#define data_date_x         15                          // X position of date data
#define data_date_y         85                          // Y position of date data
#define fan_x               55                          // X position of fan label
#define fan_y               105                         // Y position of fan label
#define data_fan_x          50                          // X position of fan data
#define data_fan_y          118                         // Y position of fan data

#define volt_x              7                           // X position of volt label
#define volt_y              140                         // Y position of volt label
#define data_volt_x         6                           // X position of volt data
#define data_volt_y         150                         // Y position of volt data
#define dc_x                60                          // X position of dc label
#define dc_y                140                         // Y position of dc label
#define data_dc_x           60                          // X position of dc data
#define data_dc_y           150                         // Y position of dc data
#define rpm_x               102                         // X position of rpm label
#define rpm_y               140                         // Y position of rpm label
#define data_rpm_x          100                         // X position of rpm data
#define data_rpm_y          150                         // Y position of rpm data
#define TS_1                1                           // Size of Normal Text
#define TS_2                2                           // Size of Number Text 

#include "ST7735_TFT.c"                                 // Include the library attached for the TFT
#define FANEN_LED           PORTAbits.RA5               // PORT locatioin for FANEN
#define DC_RED              PORTAbits.RA1               // PORT locatioin for DC_RED
#define DC_GRN              PORTAbits.RA2               // PORT locatioin for DC_GRN
#define RPM_GRN             PORTDbits.RD6               // PORT locatioin for RPM_GRN
#define RPM_BLU             PORTDbits.RD7               // PORT locatioin for RPM_BLU

char buffer[31]     = "ECE3301L Fall'20 L12\0";
char *nbr;                                              // Po
char *txt;                                              // Pointer for txt
char tempC[]        = "25";                             // text storage for tempC
char tempF[]        = "77";                             // text storage for tempF
char time[]         = "00:00:00";                       // text storage for time
char date[]         = "00/00/00";                       // text storage for Date

char Fan_Txt[]      = "OFF";                            // text storage for Fan Mode
char Volt_Txt[]     = "0.00V";                          // text storage for Volt     
char DC_Txt[]       = "00";                             // text storage for Duty Cycle value
char RPM_Txt[]      = "0000";                           // text storage for RPM

void Turn_On_Fan();                                     // Prototype for turning fan on
void Turn_Off_Fan();                                    // Prototype for turning fan off

int DS1621_tempC, DS1621_tempF;                         // Temperatures in C and F

int INT0_flag, INT1_flag, INT2_flag, T0_flag;           // INT flags
unsigned char second, minute, hour, dow, day, month, year, prev_second;
                                                        // Info for the date and time
char Device,Cmd,Address;                                // Information to access chip info

float volt;                                             // Voltage stored globally
int duty_cycle;                                         // Duty cycle stored here
int rpm;                                                // rpm stored here
int FANEN;                                              // bit to check if Fan is enabled
int Tach_cnt = 0;                                       // Not implemented

int Half_sec_cnt = 0;                                   // Initialize half_sec count ***

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

void interrupt  high_priority chkisr() 
{ 
    if (INTCONbits.INT0IF == 1) INT0_ISR();             //Go to INT0_ISR. Turn off fan
    if (INTCON3bits.INT1IF == 1) INT1_ISR();            //Go to INT1_ISR. Turn on fan
    if (INTCON3bits.INT2IF == 1) INT2_ISR();            //Go to INT2_ISR. Set-up TIme
}

void INT0_ISR() 
{    
   INTCONbits.INT0IF = 0;                               //Clear interrupt flag
   INT0_flag = 1;                                       //Set software INT0 flag
} 

void INT1_ISR() 
{ 
   INTCON3bits.INT1IF = 0;                              //Clear interrupt flag
   INT1_flag = 1;                                       //Set software INT1 flag
}

void INT2_ISR() 
{    
   INTCON3bits.INT2IF = 0;                              //Clear interrupt flag
    INT2_flag = 1;                                      //Set software INT1 flag
} 

void Initialize_Screen(void) 
{ 
    LCD_Reset();                                        // Screen reset
    TFT_GreenTab_Initialize();         
    fillScreen(ST7735_BLACK);                           // Fills background of screen with color passed to it
    
    txt = buffer;
    strcpy(txt, "ECE3301L Fall'20 L12");                                    // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   // X and Y coordinates of where the text is to be displayed
    strcpy(txt, "Temperature:");                                            //Make text to "Temperature"
    drawtext(temp_x  , temp_y , txt, ST7735_MAGENTA, ST7735_BLACK, TS_1);   // including text color and the background of it
    drawCircle(circ_x, circ_y , 2  , ST7735_YELLOW);                        //Draw the circle symbol for degree C
    strcpy(txt, "C/");                                                      //Make text to "C/"
    drawtext(tempc_x , tempc_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2);   //Display the label "C"
    strcpy(txt, "F");                                                       //Make text to "D"
    drawCircle(cirf_x, cirf_y , 2  , ST7735_YELLOW);                        //Draw the circle symbol for degree F
    drawtext(tempf_x , tempf_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2);   //Display Label "F"
    strcpy(txt, "Time");                                                    //Make text to "Time"
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);   //Display label "Time"
    strcpy(txt, "Date");                                                    //Make text to "Date"
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);   //Display label "Date"
    strcpy(txt, "FAN:");                                                    //Make text to "FAN"
    drawtext(fan_x, fan_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);        //Display label "FAN:"
    strcpy(txt, "VOLT");                                                    //Make text to "VOLT"
    drawtext(volt_x, volt_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);      //Display label "VOLT"
    strcpy(txt, "DC");                                                      //Make text to "DC"
    drawtext(dc_x, dc_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);          //Display label "DC"
    strcpy(txt, "RPM");                                                     //Make text to "RPM"
    drawtext(rpm_x, rpm_y, txt, ST7735_WHITE    , ST7735_BLACK  , TS_1);    //Display label "RPM"
    drawtext(data_tempc_x, data_tempc_y, tempC  , ST7735_YELLOW , ST7735_BLACK , TS_2); //Display tempC 
    drawtext(data_tempf_x, data_tempf_y, tempF  , ST7735_YELLOW , ST7735_BLACK , TS_2); //Display tempF
    drawtext(data_time_x , data_time_y , time   , ST7735_CYAN   , ST7735_BLACK , TS_2); //Display time 
    drawtext(data_date_x , data_date_y , date   , ST7735_GREEN  , ST7735_BLACK , TS_2); //Display date
    drawtext(data_fan_x  , data_fan_y  , Fan_Txt, ST7735_RED    , ST7735_BLACK , TS_2); //Display fan "ON" or "OFF"
    drawtext(data_dc_x   , data_dc_y   , DC_Txt , ST7735_RED    , ST7735_BLACK , TS_1); //Display Duty cycle value
    drawtext(data_rpm_x  , data_rpm_y  , RPM_Txt, ST7735_RED    , ST7735_BLACK , TS_1); //Display RPM value
}

void Update_Screen(void)
{
    tempC[0]  = DS1621_tempC/10 + '0';                  // Tens digit of C
    tempC[1]  = DS1621_tempC%10 + '0';                  // Ones digit of C
    tempF[0]  = DS1621_tempF/10 + '0';                  // Tens digit of F
    tempF[1]  = DS1621_tempF%10 + '0';                  // Ones digit of F
    time[0]  = (hour>>4) + '0';                         // Hour MSB
    time[1]  = (hour & 0x0F) + '0';                     // Hour LSB
    time[3]  = (minute>>4) + '0';                       // Minute MSB
    time[4]  = (minute & 0x0F) + '0';                   // Minute LSB
    time[6]  = (second>>4) + '0';                       // Second MSB
    time[7]  = (second & 0x0F) + '0';                   // Second LSB
    date[0]  = (month>>4) + '0';                        // Month MSB
    date[1]  = (month & 0x0F) + '0';                    // Month LSB
    date[3]  = (day>>4) + '0';                          // Day MSB
    date[4]  = (day & 0x0F) + '0';                      // Day LSB
    date[6]  = (year>>4) + '0';                         // Year MSB
    date[7]  = (year & 0x0F) + '0';                     // Year LSB
                      
    drawtext(data_tempc_x, data_tempc_y, tempC , ST7735_YELLOW , ST7735_BLACK , TS_2);  // Display the temp in C       
    drawtext(data_tempf_x, data_tempf_y, tempF , ST7735_YELLOW , ST7735_BLACK , TS_2);  // Display the temp in F
    drawtext(data_time_x , data_time_y , time  , ST7735_CYAN   , ST7735_BLACK , TS_2);  // Display the time
    drawtext(data_date_x , data_date_y , date  , ST7735_GREEN  , ST7735_BLACK , TS_2);  // Display the date
}

void Init_ADC()
{
    ADCON0 = 0x01;                                      //Read from channel 0 (AN0)
    ADCON1 = 0x0E;                                      //Make all inputs digital except AN0
    ADCON2 = 0xA9;                                      //Right Justify, 12 TAD, Fosc/8
}

void Init_IO()
{
    TRISA = 0x01;                                       //PORTA configured as all outputs except bit 0
    TRISB = 0xFF;                                       //PORTB configured as all inputs
    TRISC = 0x01;                                       //PORTC configured as all outputs except bit 0
    TRISD = 0x00;                                       //PORTD configured as all outputs
    TRISE = 0x00;                                       //PORTE configured as all outputs
}

void Init_Interrupt()
{
    RBPU = 0;
    INTCONbits.INT0IF = 0;                              // Clear INT0IF
    INTCON3bits.INT1IF = 0;                             // Clear INT1IF
    INTCON3bits.INT2IF = 0;                             // Clear INT2IF
    
    INTCON2bits.INTEDG0 = 0;                            // INT0 EDGE falling
    INTCON2bits.INTEDG1 = 0;                            // INT1 EDGE falling
    INTCON2bits.INTEDG2 = 1;                            // INT2 EDGE rising
    
    INTCONbits.INT0IE = 1;                              // Set INT0 IE
    INTCON3bits.INT1IE = 1;                             // Set INT1 IE
    INTCON3bits.INT2IE = 1;                             // Set INT2 IE
    
    INTCONbits.GIE = 1;                                 // Set the Global Interrupt Enable
}
void Do_Init()                                          // Initialize the ports 
{
    init_UART();                                        // Initialize the uart
    OSCCON =0x70;                                       // Set oscillator to 8 MHz 
    Init_ADC();                                         //Initialize the ADC
    Init_IO();                                          //Initialize the Input/Out
    Init_Interrupt();                                   //Initialize the Interrupt
    I2C_Init(100000);                                   // Initialize I2C Master with 100KHz clock
    DS1621_Init();                                      // Initialize DS1621 chip
} 

void main()
{
    Do_Init();                                          // Initialization     
    txt = buffer;                                       //Txt to buffer
    Initialize_Screen();                                //Initialize the screen
    prev_second = 0xFF;                                 //Set the prev_second to an intangible second value
    Turn_Off_Fan();                                     // Default mode is off when starting
 
    while(1)
    {  
        if (INT0_flag == 1)                             //Check for the button INT0_flag which turns off fan
        {
            printf("The fan is off\n\r");               //Print the fan is off in teraterm
            Turn_Off_Fan();                             //Turn off th fan
            INT0_flag = 0;                              //Clear INT0 flag
            FANEN = 0;                                  //Make FANEN = 0
        }
        if (INT1_flag == 1)                             //Check for the button INT1_flag which turns on fan
        {
            printf("The fan is on\n\r");                //Print fan is on in teraterm
            Turn_On_Fan();                              //Turn on the fan
            INT1_flag = 0;                              //Clear INT1 flag
            FANEN = 1;                                  //Make FANEN = 1
        }
        if (INT2_flag == 1)                             //Setup the time
        {
            INT2_flag = 0;                              //Clear INT0 flag
            DS3231_Setup_Time();                        //Setup the time
            prev_second = 0xFF;                         //Reset the prev_second
            printf("Time was reset!\n\r");
        }
        DS3231_Read_Time();                             //Read Time
        if(prev_second != second)
        {
            prev_second = second;                       //Make the new second as the prev_second for next while loop
            Print_Time();
            DS1621_tempC = DS1621_Read_Temp();          //Read the temperature in C 
            DS1621_tempF = (DS1621_tempC*9)/5 + 32;     //Convert the temperature
            Print_Temp();                               //Print the temperature
            Update_Volt(volt);                          //Update the voltage readings
            if (FANEN == 1)                             //If fan is enabled, show the fan, and monitor it
                Monitor_Fan();                          //Look at important information of the fan and display on LCD
            Update_Screen();
        }
    }
}

void Monitor_Fan()                                      //Find and display information regarding the fan
{
    volt = read_volt();                                 //Read voltage from AN0
    duty_cycle = get_duty_cycle();                      //Get the duty cycle
    do_update_pwm(duty_cycle) ;                         //Update information
    rpm = get_RPM();                                    //Get the rpm
    Set_DC_RGB(duty_cycle);                             //Set the DC RGB to corresponding color
    Set_RPM_RGB(rpm);                                   //Set the RPM RGB to corresponding color
    printf ("\tduty cycle = %d RPM = %d\r\n",duty_cycle, rpm);
    //***Wait_Half_Second();                                 //Wait 0.5 Seconds
}

void Update_Volt()                                      //Update the voltage readings
{
    volt = read_volt();                                 //Get the voltage from AN0
    Volt_Txt[0]= ((int) volt) + '0';                    //Get the ones place of voltage
    Volt_Txt[2]= (((int)(volt*10))%10) + '0';           //Get the tenths place for voltage 
    Volt_Txt[3]= (((int)(volt*100))%10) + '0';          //Get the hundredths place for voltage
    drawtext(data_volt_x, data_volt_y, Volt_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
                                                        //Display voltage on LCD
}

float read_volt()                                       //Find the voltage coming in the circuit
{
 float volt;                                            //Where the voltage is stored
 int nStep = get_full_ADC();                            //Count the steps form AN0 for voltage
    volt = nStep * 5 /1024.0;                           //Calculate the voltage
    return (volt);                                      //Return the voltage read and calculated
}

int get_duty_cycle()                                    //Routine to calculate the duty cycle
{	
int dc;                                                 //Where the calculation is stored before returning it                 
    dc = (int) (read_volt() / 5.0 * 100.0);             //Calculation of duty cycle
    return (dc); 
}

void Wait_Half_Second()
{
    T0CON = 0x03;                                       // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                       // set the lower byte of TMR
    TMR0H = 0x0B;                                       // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                              // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                               // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                     // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                               // turn off the Timer 0
    SEC_LED = ~SEC_LED;
}

int get_RPM()                                           //Routine to calculate the RPM
{
    TMR1L = 0x00;                                       // clear the count
    T1CON = 0x03;                                       // start Timer1 as counter of number of pulses
    Wait_Half_Second();                                 // wait half second
 int RPS = TMR1L;                                       // read the count. Since there are 2 pulses per rev
                                                        // and 2 ½ sec per sec, then RPS = count
    return (RPS * 60);                                  // return RPM = 60 * RPS 
}

void Show_Fan_On()
{
    Fan_Txt[1] = 'N';                                   // Change F to N
    Fan_Txt[2] = ' ';                                   // Change F to ''
    drawtext(data_fan_x, data_fan_y, Fan_Txt, ST7735_GREEN, ST7735_BLACK, TS_2);    // Change entire text to ON
}

void Show_Fan_Off()
{
    Fan_Txt[1] = 'F';                                   //Change the text from N to F
    Fan_Txt[2] = 'F';                                   //Change the text from '' to F
    drawtext(data_fan_x, data_fan_y, Fan_Txt,ST7735_RED, ST7735_BLACK, TS_2);   //Change entire text to OFF
}

void Turn_Off_Fan()                                     //Turn the fan off
{
    Show_Fan_Off();                                     //Display that the fan is off on LCD
    duty_cycle = 0;
    do_update_pwm(duty_cycle);                          //based on the new duty cycle = 0
    Set_DC_RGB(duty_cycle);                             //Change the DC_RGB color to duty cycle = 0
    rpm = 0;                                            //0 rotations due to duty cycle = 0
    Set_RPM_RGB(rpm);                                   //Change the RPM_RGB color to RPM = 0
    FANEN = 0;                                          //Set variable FANEN to 0
    FANEN_LED = 0;                                      //Turn off FANEN_LED
}

void Turn_On_Fan()                                      //Turn fan on
{
    Show_Fan_On();                                      //Show the fan is off on display
    FANEN = 1;                                          //Set variable FANEN to 1
    FANEN_LED = 1;                                      //Turn on FANEN_LED
}

void do_update_pwm(char duty_cycle)                     //Update the pulse width modulation
{
 float dc_f;
 int dc_I;
    PR2 = 0b00000100 ;                                  // set the frequency for 25 Khz
    T2CON = 0b00000111 ;                                //
    dc_f = ( 4.0 * duty_cycle / 20.0) ;                 // calculate factor of duty cycle versus a 25 Khz
                                                        // signal
    dc_I = (int) dc_f;                                  // get the integer part
    if (dc_I > duty_cycle) dc_I++;                      // round up function
        CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 
}
void DS3231_Setup_Time()
{  
    Device = 0x68;                                      //Address of DS3231
    Address = 0x00;                                     //Register 0x00 pointing to the register 'second'
    
    second = 0x00;                                      //Set the second value
    minute = 0x00;                                      //Set the minute value
    hour = 0x07;                                        //Set the hour value
    dow = 0x00;                                         //Set the day of the week
    day = 0x08;                                         //Set the day
    month = 0x11;                                       //Set the month
    year = 0x20;                                        //Set the year
    
    
    I2C_Start();                                        // Start I2C protocol
    I2C_Write((Device << 1) | 0);                       // Device address
    I2C_Write(Address);                                 // Send register address
    I2C_Write(second);                                  // Initialize data read for second
    I2C_Write(minute);                                  // Initialize data read for minute
    I2C_Write(hour);                                    // Initialize data read for hour
    I2C_Write(dow);                                     // Initialize data read for dow
    I2C_Write(day);                                     // Initialize data read for day
    I2C_Write(month);                                   // Initialize data read for month
    I2C_Write(year);                                    // Initialize data read for year
    I2C_Stop(); 
}

char DS1621_Read_Temp()
{
char Data_Ret;
    Device = 0x48;                                      //Address for DS1621
    Cmd = READ_TEMP;                                    //Command to read temp
    I2C_Start();                                        // Start I2C protocol
    I2C_Write((Device << 1) | 0);                       // Device address
    I2C_Write(Cmd);                                     // Send register address
    I2C_ReStart();                                      // Restart I2C
    I2C_Write((Device << 1) | 1);                       // Initialize data read
    Data_Ret = I2C_Read(NAK);                           // Read Data
    I2C_Stop();                                         // Stop
    return Data_Ret;
}

void DS1621_Init()
{
    Device = 0x48;                                      //Address of DS1621 device 
                                                        //Code to start the device
	I2C_Write_Cmd_Write_Data(Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);
}

void DS3231_Read_Time()
{
    Device = 0x68;                                      // Address for DS3231
    Address = 0x00;                                     // Register 0x00 pointing to the register 'second'
    I2C_Start();                                        // Start I2C protocol
    I2C_Write((Device << 1) | 0);                       // Device address
    I2C_Write(Address);                                 // Send register address
    I2C_ReStart();                                      // Restart I2C
    I2C_Write((Device << 1) | 1);                       // Initialize data read  
    second = I2C_Read(ACK);                             // Store the second
    minute = I2C_Read(ACK);                             // Store the minute
    hour = I2C_Read(ACK);                               // Store the hour
    dow = I2C_Read(ACK);                                // Store the day of the week
    day = I2C_Read(ACK);                                // Store the day
    month = I2C_Read(ACK);                              // Store the month
    year = I2C_Read(NAK);                               // Store the year and don't read more
    I2C_Stop();                                         // Stop
}

void DS3231_Write_Time()
{
   // not implemented
}

void Set_DC_RGB(int duty_cycle)                         //Routine to change DC_RGB
{
    if (duty_cycle == 0)                                //Make the DC_RGB turn off
    {
        DC_RED = 0;
        DC_GRN = 0;
    }
    else if (duty_cycle > 0 && duty_cycle < 33)         //Make the DC_RGB RED
    {
        DC_RED = 1;
        DC_GRN = 0;
    }
    else if (duty_cycle >= 33 && duty_cycle < 66)       //Make the DC_RGB Yellow
    {
        DC_RED = 1;
        DC_GRN = 1;
    }
    else if (duty_cycle >= 67)                          //Make the DC_RGB Green
    {
        DC_RED = 0;
        DC_GRN = 1;
    }
    draw_bar_graph_dc(duty_cycle);                      //draw bar graph for DC
}

void Set_RPM_RGB(int RPM)                               //Routine to change RPM_RGB
{
    if (RPM == 0)                                       //Make the RPM_RGB turn off
    {
        RPM_GRN = 0;
        RPM_BLU = 0;
    }
    else if (RPM > 0 && RPM < 1200)                     //Make the RPM_RGB Green
    {
        RPM_GRN = 1;
        RPM_BLU = 0;
    }
    else if (RPM >= 1200 && RPM < 2400)                 //Make the RPM_RGB Cyan
    {
        RPM_GRN = 1;
        RPM_BLU = 1;
    }
    else if (RPM >= 2400)                               //Make the RPM_RGB Blue
    {
        RPM_GRN = 0;
        RPM_BLU = 1;
    }
    draw_bar_graph_rpm(RPM);                            //Draw bar graph for RPM
}

unsigned int get_full_ADC()             
{
unsigned int result;                                    //Store the result from the ADC
   ADCON0bits.GO=1;                                     // Start Conversion
   while(ADCON0bits.DONE==1);                           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                  // combine result of upper byte and
                                                        // lower byte into result
   return result;                                       // return the result.
}

void draw_bar_graph_dc(int dc)
{
    DC_Txt[0] = dc/10  + '0';                           //DC MSB
    DC_Txt[1] = dc%10  + '0';                           //DC LSB
    
    if (dc == 0)                                                                    //Case for 0% DC
    {
       drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_RED, ST7735_BLACK, TS_1);      //Display text in red
    }
    else if (dc < 33)                                                               //Case for <33%
    {
       drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_RED, ST7735_BLACK, TS_1);      //Display text in red
    }
    else if (dc >=33 && dc < 67)                                                    //Case for 33%>DC>66%
    {
       drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_YELLOW, ST7735_BLACK, TS_1);   //Display text in yellow
    }
    else                                                                            //Case for DC>66%
    {
        drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);   //Display text in green
    }    
}

void draw_bar_graph_rpm(int rpm)
{
int bar;
    RPM_Txt[0] = rpm/1000  + '0';                       //Calculate number in thousands place
    RPM_Txt[1] = (rpm/100)%10 + '0';                    //Calculate number in hundreds place
    RPM_Txt[2] = (rpm/10)%10 + '0';                     //Calculate number in tens place
    RPM_Txt[3] = (rpm)%10 + '0';                        //Calculate number in ones place   
 
    bar = (rpm/36);                                     //Calculate the percentage for the RPM bar graph
    if (bar == 0)                                       //Case if off
    {
        drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);    //Display text in green
    }
    else if (bar < 33)                                                                  //Case if bar < 33%
    {
        drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);    //Display text in green
    }
    else if (bar >=33 && bar < 67)                                                      //Case if 33%>bar>66%
    {
        drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_CYAN, ST7735_BLACK, TS_1);     //Display text in cyan
    }
    else                                                                                //Case if bar > 66%
    {
        drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_BLUE, ST7735_BLACK, TS_1);     //Display text in blue
    }  
}

void Print_Temp()
{
    //Print the temperature in C and F
    printf("Temperature: %d C (%d F)\n\r", DS1621_tempC, DS1621_tempF);
}

void Print_Time()
{
    //Print the Time
    printf("%02x:%02x:%02x %02x/%02x/%02x\t", hour, minute, second, month, day, year);
}


