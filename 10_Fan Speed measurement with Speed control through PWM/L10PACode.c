#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

unsigned int get_full_ADC(void);            //Routing to get conversion from ADC conversion
void Init_ADC(void);                        //Routing to Initialize ADC
void Init_IO();                             //Routing to INitialize IO

void Wait_One_Second();                     //Routing to wait 1 second
void Wait_Half_Second();                    //Routing to wait 1/2 second

void Initialize_Screen();                   //Routing to initialize screen

void interrupt high_priority chkisr();      //Routing to set int priority
void INT0_ISR();                            //Routing to set INT service routine
void INT1_ISR();
int  get_RPM();                             //Routing to calculate RPM
float read_volt();                          //Routing to calculate voltage from AN0
int get_duty_cycle();                       //Routing to calculate duty cycle
void Turn_Off_Fan();                        //Routing to turn off fan
void Turn_On_Fan();                         //Routing to turn on fan
void Show_Fan_On();                         //Routing to show status of fan
void Show_Fan_Off();
void Monitor_Fan();                         //Routine to show information of fan
void Update_Volt();                         //Routing to update the readings for voltage
void draw_bar_graph_dc(int);                //Routines to draw bar graphs
void draw_bar_graph_rpm(int);               
void Set_DC_RGB(int duty_cycle);            //Routines to change RGB colors
void Set_RPM_RGB(int rpm);      
void do_update_pwm(char duty_cycle);        //Routine to change PWM


#define _XTAL_FREQ      8000000             // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4        // Timer Clock 2 Mhz

#define SEC_LED  PORTEbits.RE0              // Defines SEC_LED as 
//#define FAN_EN   PORTCbits.RC0              // Defines FAN ENABLE LED
#define TFT_DC   PORTDbits.RD0              // Location of TFT D/C
#define TFT_CS   PORTDbits.RD1              // Location of TFT Chip Select
#define TFT_RST  PORTDbits.RD2              // Location of TFT Reset

#define DC_RED   PORTAbits.RA1              //Defines DC_RED
#define DC_GRN   PORTAbits.RA2              //Defines DC_GRN
#define RPM_GRN  PORTDbits.RD6              //Defines RPM_GRN
#define RPM_BLU  PORTDbits.RD7              //Defines RPM_BLU
#define FANEN_LED  PORTAbits.RA5            //Defines FANEN_LED

#define TS_1            1                   // Size of Normal Text
#define TS_2            2                   // Size of PED Text

#define Fan_Txt_Y       15                  //Size of Fan txt
#define Volt_Txt_Y      35                  //Size of Volt txt
#define DC_Txt_X        55                  //location of DC txt on x axis
#define DC_Txt_Y        67                  //location of DC txt width on y axis

#define DC_Rect_X       15                  //Defines position of DC_RECT on x axis
#define DC_Rect_Y       85                  //Defines position ofDC_RECT on y axis
#define DC_Rect_Height  20                  //Defines the height of the DC Rect
#define DC_Rect_Width   100                 //Defines the width of DC rect

#define RPM_Txt_X       40                  //location of RPM txt on x axis 
#define RPM_Txt_Y       122                 //location of RPM txt on y axis

#define RPM_Rect_X      15                  //Position of RPM rect on x axis
#define RPM_Rect_Y      140                 //Position of RPM rect on y axis
#define RPM_Rect_Height 20                  //RPM Rect Height
#define RPM_Rect_Width  100                 //RPM rect width 

#include "ST7735_TFT.c"
char buffer[31];                            // general buffer for display purpose
char *nbr;                                  // general pointer used for buffer
char *txt;
	
char Fan_Txt[]  = "OFF";                    // text storage for Fan Mode
char Volt_Txt[] = "0.00V";                  // text storage for Volt            
char DC_Txt[]  = "00";                      // text storage for Duty Cycle value
char RPM_Txt[] = "0000";                    // text storage for RPM

float volt;                                 //Store value of volt
int duty_cycle;                             //Store value of the duty cycle
int rpm;                                    //Store value of the RPM
int INT0_flag;                              //Store value of INT0 flag
int INT1_flag;                              //Store value of INt1_flag
char FANEN;                                 //Store value of FANEN
int bar;                                    //Store value of bar


