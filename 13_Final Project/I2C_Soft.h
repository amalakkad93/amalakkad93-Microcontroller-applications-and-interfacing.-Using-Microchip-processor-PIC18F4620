void I2C_Wait(void);                                        // Prototype for function to wait
void I2C_Start(void);                                       // Prototype for function to start
void I2C_ReStart(void);                                     // Prototype for function to restart the I2C
void I2C_Stop(void);                                        // Prototype for function to stop reading 
char I2C_Read(char);                                        // Prototype for function to read from the chip
void I2C_Write(unsigned char);                              // Prototype for function to write information into the chip
void I2C_Write_Cmd_Only(char, char);                        // Prototype for function to write the cmd only
void I2C_Write_Cmd_Write_Data(char, char, char);            // Prototype for function to write using cmd, and 1 byte
void I2C_Write_Address_Write_One_Byte(char, char, char);    // Prototype for function to write address and write one byte
BYTE I2C_Write_Address_Read_One_Byte(char, char);           // Prototype for function to write the adress and read one byte
 
void i2cHighSda(void);                                      // Prototypes for data manipulation
void i2cLowSda(void);                             
void i2cHighScl(void);                                 
void i2cLowScl(void);                                  
void i2cNack(void);                                       
void i2cAck(void);                                         

void I2C_Init(unsigned long);                               // Prototype for function to initialize the I2C process