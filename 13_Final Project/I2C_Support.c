#include <stdio.h>                                                  // Include the following libraries to make program work
#include "Main.h"
#include "I2C_Support.h"
#include <p18f4620.h>
#include "I2C_Soft.h"

extern unsigned char second, minute, hour, dow, day, month, year;   // Information for time and date
                                                                    // Setup time and date
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
                                                                    // alarm time and date
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
                                                                    // steup alarm time and date
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


void DS1621_Init()
{
char Device = 0x48;                                                 // Address of DS1621 device 
                                                                    // Code to start the device
	I2C_Write_Cmd_Write_Data(Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);         
}

int DS1621_Read_Temp()
{
char Data_Ret;                                                      // Data returned
char Device = 0x48;                                                 // Address for DS1621
char Cmd = READ_TEMP;                                               // Command to read temp
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address
    I2C_Write(Cmd);                                                 // Send register address
    I2C_ReStart();                                                  // Restart I2C
    I2C_Write((Device << 1) | 1);                                   // Initialize data read
    Data_Ret = I2C_Read(NAK);                                       // Read Data
    I2C_Stop();                                                     // Stop
    return Data_Ret;                                                // Return the Temperature Read
}

void DS3231_Read_Time()
{
char Device = 0x68;                                                 // Address for DS3231
char Address = 0x00;                                                // Register 0x00 pointing to the register 'second'
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address
    I2C_Write(Address);                                             // Send register address
    I2C_ReStart();                                                  // Restart I2C
    I2C_Write((Device << 1) | 1);                                   // Initialize data read  
    second = I2C_Read(ACK);                                         // Store the second
    minute = I2C_Read(ACK);                                         // Store the minute
    hour = I2C_Read(ACK);                                           // Store the hour
    dow = I2C_Read(ACK);                                            // Store the day of the week
    day = I2C_Read(ACK);                                            // Store the day
    month = I2C_Read(ACK);                                          // Store the month
    year = I2C_Read(NAK);                                           // Store the year and don't read more
    I2C_Stop();                                                     // Stop
}

void DS3231_Write_Time()
{
    char Device = 0x68;                                             // Device ID
    char Address = 0x00;                                            // Beginning Address 0
    second = dec_2_bcd(setup_second);                               // Save the setup_second into second, convert to BCD 
    minute = dec_2_bcd(setup_minute);                               // Save the setup_minute into minute, convert to BCD
    hour = dec_2_bcd(setup_hour);                                   // Save the setup_hour into hour, convert to BCD
    dow = 0x01;                                                     // Save dow as 0x01 
    day = dec_2_bcd(setup_day);                                     // Save the setup_day into day, convert to BCD
    month = dec_2_bcd(setup_month);                                 // Save the setup_month into month, convert to BCD
    year = dec_2_bcd(setup_year);                                   // Save the setup_year into year, convert to BCD
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address Write mode
    I2C_Write(Address);                                             // Send register address
    I2C_Write(second);                                              // Write seconds
    I2C_Write(minute);                                              // Write minutes
    I2C_Write(hour);                                                // Write hours
    I2C_Write(dow);                                                 // Write DOW
    I2C_Write(day);                                                 // Write day
    I2C_Write(month);                                               // Write month
    I2C_Write(year);                                                // Write year
    I2C_Stop();                                                     // End I2C protocol
}

void DS3231_Write_Initial_Alarm_Time()
{
    DS3231_Read_Time();                                             // Read current time
    alarm_date = day;                                               // Set alarm to today
    char Device = 0x68;                                             // Device ID given
    char Address = 0x07;                                            // Write to register 07
    alarm_hour = dec_2_bcd(0x01);                                   // Convert info to BCD
    alarm_minute = dec_2_bcd(0x01);
    alarm_second = dec_2_bcd(0x01);   
    alarm_second = alarm_second & 0x7f;                             // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                             // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                             // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                             // Set A1M4 as 1
    
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address Write mode
    I2C_Write(Address);                                             // Send register address 7

    I2C_Write(alarm_second);                                        // Write alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                        // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                          // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                          // Write alarm date value to DS3231    
    I2C_Stop();                                                     // End I2C protocol
}    

void DS3231_Read_Alarm_Time()
{
char Device = 0x68;                                                 // Device ID given
char Address = 0x07;                                                // Read from register 07
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address
    I2C_Write(Address);                                             // Send register address
    I2C_ReStart();                                                  // Restart I2C
    I2C_Write((Device << 1) | 1);                                   // Initialize data read
    alarm_second = I2C_Read(ACK);                                   // Read seconds from register 7
    alarm_minute = I2C_Read(ACK);                                   // Read minutes from register 8
    alarm_hour   = I2C_Read(ACK);                                   // Read hour from register 9
    alarm_date   = I2C_Read(NAK);                                   // Read hour from register A
    I2C_Stop();                                                     // End I2C protocol
}

