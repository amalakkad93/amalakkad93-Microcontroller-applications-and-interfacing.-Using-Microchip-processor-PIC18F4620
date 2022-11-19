void interrupt  high_priority chkisr(void); // Prototype for function to direct the ISR routines to respective interrupts triggered
void INT0_ISR(void);                        // Prototype for function to clear hardware INT0 and set INT0 software flag
void INT1_ISR(void);                        // Prototype for function to clear hardware INT1 and set INT1 software flag
void INT2_ISR(void);                        // Prototype for function to clear hardware INT2 and set Int2 software flag
void T0_ISR();                              // Prototype for function to recalculate Tach_cnt which measures the rps of the fan
void Init_Interrupt(void);                  // Prototype for function to initialize the interrupts