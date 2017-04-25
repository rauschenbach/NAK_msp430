/*************************************
*
*   TIMER  for  msp430FR5739
*
**************************************/
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "timer.h"

extern TFLAGS flags;
extern TCLOCK timers;


//-----------------------------   INIT TIMER A_1;  period =  ~100us; 
void timer_init(void)		// time(s) = numberOfCounts/ frequencyOfClock
{
    timers.ticks = 0;
    
    // TA1EX0   |= TAIDEX_7;                   // делитель SMCLK=8MHz на 8 = 1 MHz
    TA1CCTL0 = CCIE;		// TACCR0 interrupt enabled
    TA1CCR0 = 800;		// 800/8000000Hz = ~100us;  
    TA1CTL = TASSEL__SMCLK + MC__UP + TACLR;	// SMCLK = 8 MHz , UP mode, Clear 
}


//-----------------------------------------------------------------------------------------------
//------- 100us !!!   INTERRUPT TIMER A_1  
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    WDTCTL = WDT_MRST_32;
   
    /* Main NAK timer */
    timers.ticks++;		//< 4 294 967 294)


    /* Считаем время пинга */
    if(flags.run_flag) {
       flags.ping_time++;
     }
    
    
    if (timers.delay_toggle_led) {
	timers.delay_toggle_led--;

         // Погасить лампу 
        if(timers.delay_toggle_led == 0)
            RED_LED_OFF();
    }  

    if (timers.self_test)
	timers.self_test--;
   
    if(timers.ticks == 864000000) { // 1 day!
        timers.ticks = 0;
    }   
}

/* Получить ticks */
u32 timer_get_ticks(void)
{
  return timers.ticks; 
}


/* Получить миллисекунды */
u32 timer_get_msec(void)
{
  return timers.ticks / 10; // миллисекунды
}

/* Получить секунды */
u32 timer_get_sec(void)
{
  return timers.ticks / 10000; // секунды
}

/* Записать суточное время */
void timer_set_sec(u32 sec)
{
  if(sec < 86400)
    timers.ticks = sec * 10000;
}
