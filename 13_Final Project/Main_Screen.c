#include <xc.h>                                                             // Include the following libraries to make program work
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Main.h"
#include "Main_Screen.h"
#include "TFT_ST7735.h"

extern char buffer[31];                                                     // txt buffer
extern char *nbr;                                                           // nbr pointer
extern char *txt;                                                           // txt pointer
extern char tempC[];                                                        // array of char for temperature in C (used to display on LCD)
extern char tempF[];                                                        // array of char for temperature in F (used to display on LCD)
extern char time[];                                                         // array of char for time (used to display on LCD)
extern char date[];                                                         // array of char for date (used to display on LCD)
extern char alarm_time[];                                                   // array of char for alarm time (used to display on LCD)
extern char Alarm_SW_Txt[];                                                 // array of char for Alarm SW (used to display on LCD)
extern char Fan_Set_Temp_Txt[];                                             // array of char for Fan_Set_Temp (used to display on LCD)
extern char Fan_SW_Txt[];                                                   // array of char for Fan_SW (used to display on LCD)
extern char Volt_Txt[];                                                     // array of char for Voltage (used to display on LCD)
extern char DC_Txt[];                                                       // array of char for DC (used to display on LCD)
extern char RPM_Txt[];                                                      // array of char for RPM (used to display on LCD)
extern char RTC_ALARM_Txt[];                                                // array of char for RTC Alarm (used to display on LCD)
extern signed int DS1621_tempC, DS1621_tempF;                               // Store temperature readings
extern unsigned char second, minute, hour, dow, day, month, year;           // Store readings for time
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;    // Store readings of alarm time
extern unsigned char setup_fan_temp;                                        // Store readings of setup_fan_temp
extern int duty_cycle;                                                      // Store readings of duty cycle
extern int ALARMEN;                                                         // Store readings of ALARM enable
extern int FANEN;                                                           // Store readings of FAN enable
extern int rpm;                                                             // Store readings of RPM
extern int MATCHED;                                                         // Store MATCHED
extern float volt;                                                          // Store Readings of Volt
void Initialize_Screen(void)                                                // Function to initialize the screen
{ 
    LCD_Reset();                                                            // Screen reset
    TFT_GreenTab_Initialize();                                              // Initialize TFT GreenTab
    fillScreen(ST7735_BLACK);                                               // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L Fall20 Final\0");                                 // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   
                                                                            // X and Y coordinates of where the text is to be displayed
    strcpy(txt, "Temperature:");                                            // txt now holds "Temperature"
    drawtext(temp_x, temp_y, txt, ST7735_MAGENTA, ST7735_BLACK, TS_1);      // Write the txt label from the line above on the LCD                                                        // including text color and the background of it
    drawCircle(circ_x, circ_y, 2, ST7735_YELLOW);                           // Draw the circle symbol for degree C
    strcpy(txt, "C/");                                                      // txt now holds "C"
    drawtext(tempc_x, tempc_y, txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     // Write the txt label on the line above onto the LCD
    strcpy(txt, "F");                                                       // txt now holds "F"
    drawCircle(cirf_x, cirf_y , 2, ST7735_YELLOW);                          // Draw the circle symbol for degree F
    drawtext(tempf_x, tempf_y, txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     // Write the txt label from two lines above onto the LCD
    strcpy(txt, "Time");                                                    // txt now holds "Time"
    drawtext(time_x, time_y, txt, ST7735_BLUE, ST7735_BLACK, TS_1);         // Write the txt label from the line above onto the LCD
    strcpy(txt, "Date");                                                    // txt now holds "Date" 
    drawtext(date_x, date_y, txt, ST7735_RED, ST7735_BLACK, TS_1);          // Write the txt label from the line above onto the LCD
    strcpy(txt, "Alarm Time");                                              // txt now holds "Alarm Time" 
                                                                            // Write the txt label from the line above onto the LCD
    drawtext(alarm_time_x, alarm_time_y, txt, ST7735_YELLOW, ST7735_BLACK, TS_1); 
    strcpy(txt, "Alarm SW");                                                // txt now holds "Alarm SW"
                                                                            // Write the txt label from the line above onto the LCD
    drawtext(alarm_sw_x, alarm_sw_y, txt, ST7735_YELLOW, ST7735_BLACK, TS_1);  
    strcpy(txt, "FAN Set Temp");                                            // txt now holds "FAN Set Temp"
                                                                            // Write the txt label from the line above onto the LCD
    drawtext(fan_set_temp_x, fan_set_temp_y, txt, ST7735_BLUE, ST7735_BLACK, TS_1); 
    strcpy(txt, "Fan SW");                                                  // txt now holds "Fan SW"
    drawtext(fan_sw_x, fan_sw_y, txt, ST7735_BLUE, ST7735_BLACK, TS_1);     // Write the txt label from the line above onto the LCD
    strcpy(txt, "DC");                                                      // txt now holds "DC"
    drawtext(dc_x, dc_y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);            // Write the txt label from the line above onto the LCD
    strcpy(txt, "RM");                                                      // txt now holds "RM"
    drawtext(rtc_x, rtc_y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);          // Write the txt label from the line above onto the LCD
    strcpy(txt, "Volt");                                                    // txt now holds "Volt" 
    drawtext(volt_x, volt_y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);        // Write the txt label from the line above onto the LCD
    strcpy(txt, "RPM");                                                     // txt now holds "RPM" 
    drawtext(rpm_x, rpm_y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);          // Write the txt label from the line above onto the LCD
}

void Update_Screen(void)
{
    if ((DS1621_tempC & 0x80) == 0x80)                                      // Case if the tempC is negative
    {   
        tempC[0] = '-';                                                     // Make the sign to be negative for tempC
        tempC[1]  = (DS1621_tempC & 0x7f)/10  + '0';                        // Tens digit of C      
        tempC[2]  = (DS1621_tempC & 0x7f)%10  + '0';                        // Ones digit of C
    }
    else                                                                    // Case if the tempC is non-negative
    {
        tempC[0] = '+';                                                     // Make the sign to be positive for tempC
        tempC[1]  = DS1621_tempC/10  + '0';                                 // Tens digit of C
        tempC[2]  = DS1621_tempC%10  + '0';                                 // Ones digit of C
    }
    
    if (DS1621_tempF < 0)                                                   // Case if the tempF is negative
    {
        tempF[0] = '-';                                                     // Make the  sign to be negative for tempF
        int neg_tempF = -DS1621_tempF;                                      // Convert the number to negative
        tempF[1]  = neg_tempF/10  + '0';                                    // Tens digit of C
        tempF[2]  = neg_tempF%10  + '0';                                    // Ones digit of C
    }
    else                                                                    // Case if the tempF is non-negative
    {
        tempF[0] = '+';                                                     // Make the sign to be positive for tempF
        tempF[1]  = DS1621_tempF/10  + '0';                                 // Tens digit of C
        tempF[2]  = DS1621_tempF%10  + '0';                                 // Ones digit of C
    }    
    time[0]  = (hour>>4)  + '0';                                            // Hour MSD
    time[1]  = (hour&0x0F) + '0';                                           // Hour LSB
    time[3]  = (minute>>4) + '0';                                           // Minute MSB
    time[4]  = (minute&0x0F) + '0';                                         // Minute LSB
    time[6]  = (second>>4) + '0';                                           // Second MSB
    time[7]  = (second&0x0F) + '0';                                         // Second LSB
    date[0]  = (month>>4) + '0';                                            // Month MSB
    date[1]  = (month&0x0F) + '0';                                          // Month LSB
    date[3]  = (day>>4) + '0';                                              // Day MSB
    date[4]  = (day&0x0F) + '0';                                            // Day LSB
    date[6]  = (year>>4) + '0';                                             // Year MSB
    date[7]  = (year&0x0F) + '0';                                           // Year LSB
    alarm_time[0]  = (alarm_hour>>4)  + '0';                                // Alarm Hour MSB
    alarm_time[1]  = (alarm_hour&0x0F) + '0';                               // Alarm Hour LSB
    alarm_time[3]  = (alarm_minute>>4) + '0';                               // Alarm Minute MSB
    alarm_time[4]  = (alarm_minute&0x0F) + '0';                             // Alarm Minute LSB
    alarm_time[6]  = (alarm_second>>4) + '0';                               // Alarm Second MSB
    alarm_time[7]  = (alarm_second&0x0F) + '0';                             // Alarm Second LSB
    
    if (ALARMEN == 1)                                                       // If AlARM is Enabled
    {   
        strcpy(Alarm_SW_Txt, "ON ");                                        // Text saying "ON " for Alarm
    }
    else                                                                    // If ALARM is not Enabled
    {
        strcpy(Alarm_SW_Txt, "OFF");                                        // Text saying "OFF" for Alarm
    }
      
    Fan_Set_Temp_Txt[0] = setup_fan_temp/100 + '0';                         // Fan Temp Hundreds digit
    Fan_Set_Temp_Txt[1] = (setup_fan_temp%100)/10  + '0';                   // Fan Temp Tens digit
    Fan_Set_Temp_Txt[2] = setup_fan_temp%10  + '0';                         // Fan Temp Ones digit
    
    if (FANEN == 1)                                                         // If fan is enabled
    {
        strcpy(Fan_SW_Txt, "ON ");                                          // Text saying "ON " for Fan
    }
    else                                                                    // If fan is not enabled
    {       
        strcpy(Fan_SW_Txt, "OFF");                                          // Text saying "OFF" for Fan
    }
    
    if (RTC_ALARM_NOT == 0) RTC_ALARM_Txt[0] = '1';                         // Check the RTC_ALARM_NOT to update RM
    else RTC_ALARM_Txt[0] = '0';
       
    Volt_Txt[0]= ((int) volt) + '0';                                        // Get the ones place of voltage
    Volt_Txt[2]= (((int)(volt*10))%10) + '0';                               // Get the tenths place for voltage 
    Volt_Txt[3]= (((int)(volt*100))%10) + '0';                              // Get the hundredths place for voltage

    DC_Txt[0] = (duty_cycle/100) % 10 + '0';                                // Hundreds digit of DC
    DC_Txt[1] = ((duty_cycle/10)%10) + '0';                                 // Tens digit of DC
    DC_Txt[2] = (duty_cycle%10) + '0';                                      // Ones digit of DC
    RPM_Txt[0] = rpm/1000  + '0';                                           // Thousands digit of RPM
    RPM_Txt[1] = (rpm/100)%10 + '0';                                        // Hundreds digit of RPM
    RPM_Txt[2] = (rpm/10)%10 + '0';                                         // Tens digit of RPM
    RPM_Txt[3] = (rpm)%10 + '0';                                            // Ones digit of RPM    
    
    drawtext(data_tempc_x, data_tempc_y, tempC , ST7735_YELLOW , ST7735_BLACK , TS_2);                      // Write tempC on LCD in YELLOW
    drawtext(data_tempf_x, data_tempf_y, tempF , ST7735_YELLOW , ST7735_BLACK , TS_2);                      // Write tempF on LCD in YELLOW
    drawtext(data_time_x , data_time_y , time  , ST7735_CYAN   , ST7735_BLACK , TS_2);                      // Write time on LCD in CYAN
    drawtext(data_date_x , data_date_y , date  , ST7735_GREEN  , ST7735_BLACK , TS_2);                      // Write date on LCD in GREEN
    drawtext(data_alarm_time_x , data_alarm_time_y , alarm_time  , ST7735_CYAN   , ST7735_BLACK , TS_1);    // Write alarm time on LCD in CYAN
    drawtext(data_alarm_sw_x , data_alarm_sw_y , Alarm_SW_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_1);      // Write alarm sw on LCD in CYAN
    drawtext(data_fan_set_temp_x, data_fan_set_temp_y ,Fan_Set_Temp_Txt, ST7735_RED, ST7735_BLACK, TS_1);   // Write Fan_Set_Temp on LCD in RED
    drawtext(data_fan_sw_x, data_fan_sw_y, Fan_SW_Txt, ST7735_RED    , ST7735_BLACK , TS_1);                // Write Fan_SW on LCD in RED
    drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);                               // Write DC on LCD in GREEN
    drawtext(data_rtc_x, data_rtc_y, RTC_ALARM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);                      // Write RTC_Alarm on LCD in GREEN
    drawtext(data_volt_x, data_volt_y, Volt_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);                         // Write Volt on LCD in GREEN
    drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);                            // Write RPM on LCD in GREEN
}


