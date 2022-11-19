#include <xc.h>                                                         // Include the following libraries to make program work
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "I2C_Support.h"
#include "Setup_Alarm_Time.h"
#include "Main_Screen.h"
#include "Main.h"
#include "Setup_Time.h"
#include "TFT_ST7735.h"

extern char setup_alarm_time[];                                         // array of char for the alarm time
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;// alarm time information
                                                                        // setup alarm information
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
extern int INT0_flag, INT1_flag, INT2_flag;                             // Interrupt flags
extern char *txt;                                                       // txt pointer of char

void Setup_Alarm_Time(void)
{
    char Key_Next_Flag, Key_Up_Flag, Key_Dn_Flag;                       // Store Key Flag information
    char Select_Field;                                                  // Store the field selected
    Select_Field = 0;                                                   // Field being selected
    DS3231_Read_Alarm_Time();                                           // Read alarm time
    alarm_second = bcd_2_dec(alarm_second);                             // convert to dec for use
    alarm_minute = bcd_2_dec(alarm_minute);                             // Minute to dec
    alarm_hour   = bcd_2_dec(alarm_hour);                               // Hour to dec
    setup_alarm_second = alarm_second;                                  // Beginning setup alarm time as current set alarm time
    setup_alarm_minute = alarm_minute;                                  // Setup time for minute
    setup_alarm_hour = alarm_hour;                                      // Setup time for hour
    Initialize_Setup_Alarm_Time_Screen();                               // Initialize the alarm setup screen
    Update_Setup_Alarm_Time_Screen();                                   // Update the alarm screen with latest info

    while (enter_setup == 1)                                            // Only make changes when enter_setup = 1
    {   
        if (INT0_flag == 1)                                             // if software INT0 flag is set 
        { 
            INT0_flag = 0;                                              // clear the flag
            Key_Up_Flag = 1;                                            // set increment flag
        }
        if (INT1_flag == 1)                                             // if software INT1 flag is set 
        { 
            INT1_flag = 0;                                              // clear the flag
            Key_Dn_Flag = 1;                                            // set decrement flag
        }        
        if (INT2_flag == 1)                                             // if software INT2 flag is set 
        { 
            INT2_flag = 0;                                              // clear the flag
            Key_Next_Flag = 1;                                          // set next flag
        }  

        if (Key_Up_Flag == 1)                                           // If user increments
        {
            switch (Select_Field)                                       // Increment the proper field
            {
                case 0:
                    setup_alarm_hour++;                                 // Increment setup_hour
                    if (setup_alarm_hour == 24) setup_alarm_hour = 0;   // Go to 0 when hour reaches 24
                    break;

                case 1:
                    setup_alarm_minute++;                               // Increment setup_minute
                    if(setup_alarm_minute == 60) setup_alarm_minute = 0;// Reset minute to 0 when minute reaches 0
                    break; 

                case 2:
                    setup_alarm_second++;                               // Increment set_up second
                    if(setup_alarm_second == 60) setup_alarm_second = 0;// Reset second to 0 when second reaches 0
                    break;      
                default:
                    break;
            }    
            Update_Setup_Alarm_Time_Screen();                           // Update screen with latest info
            Key_Up_Flag = 0;                                            // Clear the increment flag
        }

        if (Key_Dn_Flag == 1 )                                          // If user decrements
        {
            switch (Select_Field)                                       // Decrement the proper field
            {
                case 0:
                                                                        // If hour is 0, loop to 23
                    if(setup_alarm_hour == 0) setup_alarm_hour = 23;
                    else --setup_alarm_hour;                            // Decrement time otherwise
                    break;

                case 1:
                                                                        // If minute is 0, loop back  to 59
                    if(setup_alarm_minute == 0) setup_alarm_minute = 59;
                    else --setup_alarm_minute;                          // Else decrement minute normally
                    break; 

                case 2:
                                                                        // If second is 0, loop back to 59
                    if (setup_alarm_second == 0) setup_alarm_second = 59;
                    else --setup_alarm_second;                          // Else decrement second normally
                    break;   
                default:
                    break;
            }                
            Update_Setup_Alarm_Time_Screen();                           // Update screen with latest info
            Key_Dn_Flag = 0;                                            // Clear the decrement flag
        } 

        if (Key_Next_Flag == 1 )                                        // If user switches
        {        
            Select_Field++;                                             // Move to next field
            if (Select_Field == 3) Select_Field = 0;                    // Wrap around if necessary
            Update_Setup_Screen_Cursor(Select_Field);                   // Update cursor
            Key_Next_Flag = 0;                                          // Clear flag
        }    
    }         
    DS3231_Write_Alarm_Time();                                          // Write alarm time
    DS3231_Read_Alarm_Time();                                           // Read alarm time
    DS3231_Read_Time();                                                 // Read current time
    Initialize_Screen();                                                // Initialize main screen before returning
}
     
void Initialize_Setup_Alarm_Time_Screen(void)                           // Function to initialize setup alarm time fields
{ 
    fillScreen(ST7735_BLACK);                                           // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L Fall20 Final\0");                             // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE, ST7735_BLACK, TS_1); // X and Y coordinates of where the text is to be displayed

    strcpy(txt, "Alrm Setup");                                          // Text displayed 
    drawtext(start_x+5, start_y+20, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 
    
    strcpy(txt, "Time");                                                // Text to be displayed
    drawtext(time_x, time_y, txt, ST7735_BLUE, ST7735_BLACK, TS_1);
    fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
}

void Update_Setup_Alarm_Time_Screen(void)                               // Function to update Setup Alarm Time on LCD
{
    setup_alarm_time[0]  = (setup_alarm_hour/10)  + '0';                // Alarm time hour MSB
    setup_alarm_time[1]  = (setup_alarm_hour%10)  + '0';                // Alarm time hour LSB
    setup_alarm_time[3]  = (setup_alarm_minute/10)  + '0';              // Alarm time minute MSB
    setup_alarm_time[4]  = (setup_alarm_minute%10)  + '0';              // Alarm time minute LSB
    setup_alarm_time[6]  = (setup_alarm_second/10)  + '0';              // Alarm time second MSB
    setup_alarm_time[7]  = (setup_alarm_second%10)  + '0';              // Alarm time second LSB
                                                                        // Display alarm time onto the LCD
    drawtext(data_time_x, data_time_y, setup_alarm_time, ST7735_CYAN, ST7735_BLACK, TS_2);
}
 