void init_UART()                            //Initialize the UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}

void Init_Interrupts()                      //Initialize Interrupt pins
{
    INTCONbits.INT0IF = 0;                  //Clear INT0F
    INTCON3bits.INT1IF = 0;                 //Clear INT1F
    INTCON2bits.INTEDG0 = 1;                //INT0 Edge rising
    INTCON2bits.INTEDG1 = 1;                //INT1 Edge rising
    INTCONbits.INT0IE = 1;                  //Enable INT0
    INTCON3bits.INT1IE = 1;                 //Enable INT1
    INTCONbits.GIE = 1;                     //Set the Global Interrupt Enable
}

void interrupt high_priority chkisr()       //Set priority of the interrupts
{
    if (INTCONbits.INT0IF == 1) INT0_ISR(); //Go to INT0_ISR
    if (INTCON3bits.INT1IF == 1) INT1_ISR();//Go to INT0_ISR
}

void INT0_ISR()                             //INT0 service routine
{
    INTCONbits.INT0IF = 0;                  //Clear interrupt flag
    INT0_flag = 1;                          //Set software INT0 flag
}

void INT1_ISR()                             //INT1 service routine
{
    INTCON3bits.INT1IF = 0;                 //Clear interrupt flag
    INT1_flag = 1;                          //Set software INT1 flag
}

void main(void)
{
    Init_IO();                              //Configure I/O Ports
    Init_ADC();                             //Initialize ADC
    init_UART();                            //INitialize UART
    OSCCON = 0x70;                          // set the system clock to be 1MHz 1/4 of the 4MHz
    while(1)
    {
        rpm = get_RPM();                    //Find the rpm
        printf ("RPM = %d\r\n",rpm);        //Display rpm on teraterm
    }
    
}

void Turn_Off_Fan()                         //Turn the fan off
{
    Show_Fan_Off();                         //Display that the fan is off on LCD
    duty_cycle = 0;
    do_update_pwm(duty_cycle);              //based on the new duty cycle = 0
    Set_DC_RGB(duty_cycle);                 //Change the DC_RGB color to duty cycle = 0
    rpm = 0;                                //0 rotations due to duty cycle = 0
    Set_RPM_RGB(rpm);                       //Change the RPM_RGB color to RPM = 0
    FANEN = 0;                              //Set variable FANEN to 0
    FANEN_LED = 0;                          //Turn off FANEN_LED
}

void Turn_On_Fan()                          //Turn fan on
{
    Show_Fan_On();                          //Show the fan is off on display
    FANEN = 1;                              //Set variable FANEN to 1
    FANEN_LED = 1;                          //Turn on FANEN_LED
}

void Monitor_Fan()                          //Find and display information regarding the fan
{
    volt = read_volt();                     //Read voltage from AN0
    duty_cycle = get_duty_cycle();          //Get the duty cycle
    do_update_pwm(duty_cycle) ;             //Update information
    rpm = get_RPM();                        //Get the rpm
    Set_DC_RGB(duty_cycle);                 //Set the DC RGB to corresponding color
    Set_RPM_RGB(rpm);                       //Set the RPM RGB to corresponding color
    printf ("duty cycle = %d RPM = %d\r\n",duty_cycle, rpm);
    Wait_Half_Second();                     //Wait 0.5 Seconds
}

void Init_IO()                              //Initializes ports
{
    TRISA = 0x01;                           //PORTA configured as all outputs except bit 0
    TRISB = 0x03;                           //PORTB configured as all outputs except bit 0 and bit 1
    TRISC = 0x01;                           //PORTC configured as all outputs except bit 0
    TRISD = 0x00;                           //PORTD configured as all outputs
    TRISE = 0x00;                           //PORTE configured as all outputs
}

void Init_ADC()
{
    ADCON0 = 0x01;                          //Read from channel 0 (AN0)
    ADCON1 = 0x0E;                          //Make all inputs digital except AN0
    ADCON2 = 0xA9;                          //Right Justify, 12 TAD, Fosc/8
}

