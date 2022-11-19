#include <xc.h>                                             // Include the following libraries to make program work
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "TFT_ST7735.h"
#include "I2C_Support.h"
#include "Setup_Time.h"
#include "Main_Screen.h"
#include "Main.h"
                                                            // Store information for time and date
extern unsigned char second, minute, hour, dow, day, month, year;
                                                            // Store the setup information for time and date
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;

extern int INT0_flag, INT1_flag, INT2_flag;                 // Interrupt flags

extern char setup_time[];                                   // array of char for setting up time
extern char setup_date[];                                   // array of char for setting up date

extern char buffer[31];                                     // char buffer
extern char *nbr;                                           // char pointer of the nbr
extern char *txt;                                           // char pointer for txt
extern char tempC[];                                        // array of char for temp C onto LCD display
extern char tempF[];                                        // array of char for temp F onto LCD display
extern char time[];                                         // array of char for time onto LCD display
extern char date[];                                         // array of char for date onto LCD display

void Setup_Time(void)
{
    char Key_Next_Flag, Key_Up_Flag, Key_Dn_Flag;           // Flags to cause changes
    char Select_Field;                                      // Field Selected
    Select_Field = 0;                                       // Initial field selected
    DS3231_Read_Time();                                     // Read time
    setup_second = bcd_2_dec(second);                       // Set setup time as current time
    setup_minute = bcd_2_dec(minute);                       // Setting up minute
    setup_hour = bcd_2_dec(hour);                           // Setting up hour
    setup_day = bcd_2_dec(day);                             // Setting up day
    setup_month = bcd_2_dec(month);                         // Setting up month
    setup_year = bcd_2_dec(year);                           // Setting up year 
    Initialize_Setup_Time_Screen();                         // Initialize setup screen
    Update_Setup_Time_Screen();                             // Update screen with latest information
    
    while (enter_setup == 1)                                // Change values only if in enter_setup
    {   
        if (INT0_flag == 1)                                 // if software INT0 flag is set 
        { 
            INT0_flag = 0;                                  // clear the flag
            Key_Up_Flag = 1;                                // set increment flag
        }
        if (INT1_flag == 1)                                 // if software INT1 flag is set 
        { 
            INT1_flag = 0;                                  // clear the flag
            Key_Dn_Flag = 1;                                // set decrement flag
        }        
        if (INT2_flag == 1)                                 // if software INT2 flag is set 
        { 
            INT2_flag = 0;                                  // clear the flag
            Key_Next_Flag = 1;                              // set next flag
        }  

        if (Key_Up_Flag == 1)                               // If user increments
        {
            switch (Select_Field)                           // Increment the proper field
            {
                case 0:
                    setup_hour++;                           // Increment setup_hour
                    if (setup_hour == 24) setup_hour = 0;   // Go to 0 when hour reaches 24
                    break;

                case 1:
                    setup_minute++;                         // Increment setup_minute
                    if(setup_minute == 60) setup_minute = 0;// Reset minute to 0 when minute reaches 0
                    break; 

                case 2:
                    setup_second++;                         // Increment set_up second
                    if(setup_second == 60) setup_second = 0;// Reset second to 0 when second reaches 0
                    break;   

                case 3:
                    setup_month++;                          // Increment setup_month
                    if(setup_month == 13) setup_month = 1;  // Reset month to 1 when it reaches 12
                    break;   

                case 4:
                    setup_day++;                            // Increment setup_day
                    if (setup_day == 32) setup_day = 1;     // Reset day to 1 when it reaches 32
                    break;    

                case 5:
                    setup_year++;                           // Increment setup_year 
                    if (setup_year == 100) setup_year = 0;  // Reset year to 0 when it reaches 100
                    break;    

                default:
                    break;
            }    
            Update_Setup_Time_Screen();                     // Update screen with latest info
            Key_Up_Flag = 0;                                // Clear the increment flag
        }

        if (Key_Dn_Flag == 1 )                              // If user decrements
        {
            switch (Select_Field)                           // Decrement the proper field
            {
                case 0:
                    if(setup_hour == 0) setup_hour = 23;    // If hour is 0, loop to 23
                    else --setup_hour;                      // Decrement time otherwise
                    break;

                case 1:
                                                            // If minute is 0, loop back  to 59
                    if(setup_minute == 0) setup_minute = 59;
                    else --setup_minute;                    // Else decrement minute normally
                    break; 

                case 2:
                                                            // If second is 0, loop back to 59
                    if (setup_second == 0) setup_second = 59;
                    else --setup_second;                    // Else decrement second normally
                    break;   

                case 3:
                                                            // If month is 1, loop back to 12
                    if (setup_month == 1) setup_month = 12;
                    else --setup_month;                     // Else decrement normally
                    break;   

                case 4:
                    if (setup_day == 1) setup_day = 31;     // If day is 1, loop back to 31
                    else --setup_day;                       // Else decrement day normally
                    break;    

                case 5:
                    if(setup_year == 0) setup_year = 99;    // If year is 0, loop back to 99
                    else --setup_year;                      // Else decrement year normally
                    break;    

                default:
                break;
            }                
            Update_Setup_Time_Screen();                     // Update screen with latest info
            Key_Dn_Flag = 0;                                // Clear the decrement flag
        } 

        if (Key_Next_Flag == 1 )                            // If user switches
        {        
            Select_Field++;                                 // Move to next field
            if (Select_Field == 6) Select_Field = 0;        // Wrap around if necessary
            Update_Setup_Screen_Cursor(Select_Field);       // Update cursor
            Key_Next_Flag = 0;                              // Clear flag
        }    
    }         

    DS3231_Write_Time();                                    // Write time
    DS3231_Read_Time();                                     // Read time
    Initialize_Screen();                                    // Initialize the screen before returning     
}
    
