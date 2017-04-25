#include <stdlib.h>
#include "sysclk.h"
#include "ports.h"



/**
 * Здесь настройка генератора и PLL для проца MSP430 
 */
void sysclk_init(void)
{

    //Configure External Oscillator XT1 pins
    PJSEL0 |= BIT4 + BIT5;	//select XIN / XOUT functions

    /* CSCTL0_H = 0xA5;              // Unlock register - необходимо для записи настроек разблокировать CS регистры             
       CSCTL2   = SELA__VLOCLK + SELM__XT1CLK + SELS__XT1CLK;  
       CSCTL3   = DIVA_0 + DIVS__1 + DIVM__1; // set dividers  // ACLK =   ; SMCLK = 8MHz; MCLK = 8MHz/
       CSCTL4  |= XT1DRIVE_0 + XTS;           // внешний кварц 8 MHz, HF
       CSCTL4  &= ~XT1OFF;                    // XT1 turn ON
     */

    CSCTL0_H = 0xA5;		// Unlock register - необходимо для записи настроек разблокировать CS регистры             
    CSCTL1 = 0x07;		// default - Internal DCO - not used
    CSCTL2 = SELA__VLOCLK + SELM__XT1CLK + SELS__XT1CLK;
    CSCTL3 = DIVA_0 + DIVS__1 + DIVM__1;	// set dividers   ACLK =   ; SMCLK = 1MHz; MCLK = 8MHz/
    //Increased drive strength for XT1 LF mode. XT1 oscillator operating range
    //in HF mode is 8 MHz to 16 MHz
    CSCTL4 = XT1DRIVE_1 + XTS + XT2OFF;	// внешний кварц 8 MHz, HF
    CSCTL4 &= ~XT1OFF;		// XT1 turn ON


    volatile uint16_t cs_cnt = 30000;
    do {
	cs_cnt--;
	if (!cs_cnt)
	    break;
	// Clear XT1 fault flag
	CSCTL5 &= ~XT1OFFG;
	SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);	// Test oscillator fault flag  

    // Turn off temperature sensor( to conserve power). 
    REFCTL0 |= REFTCOFF;	//REFTCOFF - disable temperature sensor 
}
