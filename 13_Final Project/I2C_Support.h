void DS1621_Init(void);                     // Prototype for function to initialize the DS1621 chip
void DS3231_Read_Time(void);                // Prototype for function to Read the time from the DS1621 chip
void DS3231_Write_Time(void);               // Prototype for function to Write the time into the DS3231 chip
void DS3231_Read_Alarm_Time(void);          // Prototype for function to Read the alarm time in the DS3231 chip
void DS3231_Write_Alarm_Time(void);         // Prototype for function to write the alarm time into the DS3231 chip
int DS3231_Read_Temp(void);                 // Prototype for function to read the temp on the DS3231 chip
int DS1621_Read_Temp(void);                 // Prototype for function to read the temp from the DS1621 chip
void DS3231_Turn_On_Alarm();                // Prototype for function to turn on the alarm in DS3231 chip
void DS3231_Turn_Off_Alarm();               // Prototype for function to turn off the alarm in DS3231 chip
void DS3231_Init();                         // Prototype for function to initialize the DS3231 chip
void DS3231_Write_Initial_Alarm_Time(void); // Prototype for function to write the initial alarm time if it was corrupted
int dec_2_bcd (char);                       // Prototype for function to convert decimal numbers to binary coded decimal
int bcd_2_dec (char);                       // Prototype for function to convert binary coded decimal to decimal