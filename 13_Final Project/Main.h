
#define _XTAL_FREQ      8000000                 // The frequency

#define ACK             1                       // Definition for ACK (true)
#define NAK             0                       // Definition for NAK (flase)

#define ACCESS_CFG      0xAC                    // Impartant code definitions
#define START_CONV      0xEE
#define READ_TEMP       0xAA                    
#define CONT_CONV       0x02                    

#define TFT_DC          PORTDbits.RD0           // Define location of port for TFT_DC
#define TFT_CS          PORTDbits.RD1           // Define location of port for TFT_CS
#define TFT_RST         PORTDbits.RD2           // Define location of port for TFT_RST

#define enter_setup     PORTEbits.RE0           // Define location of port for enter_setup switch
#define setup_sel0      PORTEbits.RE1           // Define location of port for setup_sel0 switch
#define setup_sel1      PORTEbits.RE2           // Define location of port for setup_sel1 switch

#define SEC_LED         PORTCbits.RC4           // Define location of port for the 'sec' LED

#define FANEN_LED       PORTAbits.RA5           // Define location of port for the signal of fan enabled
#define RTC_ALARM_NOT   PORTAbits.RA4           // Define location of port for the alarm not