void DS3231_Init()
{
char Device = 0x68;                                                 // Device ID Given
char Address_7 = 0x07;                                              // Read from register 07
char Address_E = 0x0E;                                              // Read from register 0E
char control_E;                                                     // value for control                                                             
    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // Getting the control_E values
    control_E = control_E & 0x01;                                   // Mask the Upper 7 bits off
    control_E = control_E | 0x25;                                   // OR the control value with 0x25
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // Write the control value in register E
    I2C_Start();                                                    // Start I2C protocol   
    I2C_Write((Device << 1) | 0);                                   // Device address
    I2C_Write(Address_7);                                           // Send register address
    I2C_ReStart();                                                  // Restart I2C
    I2C_Write((Device << 1) | 1);                                   // Initialize data read
    alarm_second = I2C_Read(ACK);                                   // Read seconds from register 7
    alarm_minute = I2C_Read(ACK);                                   // Read minutes from register 8
    alarm_hour   = I2C_Read(ACK);                                   // Read hour from register 9
    alarm_date   = I2C_Read(NAK);                                   // Read hour from register A
      
    alarm_second = alarm_second & 0x7f;                             // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                             // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                             // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                             // OR the bit 7
    
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address Write mode
    I2C_Write(Address_7);                                           // Send register address 7

    I2C_Write(alarm_second);                                        // Reset alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                        // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                          // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                          // Write alarm date value to DS3231    
    I2C_Stop();                                                     // Stop
}

void DS3231_Write_Alarm_Time()
{
    DS3231_Read_Time();                                             // Read current time
    alarm_date = day;                                               // Set alarm to today
    char Device = 0x68;                                             // Device ID given
    char Address = 0x07;                                            // Write to register 07
    alarm_hour = dec_2_bcd(setup_alarm_hour);                       // Convert info to BCD
    alarm_minute = dec_2_bcd(setup_alarm_minute);
    alarm_second = dec_2_bcd(setup_alarm_second);   
    alarm_second = alarm_second & 0x7f;                             // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                             // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                             // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                             // Set A1M4 as 1
    I2C_Start();                                                    // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                   // Device address Write mode
    I2C_Write(Address);                                             // Send register address 7

    I2C_Write(alarm_second);                                        // Write alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                        // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                          // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                          // Write alarm date value to DS3231    
    I2C_Stop();                                                     // End I2C protocol
}    

void DS3231_Turn_Off_Alarm()
{
char Device = 0x68;                                                 // Device ID Given
char Address_E = 0x0E;                                              // Register E address
char Address_F = 0x0F;                                              // Register F address
char control_E;                                                     // Control E stored here
char control_F;                                                     // Control F stored here 

    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // Get Control E value
    control_E = control_E & 0xFE;                                   // Mask off bit 0
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // Write the data in the address for E
    control_F = I2C_Write_Address_Read_One_Byte(Device, Address_F); // Get Control F value
    control_F = control_F & 0xFE;                                   // Mask off bit 0
    I2C_Write_Address_Write_One_Byte(Device, Address_F, control_F); // Write the data in the address for F
    DS3231_Init();                                                  // Initialize the DS32311
}

void DS3231_Turn_On_Alarm()
{
char Device = 0x68;                                                 // Device ID given
char Address_E = 0x0E;                                              // Register E address
char Address_F = 0x0F;                                              // Register F address
char control_E;                                                     // Control E value
char control_F;                                                     // Control F value

    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // Read the control_E value from the register
    control_E = control_E | 0x01;                                   // OR it with 0x01
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // Write the value into the Address_E
    control_F = I2C_Write_Address_Read_One_Byte(Device, Address_F); // Read the control_F value from register
    control_F = control_F & 0xFE;                                   // Mask off bit 0 from the control_F
    I2C_Write_Address_Write_One_Byte(Device, Address_F, control_F); // Write the value into Address_F
    DS3231_Init();                                                  // Initialize DS32331
    
}

int bcd_2_dec (char bcd)
{
int dec;                                                            // Decimal output storage
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);                          // Convert to BCD to DEC value
    return dec;                                                     // Return the decimal value
}

int dec_2_bcd (char dec)
{
int bcd;                                                            // BCD output storage
    bcd = ((dec / 10) << 4) + (dec % 10);                           // Convert DEC to BCD value
    return bcd;                                                     // return the BCD value
}
