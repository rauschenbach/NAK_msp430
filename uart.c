/*************************************
*
*   UART  for  msp430FR5739
*
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "uart.h"
#include "utils.h"


#define RX_BUF_SIZE  64
#define TX_BUF_SIZE  64


extern TFLAGS flags;


/* Обмен с модемам - указатель */
static struct MODEM_XCHG_STRUCT {
    c8 rx_buf[RX_BUF_SIZE];
    c8 tx_buf[TX_BUF_SIZE];
    u8 rx_prv;
    u8 rx_beg;			/* Начало пакета */
    u8 rx_cnt;			/* Счетчик принятого */
    u8 rx_fin;			/* Конец приема */
    u8 rx_len;			/* Сколько принято */
    u8 tx_len;			/* Сколько передавать  */
    u8 tx_cnt;
    u8 tx_crc;
} xchg_buf;


/* Флаг приема параметра */
static struct {
    u8 cnt;
    u8 p1;
    u8 p2;
    u8 rsvd;
    u32 par;
} rx_pars;


/************************************************************************
 * INTERRUPT VECTOR UART 1 - Сейсмограф - прерывание по приему байта     
 ************************************************************************/
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    volatile u8 rx_byte;	// принятый байт
    c8 buf[RX_BUF_SIZE/4];

    switch (__even_in_range(UCA1IV, 0x08)) {
    case 0:
	break;			// Vector 0 - no interrupt
    case 2:			// Vector 2 - UCRXIFG
	rx_byte = UCA1RXBUF;

	/*  Первый байт */
	if (0x24 == rx_byte) {	/* Начинается всегда с $ */
	    xchg_buf.rx_buf[0] = 0x24;
	    xchg_buf.rx_beg = 1;
	    xchg_buf.tx_len = 0;
	    xchg_buf.rx_cnt = 1;
	    xchg_buf.rx_prv = 0;
	    xchg_buf.rx_fin = 0;
	    rx_pars.cnt = 0;
	} else if (xchg_buf.rx_beg == 1) {	// Если уже есть начало
	    xchg_buf.rx_buf[xchg_buf.rx_cnt] = rx_byte;

	    /* Если есть первая запятая - передается параметр  */
	    if (rx_byte == 0x2c && rx_pars.cnt == 0) {
		rx_pars.p1 = xchg_buf.rx_cnt + 1;
		rx_pars.cnt = 1;
	    } else if (rx_byte == 0x2c && rx_pars.cnt == 1) {
		rx_pars.p2 = xchg_buf.rx_cnt;
		rx_pars.cnt = 2;
		if (rx_pars.p2 - rx_pars.p1 > 0 && rx_pars.p2 - rx_pars.p1 < 16) {
		    memcpy(buf, &xchg_buf.rx_buf[rx_pars.p1], rx_pars.p2 - rx_pars.p1);
                    buf[rx_pars.p2 - rx_pars.p1] = 0;
		    rx_pars.par = atol(buf);	/* Получили параметр */                 
		}
	    }


	    /* Если перевод строки - считаем что набрали буфер */
	    if (rx_byte == 0x0A && xchg_buf.rx_prv == 0x0d) {
		xchg_buf.rx_buf[xchg_buf.rx_cnt + 1] = 0;
		xchg_buf.rx_beg = 0;
		xchg_buf.rx_fin = 1;	/* FIN стоит до тех пока не начали принимать с $ */

		/* Analyze data - смотрим что приняли */

		/* Послать команду ENABLE */
		if (strncmp(&xchg_buf.rx_buf[0], "$PSENA", 6) == 0) {
		    flags.tx_cmd = UART_ENABLE_CMD;
		    flags.am3_num = rx_pars.par;
                    xchg_buf.tx_len = 13;
		    set_crc8(xchg_buf.tx_buf, "$PAENA,,*00\r\n", xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSPNG", 6) == 0) {	// пинг
		    flags.tx_cmd = UART_PING_CMD;
		    flags.am3_num = rx_pars.par;
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PAPNG,,*00\r\n", xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSRST", 6) == 0) {	// ресет
		    flags.tx_cmd = UART_RESET_CMD;
		    flags.am3_num = rx_pars.par;
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PARST,,*00\r\n", xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSREL", 6) == 0) {	// релиз
		    flags.tx_cmd = UART_RELEASE_CMD;
		    flags.am3_num = rx_pars.par;
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PAREL,,*00\r\n", xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSTXP", 6) == 0) {	// мощность передачи
		    flags.tx_cmd = UART_TX_POWER_CMD;
		    flags.tx_power = rx_pars.par;
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PATXP,,*00\r\n", xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSRXS", 6) == 0) {	// чувствительность
		    flags.tx_cmd = UART_RX_SENSE_CMD;
		    flags.rx_sense = rx_pars.par;
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PARXS,,*00\r\n", xchg_buf.tx_len);    
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PGSTA", 6) == 0) {	// выдать статус
		    flags.tx_cmd = UART_GET_STATUS_CMD;
                    xchg_buf.tx_len = make_full_status(&xchg_buf.tx_buf[0]);
                    set_crc8(xchg_buf.tx_buf, xchg_buf.tx_buf, xchg_buf.tx_len);
		} else if(strncmp(&xchg_buf.rx_buf[0], "$PGTIM", 6) == 0) {	// выдать время в секундах
		    flags.tx_cmd = UART_GET_TIME_CMD;		
                    xchg_buf.tx_len = make_day_time(&xchg_buf.tx_buf[0]);
                    set_crc8(xchg_buf.tx_buf, xchg_buf.tx_buf, xchg_buf.tx_len);
		} else if (strncmp(&xchg_buf.rx_buf[0], "$PSTIM", 6) == 0) {	// Установить время
		    flags.tx_cmd = UART_SET_TIME_CMD;
		    timer_set_sec(rx_pars.par);
                    xchg_buf.tx_len = 13;                    
		    set_crc8(xchg_buf.tx_buf, "$PATIM,,*00\r\n", xchg_buf.tx_len);
                }

		if (xchg_buf.tx_len) {
		    xchg_buf.tx_cnt = 0;
                    xchg_buf.tx_crc = 0;

		    // On TX irq
		    UCA1IFG |= UCTXIFG;	// Interrupt when tx buf empty
		    UCA1IE |= UCTXIE;	// Receive Interrupts Enabled                 
		}
	    } else {
		xchg_buf.rx_cnt++;
		if(xchg_buf.rx_cnt >= RX_BUF_SIZE) {
		   xchg_buf.rx_cnt = 0;
		}
	    }
	}
	xchg_buf.rx_prv = rx_byte;	// last byte
	break;

    case 4:			// UCTXIFG
	if (xchg_buf.tx_len-- > 0) {
            volatile u8 tx_byte;
            tx_byte = xchg_buf.tx_buf[xchg_buf.tx_cnt++];
	    UCA1TXBUF = tx_byte;
	} else {
	    UCA1IFG &= ~UCTXIFG;// Interrupt when tx buf empty off
	    UCA1IE &= ~UCTXIE;	// Tx Interrupts disabled                           
	}

	break;			// Vector 4 - TXIFG
    default:
	break;
    }
}