unsigned int get_full_ADC()             
{
unsigned int result;                        //Store the result from the ADC
   ADCON0bits.GO=1;                         // Start Conversion
   while(ADCON0bits.DONE==1);               // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;      // combine result of upper byte and
                                            // lower byte into result
   return result;                           // return the result.
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
    SEC_LED = ~SEC_LED;                     //Switch (ON or OFF) the SEC_LED
}

void Set_DC_RGB(int duty_cycle)             //Routine to change DC_RGB
{
    if (duty_cycle == 0)                            //Make the DC_RGB turn off
    {
        DC_RED = 0;
        DC_GRN = 0;
    }
    else if (duty_cycle > 0 && duty_cycle < 33)     //Make the DC_RGB RED
    {
        DC_RED = 1;
        DC_GRN = 0;
    }
    else if (duty_cycle >= 33 && duty_cycle < 66)   //Make the DC_RGB Yellow
    {
        DC_RED = 1;
        DC_GRN = 1;
    }
    else if (duty_cycle >= 67)                      //Make the DC_RGB Green
    {
        DC_RED = 0;
        DC_GRN = 1;
    }
}

void Set_RPM_RGB(int RPM)                   //Routine to change RPM_RGB
{
    if (RPM == 0)                                   //Make the RPM_RGB turn off
    {
        RPM_GRN = 0;
        RPM_BLU = 0;
    }
    else if (RPM > 0 && RPM < 1200)                 //Make the RPM_RGB Green
    {
        RPM_GRN = 1;
        RPM_BLU = 0;
    }
    else if (RPM >= 1200 && RPM < 2400)             //Make the RPM_RGB Cyan
    {
        RPM_GRN = 1;
        RPM_BLU = 1;
    }
    else if (RPM >= 2400)                           //Make the RPM_RGB Blue
    {
        RPM_GRN = 0;
        RPM_BLU = 1;
    }
}

void do_update_pwm(char duty_cycle)         //Update the pulse width modulation
{
 float dc_f;
 int dc_I;
    PR2 = 0b00000100 ;                      // set the frequency for 25 Khz
    T2CON = 0b00000111 ;                    //
    dc_f = ( 4.0 * duty_cycle / 20.0) ;     // calculate factor of duty cycle versus a 25 Khz
                                            // signal
    dc_I = (int) dc_f;                      // get the integer part
    if (dc_I > duty_cycle) dc_I++;          // round up function
        CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 
} 

int get_RPM()                               //Routine to calculate the RPM
{
    TMR1L = 0x00;                           // clear the count
    T1CON = 0x03;                           // start Timer1 as counter of number of pulses
    Wait_Half_Second();                     // wait half second
 int RPS = TMR1L;                           // read the count. Since there are 2 pulses per rev
                                            // and 2 ½ sec per sec, then RPS = count
    return (RPS * 60);                      // return RPM = 60 * RPS 
}

float read_volt()                           //Find the voltage coming in the circuit
{
 float volt;                                //Where the voltage is stored
 int nStep = get_full_ADC();                //Count the steps form AN0 for voltage
    volt = nStep * 5 /1024.0;               //Calculate the voltage
    return (volt);                          //Return the voltage read and calculated
}


int get_duty_cycle()                        //Routine to calculate the duty cycle
{	
int dc;                                     //Where the calculation is stored before returning it                 
    dc = (int) (read_volt() / 5.0 * 100.0); //Calculation of duty cycle
    return (dc); 
}

void Show_Fan_On()                          //Change the txt on LCD to say ON
{
    Fan_Txt[1] = 'N';                       //Change F to N
    Fan_Txt[2] = ' ';                       //Change F to ''
    drawtext(75, Fan_Txt_Y, Fan_Txt, ST7735_GREEN, ST7735_BLACK, TS_2); //Display changes on LCD
}

void Show_Fan_Off()                         //Change the txt on LCD to say OFF
{
    Fan_Txt[1] = 'F';                       //Change N to F
    Fan_Txt[2] = 'F';                       //Change ' ' to F
    drawtext(75, Fan_Txt_Y, Fan_Txt, ST7735_RED, ST7735_BLACK, TS_2);   
                                            //Display changes on LCD
}

