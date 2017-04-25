#ifndef _MAIN_H_
#define _MAIN_H_

#include "type.h"
#include "uart.h"
#include "timer.h"
#include "ports.h"
#include "stdio.h"
#include "pwm.h"


#define T_START_LED_INTERV    5000
#define START_LED_COUNT       10



int                  main(void);
void                 init_Port_IO(void);
void                 send_Acoustic_Diagram(void);
void                 receive_AM3_Response(void);
unsigned char        get_AM3_impulse_cnt_in_buffer(void);
void                 clear_AM3_impulse_buffer(void);
unsigned char        get_Po_Vers(void);
unsigned char        get_Nak_Status(void);
void                 set_Nak_Status(unsigned char);
void                 clear_tx_impulse_cnt(void);
unsigned char        get_Cur_PC_Buff_Data_Cnt(void);


extern void          init_Timer_A(void);
extern void          putc(char c);

#endif