//-------------------------------------------------------------------------
//  УАРТ связи с ПК  - 115200 baudrate
//-------------------------------------------------------------------------
void Init_NAK_UART1(void)
{				// UCA0IE   |= UCRXIE;              // Enable USCI_A0 RX interrupt
    //**** UART1 - SEISMOGRAPH ***//
    P2SEL1 |= (BIT5 + BIT6);
    P2SEL0 &= ~(BIT5 + BIT6);

    // Configure UART 1
    UCA1CTL1 |= UCSWRST;	// software reset enable
    UCA1CTL1 = UCSSEL__SMCLK;	// Set SMCLK as UCLk  (UART - 8 MHz)

    UCA1BR0 = 4;
    UCA1BR1 = 0;
    //  UCA1MCTLW |= UCSx     | UCFx     | UCOS16;   //UCBRS = 0x11, UCBRF = 8//
    UCA1MCTLW |= 0x5551;	//(0x2 << 8) | (4 << 4) | UCOS16;   //0x241;

    UCA1CTL1 &= ~UCSWRST;	// Initialize USCI state machine  

    UCA1IFG |= UCRXIFG;		// Interrupt when buf received a char
    UCA1IE |= UCRXIE;		// Receive Interrupts Enabled
}


void UART_clear_buf(void)
{
    memset(&xchg_buf, 0, sizeof(xchg_buf));
}


/* Bytes in rx buf */
u8 UART_get_buf_cnt(void)
{
    u8 ret = 0;
    if (xchg_buf.rx_fin)
	ret = xchg_buf.rx_cnt;
    return ret;
}

char *UART_get_ptr(void)
{
    return (char *) xchg_buf.rx_buf;
}

//-----------------------------------------------------------------------------------------------
//----------------------------- Переопределение вывода Printf - UART
int putchar(int ch)
{
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = ch;
    return ch;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------   UART TRANSMIT
void UART_Transmit(unsigned char *ch, unsigned char len)
{
    while (len--) {
	while (!(UCA1IFG & UCTXIFG));
	UCA1TXBUF = *ch++;
    }
}
