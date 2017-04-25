#include "sysclk.h"
#include "string.h"
#include "utils.h"
#include "proto.h"
#include "main.h"

/* Structs */
extern TCLOCK timers;
extern TFLAGS flags;


/****************************************************************
 *   INIT PEREPHERY
 ***************************************************************/
void Init_Perephery(void)
{
    sysclk_init();
    port_io_init();
    timer_init();
    Init_NAK_UART1();
    __enable_interrupt();
}


//-----------------------------------------------------------------------------------------------
//----------------------------- ***** MAIN *****   
int main(void)
{

    // Stop watchdog timer 
    WDTCTL = WDTPW + WDTHOLD;

    /* Initialize perephery */
    Init_Perephery();

    flags.am3_num = 0;
    flags.tx_cmd = 0;

    flags.tx_power = 0;
    flags.rx_sense = 0;

    flags.rx_imp_num = 0;
    flags.tx_imp_num = 0;

    flags.tx_imp_time = 0;


    /* START WATCHDOG TIMER  32 msek interval */
    WDTCTL = WDT_MRST_32;

    SHD_ENABLE();


    /* тест светодиода */
    RED_LED_ON();
    timers.self_test = 20000;
    while (timers.self_test) {
    };
    RED_LED_OFF();

        

    /* Мощность на минимум */
    tx_power_set(0);

    /*** ***/
    while (1) {
	exec_uart_cmd();
    }
}
