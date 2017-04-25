#ifndef _UART_MSP_H_
#define _UART_MSP_H_

#include "type.h"
#include "ports.h"


#define 	NO_CMD			0
#define 	UART_ENABLE_CMD		1
#define 	UART_PING_CMD		2
#define 	UART_RESET_CMD		3
#define 	UART_RELEASE_CMD	4
#define 	UART_TX_POWER_CMD	5
#define 	UART_RX_SENSE_CMD	6
#define 	UART_GET_STATUS_CMD	7
#define 	UART_GET_TIME_CMD	8
#define 	UART_SET_TIME_CMD	9




void Init_NAK_UART1(void);
void UART_Transmit(unsigned char *ch, unsigned char len);


void Init_UART0(void);

void print_Uart_Buff(void);

void UART_clear_buf(void);
u8   UART_get_buf_cnt(void);
s8*  UART_get_ptr(void);

#endif

