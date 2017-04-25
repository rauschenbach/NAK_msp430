#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "proto.h"
#include "pwm.h"
#include "type.h"

#define NO_ERR             0
#define ERR_CRC            1
#define ERR_COMMAND        2
#define ERR_FORMAT         3
#define ERR_LENGTH         4
#define ERR_VAL_ABOVE_MAX  5
#define ERR_SRAM_DEFICIENT 6
#define ERR_NAK_BISY       7


enum { WRITE, READ };
enum { CHECK_LENGTH, DEVICE_NUM, REAL_TIME, ALARM_TIME, SUMMERTIME, TZ, RELEASE_FLAG, GPS, FILE_SIZE };

extern TFLAGS flags;
extern TCLOCK timers;

/* Send distance cmd  */
void send_ping_cmd(c8 p)
{
   /* Перед посылкой ставим старт */
    flags.ping_time = 0;
    flags.run_flag = 1;
    flags.tx_imp_time = timer_get_ticks();
    flags.tx_imp_num = 0;
    PWM_test(PING_9KHZ);
}

/* Send acoustic command to modem  */
void send_enable_cmd(c8 num)
{
    u8 cmd;
    cmd = num + 9;
    flags.tx_imp_num = 0;
    PWM_send(cmd & 0x7F);
}


/* Acoustic release */
void send_release_cmd(c8 num)
{
    u8 cmd = 0x80;
    cmd |= num + 1;
    flags.tx_imp_num = 0;
    PWM_send(cmd);
}

/* Acoustic reset */
void send_reset_cmd(c8 num)
{
    u8 cmd = 0x01;
    flags.tx_imp_num = 0;    
    PWM_send(cmd);
}

/* Мощность передатчика */
void send_tx_power_cmd(c8 p)
{
    u8 par;
    par = (p < 4) ? p : 3;
    tx_power_set(par);
    flags.tx_power = par;
}


/* Чувств. приемника */
void send_rx_sense_cmd(c8 s)
{
    u8 par;
    par = (s < 4) ? s : 3;
    rx_sense_set(par);
    flags.rx_sense = par;
}

/*  parse string  */
void exec_uart_cmd(void)
{
   switch (flags.tx_cmd) {
    case UART_ENABLE_CMD:
	send_enable_cmd(flags.am3_num);
        flags.tx_cmd = NO_CMD;        
	break;

    case UART_PING_CMD:
	send_ping_cmd(flags.am3_num);
        flags.tx_cmd = NO_CMD;
	break;

    case UART_RESET_CMD:
	send_reset_cmd(flags.am3_num);
        flags.tx_cmd = NO_CMD;        
	break;

    case UART_RELEASE_CMD:
	send_release_cmd(flags.am3_num);
        flags.tx_cmd = NO_CMD;                
	break;

    case UART_TX_POWER_CMD:
	send_tx_power_cmd(flags.tx_power);
        flags.tx_cmd = NO_CMD;                
	break;

    case UART_RX_SENSE_CMD:
	send_rx_sense_cmd(flags.rx_sense);
        flags.tx_cmd = NO_CMD;                
	break;

    default:
	break;
    }

}
