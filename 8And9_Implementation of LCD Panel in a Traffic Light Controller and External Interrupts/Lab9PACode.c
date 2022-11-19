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

#define debug 1
//LEDS
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4        // Timer Clock 2 Mhz

#define TFT_DC      PORTDbits.RD0       // Location of TFT D/C
#define TFT_CS      PORTDbits.RD1       // Location of TFT Chip Select
#define TFT_RST     PORTDbits.RD2       // Location of TFT Reset

//colors
#define OFF         0                   //Defines OFF as decimal value 0
#define RED         1                   //Defines RED as decimal value 1
#define GREEN       2                   //Defines GREEN as decimal value 2
#define YELLOW      3                   // Defines YELLOW as decimal value 3

#define NS          0
#define NSLT        1
#define EW          2
#define EWLT        3

#define Circle_Size     7               // Size of Circle for Light
#define Circle_Offset   15              // Location of Circle
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of PED Text
#define Count_Offset    10              // Location of Count

#define XTXT            30              // X location of Title Text 
#define XRED            40              // X location of Red Circle
#define XYEL            60              // X location of Yellow Circle
#define XGRN            80              // X location of Green Circle
#define XCNT            100             // X location of Sec Count

#define NS              0               // Number definition of North/South
#define NSLT            1               // Number definition of North/South Left Turn
#define EW              2               // Number definition of East/West
#define EWLT            3               // Number definition of East/West Left Turn
 
#define Color_Off       0               // Number definition of Off Color
#define Color_Red       1               // Number definition of Red Color
#define Color_Green     2               // Number definition of Green Color
#define Color_Yellow    3               // Number definition of Yellow Color

#define NS_Txt_Y        20
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      50
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define EW_Txt_Y        80
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      110
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define PED_NS_Count_Y  30
#define PED_EW_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140

#define PED_Count_NS    8
#define PED_Count_EW    9

#include "ST7735_TFT.c"

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char NS_Count[]     = "00";             // text storage for NS Count
char NSLT_Count[]   = "00";             // text storage for NS Left Turn Count
char EW_Count[]     = "00";             // text storage for EW Count
char EWLT_Count[]   = "00";             // text storage for EW Left Turn Count

char PED_NS_Count[] = "00";             // text storage for NS Pedestrian Count
char PED_EW_Count[] = "00";             // text storage for EW Pedestrian Count

char SW_NSPED_Txt[] = "0";              // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";              // text storage for NS Left Turn Switch
char SW_EWPED_Txt[] = "0";              // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";              // text storage for EW Left Turn Switch
char SW_MODE_Txt[]  = "D";              // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";             // text storage for Actual Mode
char EmergencyS_Txt[] = "0";            // text storage for Emergency Status
char EmergencyR_Txt[] = "0";            // text storage for Emergency Request
char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count

char SW_NSPED;                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                           // RAM variable for NS Left Turn Switch
char SW_EWPED;                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                           // RAM variable for EW Left Turn Switch
char SW_MODE;                           // RAM variable for Mode Light Sensor
char EMERGENCY;
char EMERGENCY_REQUEST;
int MODE;
int INT0_flag = 0, INT1_flag = 0, INT2_flag = 0;
char direction;
float volt;

#define SEC_LED  PORTEbits.RE0          //Defines SEC_LED as PORTE bit RE2

#define NS_RED   PORTAbits.RA1          // Defines NS_RED as PORTB bits RA1
#define NS_GREEN PORTAbits.RA2          // Defines NS_GREEN as PORTA bit RA2

#define NSLT_RED PORTAbits.RA3          // Defines NS_LT RED as PORTC bit RA3
#define NSLT_GREEN PORTAbits.RA4        // Defines NS_LT GREEN as PORTC bit RA4

#define EW_RED   PORTDbits.RD4          // Defines EW_RED as PORTC bit RB4
#define EW_GREEN PORTDbits.RD5          // Defines EW_GREEN as PORTC bit RB5

#define EWLT_RED PORTDbits.RD6          // Defines EWLT_RED as PORTD bit RD6
#define EWLT_GREEN PORTDbits.RD7        // Defines EWLT_GREEN as PORTD bit RD7

#define NS_LT_SW PORTBbits.RB4          // Defines NS_LT as PORTB bit RB4 for left turn
#define NS_PED_SW PORTBbits.RB0         // Defines NS_PED as PORTB bit RB0 for Ped-switch

