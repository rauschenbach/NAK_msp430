/*************************************
*
*   PWM  for  msp430FR5739
*
**************************************/
#include "utils.h"
#include "pwm.h"

#define TEST_PIC         100
#define TEST_PAUSE	9900

extern TCLOCK timers;
extern TFLAGS flags;


void PWM_send(u8 data)
{
  int i;

  for(i = 0; i < 8; i++) { 
   flags.tx_imp_num = (i + 1 > 8)? 0 : i + 1;   
   PWM_test((PING_FREQ)((data >> (7 - i)) & 1));
  }
}

/********************************************************
 * ƒелает пинг на определенное врем€. пауза всегда 20000  
 ********************************************************/
void  PWM_test(PING_FREQ freq)
{
    timers.self_test = TEST_PIC;
    PWM_start(freq);
    while (timers.self_test);
      
    timers.self_test = TEST_PAUSE;
    PWM_start(PING_OFF);
    while (timers.self_test);
}


/******************************************************************************
 *   PWM - Ў»–ќ“Ќќ-»ћѕ”Ћ№—Ќјя ћќƒ”Ћя÷»я 
 ******************************************************************************/
void PWM_start(PING_FREQ freq)
{
    switch (freq) {

    case PING_9KHZ:		//  9 kHz  '0'
	TB0CCR0 = 887 / 2;	//110/2;   // 1mHz/9kHz = 111 - 1
	break;

    case PING_11_5KHZ:		// 11.5 kHz = '1'
	TB0CCR0 = 694 / 2;	//86/2;    // 1mHz/11,5 kHz = 86,9 - 1 
	break;

    case PING_13KHZ:		// 13 kHz  = ответные пинги 
	TB0CCR0 = 614 / 2;	//76/2;    // 1mHz/13kHz = 76,9 - 1  
	break;

    case PING_OFF:		// Stop PWM        
    default:      
	TB0CCTL0 &= ~CCIE;	// interrupts disable 
	TB0CTL &= ~0x30;	//STOP  4-5 биты сн€ть
	P2OUT &= ~(BIT3 + BIT4);	// "0"  to output
	return;
    }

    TB0CCTL0 = CCIE;		// TBCCR0 interrupt enabled   
    TB0CTL = TBSSEL_2 + MC__UP + TBCLR;	// SMCLK, up to CCR0 mode, clear TAR 
    
    //begin with 1 0
    P2OUT |= BIT3;		// Hi signal
    P2OUT &= ~BIT4;		// Lo signal
}

//-----------------------------------------------------------------------------------------------
//-----------------------------   INTERRUPT TIMER B_1  
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{
//    volatile int i;

//    SHD_DISABLE();
    P2OUT ^= (BIT3 + BIT4);	// шимуем выходы на трансдьюсер
//    for(i = 0; i < 14; i++) asm(" nop ");
//    SHD_ENABLE();
}


void PWM_stop(void)
{
    TB0CCTL0 &= ~CCIE;		// interrupts disable 
    TB0CTL &= ~0x30;		//STOP  4-5 биты сн€ть
    P2OUT &= ~(BIT3 + BIT4);	// "0"  to output
}