void Update_Volt()                          //Update the voltage readings
{
    volt = read_volt();                         //Get the voltage from AN0
    Volt_Txt[0]= ((int) volt) + '0';            //Get the ones place of voltage
    Volt_Txt[2]= (((int)(volt*10))%10) + '0';   //Get the tenths place for voltage 
    Volt_Txt[3]= (((int)(volt*100))%10) + '0';  //Get the hundredths place for voltage
    drawtext(70, Volt_Txt_Y, Volt_Txt, ST7735_WHITE, ST7735_BLACK, TS_2); 
                                                //Display voltage on LCD
}

void Initialize_Screen()
{
    LCD_Reset();                            //Reset LCD
    TFT_GreenTab_Initialize();              //Initialize GreenTab
    fillScreen(ST7735_BLACK);               //Make entire screen black

    txt = buffer;                           // TOP HEADER FIELD 
    strcpy(txt, "ECE3301L Fall'20 L10");    //Text to display as header of LCD
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "FAN:");                    //Text to create label for Fan
    drawtext(15, Fan_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_2);         //Display label on LCD
    drawtext(75, Fan_Txt_Y, Fan_Txt, ST7735_RED, ST7735_BLACK, TS_2);       //Display fan status
    
    strcpy(txt, "VOLT:");                   //Text to create label for volt
    drawtext(2, Volt_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_2);         //Display volt label on LCD
    drawtext(70, Volt_Txt_Y, Volt_Txt, ST7735_WHITE, ST7735_BLACK, TS_2);   //Display voltage readings

    strcpy(txt, "Duty Cycle");              //Text to create label for Duty Cycle
    drawtext(35, 55, txt, ST7735_BLUE, ST7735_BLACK, TS_1);                 //Display DC label oon LCD
    drawtext(DC_Txt_X, DC_Txt_Y, DC_Txt, ST7735_RED, ST7735_BLACK, TS_2);   //Display DC value on LCD
    drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_RED);//Draw a rectangle to display DC percentage

    strcpy(txt, "RPM");                     //Text to create label for RPM
    drawtext(54, 110, txt, ST7735_BLUE, ST7735_BLACK, TS_1);                //Display RPM label on LCD
    drawtext(RPM_Txt_X, RPM_Txt_Y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_2);//Display RPM value on LCD
    drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_GREEN); //Draw rectangle for RPM percentage
}

void draw_bar_graph_dc(int dc)
{
    DC_Txt[0] = dc/10  + '0';                   //Calculate DC txt in tens place
    DC_Txt[1] = dc%10  + '0';                   //Calculate DC txt in ones place

    //Fill rectangle for dc to black
    fillRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_BLACK);

    if (dc == 0)                                                                    //Case for 0% DC
    {
       drawtext(DC_Txt_X, DC_Txt_Y, DC_Txt, ST7735_RED, ST7735_BLACK, TS_2);        //Make text red     
       drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_RED);    //Fill in the corresponding percentage of red
    }
    else if (dc < 33)                                                               //Case for <33%
    {
       drawtext(DC_Txt_X, DC_Txt_Y, DC_Txt, ST7735_RED, ST7735_BLACK, TS_2);        //Make text red    
       drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_RED);    //Make rectangle red    
       fillRect(DC_Rect_X, DC_Rect_Y, dc, DC_Rect_Height,ST7735_RED);               //Fill in corresponding percentage of red
    }
    else if (dc >=33 && dc < 67)                                                    //Case for 33%>DC>66%
    {
       drawtext(DC_Txt_X, DC_Txt_Y, DC_Txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     //Make text yellow      
       drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_YELLOW); //Draw rectangle for 33% red
       fillRect(DC_Rect_X, DC_Rect_Y, 33, DC_Rect_Height,ST7735_RED);               //Fill in the red portion of bar graph
       fillRect(DC_Rect_X+33, DC_Rect_Y, (dc-33), DC_Rect_Height,ST7735_YELLOW);    //Fill corresponding percentage yellow
       drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_YELLOW); //Draw rectangle for yellow 
    }
    else                                                                            //Case for DC>66%
    {
        drawtext(DC_Txt_X, DC_Txt_Y, DC_Txt, ST7735_GREEN, ST7735_BLACK, TS_2);     //Make text green
        drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_GREEN); //Make rectangle green
        fillRect(DC_Rect_X, DC_Rect_Y, 33, DC_Rect_Height,ST7735_RED);              //Fill 33% of graph red
        fillRect(DC_Rect_X+33, DC_Rect_Y, 33, DC_Rect_Height,ST7735_YELLOW);        //Fill another 33% yellow
        drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_YELLOW);//Draw the rectangle yellow
        fillRect(DC_Rect_X+66, DC_Rect_Y, (dc-66), DC_Rect_Height,ST7735_GREEN);    //Fill in corresponding percentage green
        drawRect(DC_Rect_X, DC_Rect_Y, DC_Rect_Width, DC_Rect_Height,ST7735_GREEN); //Make the rectangle green
    }        
}