#define EW_PED_SW PORTBbits.RB5         // Defines EW_PED as PORTB bit RB5 for Ped-switch
#define EW_LT_SW PORTBbits.RB5          // Defines EW_LT as PORTB bit RB1 for left turn

#define MODE_LED PORTDbits.RD3          // Defines MODE_LED as PORTD bit RD3,to differentiate day/night mode
    
unsigned int get_full_ADC(void);
void Init_ADC(void);
void init_IO(void);


void Set_NS(char color);
void Set_NS_LT(char color);
void Set_EW(char color);
void Set_EW_LT(char color);

void PED_Control( char Direction, char Num_Sec);
void Day_Mode(void);
void Night_Mode(void);


void Wait_One_Second(void);
void Wait_Half_Second(void);
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep(void);

void update_color(char , char );
void update_PED_Count(char direction, char count);
void Initialize_Screen(void);
void update_misc(void);
void update_count(char, char);

void Do_Init();
void interrupt  high_priority chkisr() ;
void INT0_ISR();
void INT1_ISR();
void INT2_ISR();

void Initialize_Screen()                    //Set-up initial screen
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Fall 2020");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* EMERGENCY REQUEST FIELD */
  strcpy(txt, "ER:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, EmergencyR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* EMERGENCY STATUS FIELD */
  strcpy(txt, "ES:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, EmergencyS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 

  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
      
  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
      
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "NSP NSLT EWP EWLT MD");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void init_UART()
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

void main(void)
{
    Do_Init();                                  // Initialization 
    while(1)
    {
        if (INT0_flag == 1)                     // if software INT0 flag is set
        {
            printf("INT0 interrupt pin detected\r\n");
            INT0_flag = 0;                      // clear that software flag 
        }
        if (INT1_flag == 1)                     // if software INT1 flag is set
        {
            printf("INT1 interrupt pin detected\r\n");
            INT1_flag = 0;                      // clear that software flag 
        }
        if (INT2_flag == 1)                     // if software INT2 flag is set
        {
            printf("INT2 interrupt pin detected\r\n");
            INT2_flag = 0;                      // clear that software  
        }
    }
}

void init_IO()
{
    TRISA = 0x01;                               //make port A all outputs except bit 0
    TRISB = 0xF7;                               //make port B all inputs except bit 3 
    TRISC = 0x00;                               //make port C all outputs
    TRISD = 0x00;                               //make port D all outputs 
    TRISE = 0x00;                               //make port E all outputs
}

void Init_ADC()
{
    ADCON0 = 0x01;                              //Selecting ch0
    ADCON1= 0x0F;                               //All digital input except AN0
    ADCON2= 0xA9;                               //Right justify the result. Set the bit conversion
                                                //TAD and acquisition time
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                     // Start Conversion
   while(ADCON0bits.DONE==1);           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;  // combine result of upper byte and
                                        // lower byte into result
   return result;                       // return the result.
}

void Set_NS(char color)
{
    direction = NS;
    update_color(direction, color);
    
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;           // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;           // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;         // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;        // sets NS LED YELLOW
    }
}

void Set_NS_LT(char color)
{
    direction = NSLT;
    update_color(direction, color);
    
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;       // Turns off the NSLT LED
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;       // Sets NSLT LED RED
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;     // sets NSLT LED GREEN
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;    // sets NSLT LED YELLOW
    }
}

void Set_EW(char color)
{
    direction = EW;
    update_color(direction, color);
    
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;           // Turns off the NSLT LED
        case RED: EW_RED =1;EW_GREEN=0;break;           // Sets NSLT LED RED
        case GREEN: EW_RED =0;EW_GREEN=1;break;         // sets NSLT LED GREEN
        case YELLOW: EW_RED =1;EW_GREEN=1;break;        // sets NSLT LED YELLOW
    }
}

void Set_EW_LT(char color)
{
    direction = EWLT;
    update_color(direction, color);
    
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;       // Turns off the NSLT LED
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;       // Sets NSLT LED RED
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;     // sets NSLT LED GREEN
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;    // sets NSLT LED YELLOW
    }
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ; 
    T2CON = 0b00000101 ; 
    CCPR2L = 0b01001010 ; 
    CCP2CON = 0b00111100 ; 
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0; 
    PORTBbits.RB3 = 0;
}