void Initialize_Setup_Time_Screen(void) 
{ 
    fillScreen(ST7735_BLACK);                               // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L Fall20 Final\0");                 // Text displayed on LCD
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
                                                            // X and Y coordinates of where the text is to be displayed

    strcpy(txt, "Time Setup\0");                            // Text displayed 
    drawtext(start_x+3 , start_y+15, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 
       
    strcpy(txt, "Time");                                    // Display "Time" on LCD
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    
                                                            // Create a rectangle to show field selected
    fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
    strcpy(txt, "Date");                                    // Display "Date" on LCD
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
}

void Update_Setup_Time_Screen(void)
{
    setup_time[0]  = (setup_hour/10) + '0';                 // Setup hour MSB
    setup_time[1]  = (setup_hour%10) + '0';                 // Setup hour LSB 
    setup_time[3]  = (setup_minute/10) + '0';               // Setup minute MSB
    setup_time[4]  = (setup_minute%10) + '0';               // Setup minute LSB
    setup_time[6]  = (setup_second/10) + '0';               // Setup second MSB
    setup_time[7]  = (setup_second%10) + '0';               // Setup second LSB
    setup_date[0]  = (setup_month/10) + '0';                // Setup month MSB
    setup_date[1]  = (setup_month%10) + '0';                // Setup month LSB
    setup_date[3]  = (setup_day/10) + '0';                  // Setup day MSB
    setup_date[4]  = (setup_day%10) + '0';                  // Setup day LSB
    setup_date[6]  = (setup_year/10) + '0';                 // Setup year MSB
    setup_date[7]  = (setup_year%10) + '0';                 // Setup year LSB
                                                            // Display updated time and date on LCD
    drawtext(data_time_x, data_time_y, setup_time, ST7735_CYAN, ST7735_BLACK, TS_2);
    drawtext(data_date_x, data_date_y, setup_date, ST7735_GREEN, ST7735_BLACK, TS_2);
}

void Update_Setup_Screen_Cursor(char cursor_position)       // Function to display cursor on LCD for setup
{
    char xinc = 36;                                         // Scale for x-increment
    char yinc = 30;                                         // Scale for y-increment
    switch (cursor_position)
    {
        case 0:                                             // Fill in the rectangle black for case 1 and 5 (clear them out)
                                                            // Fill in the rectangle for case 0 with cyan, which is the new position
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:                                             // Fill in the rectangle black for case 2 and 0 (clear them out)
                                                            // Fill in the rectangle for case 1 with cyan, which is the new position
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:                                             // Fill in the rectangle black for case 1 and 3 (clear them out)
                                                            // Fill in the rectangle for case 2 with cyan, which is the new position
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:                                             // Fill in the rectangle black for case 2 and 4 (clear them out)
                                                            // Fill in the rectangle for case 3 with cyan, which is the new position
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:                                              // Fill in the rectangle black for case 3 and 5 (clear them out)
                                                            // Fill in the rectangle for case 4 with cyan, which is the new position
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:                                              // Fill in the rectangle black for case 4 and 0 (clear them out)
                                                            // Fill in the rectangle for case 5 with cyan, which is the new position
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;              
    }
}




