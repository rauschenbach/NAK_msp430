#ifndef _PORTS_H
#define _PORTS_H


#include <stdio.h>
#include "type.h"
#include "main.h"


#define OUTPUT 	1
#define INPUT 	0

//#define Tz_RELEASE_TIME       10    // 600 = 10мин// Tz задержка высплытия
/* Задержки задаются в милисекундах */

/* Задержки задаются в секундах  */


// Отрезок памяти ФРАМ для хранения различных значений
#define FRAM_TEST_START       0xFE00
#define FRAM_TEST_END         0xFE80

#define DEVICE_NUM_ADR        0xFE02
#define Tz_TIME_ADR           0xFE04
#define TIME_LIGHT_LOW_ADR    0xFE06
#define TIME_LIGHT_HIGH_ADR   0xFE08           
  
#define ALARM_MIN             0xFE0A
#define ALARM_HR              0xFE0C
#define ALARM_MONTH           0xFE0E  
#define ALARM_YEAR            0xFE10
 
#define FLAG_BURN_RELE_ACTIV  0xFE12

#define FILE_SIZE_LOW_ADR     0xFE14
#define FILE_SIZE_HIGH_ADR    0xFE16



#define MASK_EXIST_DIAGRAM_TO_AM3  0x02
#define MASK_SEND_DIAGRAM_TO_AM3   0x04 
#define MASK_AM3_IMPULSE_RECEIVED  0x08

#define FREQ_9kHz             (P1IN & BIT0)
#define FREQ_11_5kHz          (P1IN & BIT1)
#define FREQ_13kHz            (P1IN & BIT2)
#define FREQ_14kHz            (P3IN & BIT0)


/* Сделано функциями */
#define RELE_SET_ON()         do {  P3OUT |=  BIT1; } while(0)
#define RELE_RST_ON()         do {  P3OUT |=  BIT2; } while(0)
#define LAMP_ON()             do {  P3OUT |=  BIT3; } while(0)
#define LAMP_OFF()            do {  P3OUT &= ~BIT3; } while(0)
#define LAMP_TOGGLE()         do {  P3OUT ^=  BIT3; } while(0)
#define RED_LED_ON()          do {  P1OUT |=  BIT5; } while(0)
#define RED_LED_OFF()         do {  P1OUT &= ~BIT5; } while(0)
#define RED_LED_TOGGLE()      do {  P1OUT ^=  BIT5; } while(0)

/* SHD - сигнал трансдьюсера    */
#define SHD_DISABLE()         do {  P2OUT |=  BIT7; } while(0)
#define SHD_ENABLE()          do {  P2OUT &= ~BIT7; } while(0)




/************************************************************************
 * Tx power = 1 value   VSEL1 
 * Мощность передатчика 1 - 00, 2 - 01, 3 - 10, 4 - 11. 
 * (BA, где B - это P3.6, а А - P3.5) 
 *************************************************************************/
#define TX_POWER_1()    	do { P3OUT &= ~BIT6; P3OUT &= ~BIT5; } while(0)
#define TX_POWER_2()   		do { P3OUT &= ~BIT6; P3OUT |=  BIT5; } while(0)
#define TX_POWER_3()    	do { P3OUT |=  BIT6; P3OUT &= ~BIT5; } while(0)
#define TX_POWER_4()    	do { P3OUT |=  BIT6; P3OUT |=  BIT5; } while(0)

/**************************************************************************
 * Чувствительность приемника 
 * 1 - 00, 2 - 01, 3 - 10, 4 - 11. 
 * (BA, где B - это PJ.3, а А - PJ.2)
 *************************************************************************/
#define RX_SENSE_1()    	do{ PJOUT &= ~BIT3; PJOUT &= ~BIT2; } while(0)
#define RX_SENSE_2()    	do{ PJOUT &= ~BIT3; PJOUT |=  BIT2; } while(0)
#define RX_SENSE_3()    	do{ PJOUT |=  BIT3; PJOUT &= ~BIT2; } while(0)
#define RX_SENSE_4()    	do{ PJOUT |=  BIT3; PJOUT |=  BIT2; } while(0)


// Джампера
#define JMP1         !(PJIN & BIT0) 
#define JMP2         !(PJIN & BIT1)
#define JMP3         !(PJIN & BIT2)
#define JMP4         !(PJIN & BIT3)


void port_io_init(void);
void port_irq_init(void);
void pin_high(int PORT, int PIN);
void port_high(int PORT, int MASK);

void pin_low(int PORT, int PIN);
void port_low(int PORT, int MASK);

void pin_dir(int PORT, int PIN, int state);
void port_dir(int PORT, int MASK);

void LED_test(void);
void tx_power_set(int);
void rx_sense_set(int);

#endif				/* ports.h */