void PED_Control( char direction, char Num_Sec)
{ 
    for(char i = Num_Sec-1;i>0; i--)
    {
        update_PED_Count(direction, i);
        Wait_One_Second_With_Beep();            // hold the number on 7-Segment for 1 second
    }
    
    update_PED_Count(direction, 0);             
    Wait_One_Second_With_Beep();                // leaves the 7-Segment off for 1 second
}

void Day_Mode()
{
    MODE = 1;                                   // turns on the MODE_LED
    MODE_LED = 1;
    Act_Mode_Txt[0] = 'D';
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    
    Set_NS_LT(RED);                              //Step 1: NS to Green, others to Red
    Set_EW(RED);
    Set_EW_LT(RED);
    Set_NS(GREEN);
    if (SW_NSPED==1)                            //Step 1a: Pedestrian present
        PED_Control(NS,9);                       //PED_Control for NS with 9 seconds
    Wait_N_Seconds(8);                          //Step 2: NS still Green and wait 3 seconds
    Set_NS(YELLOW);                             //Step 3: NS to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_NS(RED);                                //Step 4: NS to Red
    
    if(SW_EWLT==1)                              //Step 5: Check EWLT
    {
        Set_EW_LT(GREEN);                       //Step 6: EWLT to Green and wait 7 seconds
        Wait_N_Seconds(7);
        Set_EW_LT(YELLOW);                      //Step 7: EWLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_EW_LT(RED);                         //Step 8: EWLT to Red
    }
    
    Set_EW(GREEN);                              //Step 9: Turn EW to Green
    if(SW_EWPED == 1)                           //Step 9a: Pedestrian present
        PED_Control(EW,8);                       //PED_Control for EW with 8 seconds
    Wait_N_Seconds(7);                          //Step 10: EW stays Green, wait 7 seconds
    Set_EW(YELLOW);                             //Step 11: EW to Yellow, wait 3 seconds
    Wait_N_Seconds(3);                              
    Set_EW(RED);                                //Step 12: EW to Red
    
    if (SW_NSLT == 1)                           //Step 13: Check NSLT switch
    {
        Set_NS_LT(GREEN);                       //Step 14: NSLT to Green and wait 8 seconds
        Wait_N_Seconds(8);
        Set_NS_LT(YELLOW);                      //Step 15: NSLT to Yellow and wait 3
        Wait_N_Seconds(3);
        Set_NS_LT(RED);
    } 
}
void Night_Mode()
{ 
    MODE = 0;                                   // turns on the MODE_LED
    MODE_LED = 0;
    Act_Mode_Txt[0] = 'N';
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
    Deactivate_Buzzer();
    Set_NS_LT(RED);                              //Step 1: NS to Green, others to Red
    Set_EW(RED);
    Set_EW_LT(RED);
    Set_NS(GREEN);
    Wait_N_Seconds(7);                          //Step 2: Wait 7 Seconds
    Set_NS(YELLOW);                             //Step 3: NS to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_NS(RED);                                //Step 4: NS to Red
    
    if(SW_EWLT==1)                              //Step 5: Check EWLT
    {
        Set_EW_LT(GREEN);                       //Step 6: EWLT to Green and wait 7 seconds
        Wait_N_Seconds(7);                           
        Set_EW_LT(YELLOW);                      //Step 7: EWLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_EW_LT(RED);                         //Step 8: EWLT to Red
    }
    
    Set_EW(GREEN);                              //Step 9: EW to Green and wait 6 seconds
    Wait_N_Seconds(6);
    Set_EW(YELLOW);                             //Step 10: EW to Yellow and wait 3 seconds
    Wait_N_Seconds(3);
    Set_EW(RED);                                //Step 11: EW to Red
    
    if(SW_NSLT==1)                              //Step 12: Check NSLT
    {
        Set_NS_LT(GREEN);                       //Step 13: NSLT to Green and wait 8 seconds
        Wait_N_Seconds(8);           
        Set_NS_LT(YELLOW);                      //Step 14: NSLT to Yellow and wait 3 seconds
        Wait_N_Seconds(3);
        Set_NS_LT(RED);                         //Step 15: NSLT to Red
    }

}

void Old_Wait_One_Second()                      //creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    update_misc();
}

