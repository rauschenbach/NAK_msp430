#ifndef _PWM_H_
#define _PWM_H_


#include "type.h"


typedef enum  {PING_9KHZ, PING_11_5KHZ, PING_13KHZ, PING_OFF} PING_FREQ;


void  PWM_init(void);
void  PWM_test(PING_FREQ);
void  PWM_start(PING_FREQ);
void  PWM_stop(void);
void  PWM_send(u8);

#endif