void draw_bar_graph_rpm(int rpm)                //Routine for the RPM Bar Graph
{
    
    RPM_Txt[0] = rpm/1000  + '0';               //Calculate number in thousands place
    RPM_Txt[1] = (rpm/100)%10 + '0';            //Calculate number in hundreds place
    RPM_Txt[2] = (rpm/10)%10 + '0';             //Calculate number in tens place
    RPM_Txt[3] = (rpm)%10 + '0';                //Calculate number in ones place
    
    //Fill in RPM rectangle with black
    fillRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_BLACK) ;

    bar = (rpm/36);                             //Calculate the percentage for the RPM bar graph
    if (bar == 0)                                                                       //Case if bar = 0%
    {
       drawtext(RPM_Txt_X, RPM_Txt_Y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_2);       //Make text green     
       drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_GREEN);  //Make the rectangle green
    }
    else if (bar < 33)                                                                  //Case if bar < 33%
    {
       drawtext(RPM_Txt_X, RPM_Txt_Y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_2);       //Make text green          
       drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_GREEN);  //Make rectangle green        
       fillRect(RPM_Rect_X, RPM_Rect_Y, bar, RPM_Rect_Height,ST7735_GREEN) ;            //Fill corresponding percentage green
    }
    else if (bar >=33 && bar < 67)                                                      //Case if 33%>bar>66%
    {
       drawtext(RPM_Txt_X, RPM_Txt_Y, RPM_Txt, ST7735_CYAN, ST7735_BLACK, TS_2);        //Make text cyan  
       drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_CYAN);   //Draw rectangle cyan        
       fillRect(RPM_Rect_X, RPM_Rect_Y, 33, RPM_Rect_Height,ST7735_GREEN);              //Fill 33% of graph green
       fillRect(RPM_Rect_X+33, RPM_Rect_Y, (bar-33), RPM_Rect_Height,ST7735_CYAN);      //Fill corresponding percentage cyan
       drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_CYAN);   //Make the rectangle cyan
    }
    else                                                                                //Case if bar > 66%
    {
        drawtext(RPM_Txt_X, RPM_Txt_Y, RPM_Txt, ST7735_BLUE, ST7735_BLACK, TS_2);       //Make text blue
        drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_BLUE);  //Draw rectangle blue
        fillRect(RPM_Rect_X, RPM_Rect_Y, 33, RPM_Rect_Height,ST7735_GREEN);             //Fill 33% of graph green
        fillRect(RPM_Rect_X+33, RPM_Rect_Y, 33, RPM_Rect_Height,ST7735_CYAN);           //Fill 33% of graph cyan
        drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_CYAN);  //Draw rectangle cyan
        fillRect(RPM_Rect_X+66, RPM_Rect_Y, (bar-66), RPM_Rect_Height,ST7735_BLUE);     //Fill in corresponding percentage blue
        drawRect(RPM_Rect_X, RPM_Rect_Y, RPM_Rect_Width, RPM_Rect_Height,ST7735_BLUE);  //Make rectangle blue
    }     
}