void Wait_One_Second_With_Beep()				// creates one second delay as well as sound buzzer
{
    SEC_LED = 1;                                // First, turn on the SEC LED
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Activate_Buzzer();                          // Activate the buzzer
    Wait_Half_Second();                         // Wait for half second (or 500 msec) 
    
    SEC_LED = 0;                                // then turn off the SEC LED 
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Deactivate_Buzzer();                        // Deactivate the buzzer 
    Wait_Half_Second();                         // Wait for half second (or 500 msec) 
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                               // set the lower byte of TMR
    TMR0H = 0x0B;                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                       // turn off the Timer 0
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        Wait_One_Second();                      // calls Wait_One_Second for x number of times
        update_count(direction, I);        
    }
    Wait_One_Second();                          // calls Wait_One_Second for x number of times
    update_count(direction, 0);   
}

void update_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = NS_Cir_Y + direction * 30;    
    
    if (color == Color_Off)     //if Color off make all circles black but leave outline
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)     //if the color is red only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    if (color == Color_Green)     //if the color is green only fill the red circle with green
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
    
    if (color == Color_Yellow)    //if the color is yellow only fill the red circle with yellow
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
}

void update_count(char direction, char count)
{
   switch (direction)                       
   {
       case NS:       
            NS_Count[0] = count/10  + '0';
            NS_Count[1] = count%10  + '0';
            drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2); 
            break;
      
       case NSLT:
           NSLT_Count[0] = count/10 + '0';
           NSLT_Count[1] = count%10 + '0';
           drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
           break;
           
       case EW:
           EW_Count[0] = count/10 + '0';
           EW_Count[1] = count%10 + '0';
           drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);
           break;
           
       case EWLT:
           EWLT_Count[0] = count/10 + '0';
           EWLT_Count[1] = count%10 + '0';
           drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);
           break;
    }  
}

void update_PED_Count(char direction, char count)
{

   switch (direction)
   {
      case NS:       
        PED_NS_Count[0] = count/10  + '0';          // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);         //Put counter digit on screen
        break;
      
      case EW:       
        PED_EW_Count[0] = count/10  + '0';          // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);         //Put counter digit on screen
        break;  
   }
   
}

void update_misc()                                  //Update information
{
    ADCON1 = 0x0E;
    int nStep = get_full_ADC();        
    volt = nStep * 5 /1024.0;       
    SW_MODE = volt < 2.5 ? 1:0;   
    ADCON1 = 0x0F;
    
    SW_NSPED = NS_PED_SW;
    SW_NSLT = NS_LT_SW;
    SW_EWPED = EW_PED_SW;
    SW_EWLT = EW_LT_SW;
      
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';               // Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';
   
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(70,10, EmergencyR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
    drawtext(100,10, EmergencyS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);     
    drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(32,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(87,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
            
}

void Do_Init()
{
    init_UART();                                // Initialize the uart 
    Init_ADC();                                 // Initiailize the ADC with the  
                                                // programming of ADCON1 
    OSCCON=0x70;                                // Set oscillator to 8 MHz 
    TRISB = 0xF7;                               // Configure the PORTB for the External  
                                                // pins to make sure that all the INTx are 
                                                // inputs 
    INTCONbits.INT0IF = 0;                      // Clear INT0IF  
    INTCON3bits.INT1IF = 0;                     // Clear INT1IF  
    INTCON3bits.INT2IF =0;                      // Clear INT2IF  
    INTCON2bits.INTEDG0=0 ;                     // INT0 EDGE falling 
    INTCON2bits.INTEDG1=0;                      // INT1 EDGE falling 
    INTCON2bits.INTEDG2=1;                      // INT2 EDGE rising 
    INTCONbits.INT0IE =1;                       // Set INT0 IE 
    INTCON3bits.INT1IE=1;                       // Set INT1 IE  
    INTCON3bits.INT2IE=1;                       // Set INT2 IE  
    INTCONbits.GIE=1;                           // Set the Global Interrupt Enable 
}

void interrupt  high_priority chkisr()
{
    if (INTCONbits.INT0IF == 1) INT0_ISR(); // check if INT0 
                                            // has occurred 
    if (INTCON3bits.INT1IF == 1) INT1_ISR(); 
    if (INTCON3bits.INT2IF == 1) INT2_ISR();
}

void INT0_ISR()
{
    INTCONbits.INT0IF=0;                    // Clear the interrupt flag 
    INT0_flag = 1;                          // set software INT0_flag
}

void INT1_ISR()
{
    INTCON3bits.INT1IF=0;                   // Clear the interrupt flag
    INT1_flag = 1;                          // set software INT1_flag
}

void INT2_ISR()
{
    INTCON3bits.INT2IF=0;                   // Clear the interrupt flag
    INT2_flag = 1;                          // set software INT2_flag 
}

