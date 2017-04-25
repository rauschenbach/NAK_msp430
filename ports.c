#include "ports.h"
#include "timer.h"
#include "utils.h"


extern TCLOCK timers;
extern TFLAGS flags;

static /*volatile */struct {
  u32 temp0;
  u32 temp1; 
  u32 temp2; 
} save_time;


#define         IMP_LEN_TIME        10 /* 1 �� */
#define         IMP_GAP_TIME        8500    


/*

I - ����
nI - ��������� ����
O - �����
nO - ��������� �����
P - ���� �����/������




	����	430	I/O		Init	��������
DET_9KHZ	PA1	P1.0	nI	P	I	������ � ���������� ��������� 9 ���. ��� ������ ������ �������� ������������ ������� � ������� ������ �������.
DET_11.5KHZ	PA2	P1.1	nI	P	I	������ � ���������� ��������� 11,5 ���. ��� ������ ������ �������� ������������ ������� � ������� ������ �������.
DET_13KHZ	PA3	P1.2	nI	P	I	������ � ���������� ��������� 13 ���. ��� ������ ������ �������� ������������ ������� � ������� ������ �������. � ������ ������ ������ �� ������������.
���� �� ������������, ������� � Z ���������
ADC_FOUT	PA0	P1.3	I	ADC	I	���������� ������ � ������ ���������� ������� ���������. ����� ��������������  ��� ������ ������������� ������ �������
���� �� ������������, ������� � Z ���������
ADC_VCAP	PB11	P1.4	I	ADC	I	���������� ������: ���������� ������� ����������� (12V). ��������� ������� ������� ������ �������������
���� �� ������������, ������� � Z ���������
LED	PB4	P1.5	O	P	L	���������� ������������ ����������� �� �������� ������ (������� ���������). ����� ������� ������� - ��������.
GPS_TX	PA9	P2.0	O	UART / P	L	����� �� ����������� (�������� ���� 500�� � GPS NMEA). ��� ����������� ������� ����������� ����������� �� ����!
GPS_RX	PA10	P2.1	I	UART		���� ��� �������� NMEA � ������������
HI	PA7	P2.3	O	P	L	���������� "������� ������" �����������. ��� ������������ ������ ������ ������� ������� ��������� � LO!!!�������� ��������� LO=H, HI=H, PKEY_ON=H!!! ��� ��������� ������� �� �����!!!
LO	PB0	P2.4	O	P	L	���������� ������ ������ �����������. ��� ������������ ������ ������ ������� ������� ��������� � HI. !!!�������� ��������� LO=H, HI=H, PKEY_ON=H!!! ��� ��������� ������� �� �����!!!
TXD_SWCLK	PA14	P2.5	O	UART		����� UART �� ����������� / PC
RXD	PA15	P2.6	I	UART		���� UART c ������������ / PC
SHD	PA6	P2.7	O	P	H	���������� �����������. H - ���������� �� ��������
STEPUP_ON	PB1	P3.1	O	P	L	����� ��������� ����������� ��������������� ��� ������� ����������� (����� �������� ��� �������� � �������� ������������ ����� (���� 7 ��� ������� "����", ��������) � ��������� ��� ������). ����� �������� ��� ������ � ���������.
� ������ ������ REL_SET
���� �� ������������ ������� � HI ���������.
PKEY_ON	PB2	P3.2	O	P	L	���� ����������� ������� 12V � �����������. ���� ���� ����� ���������, ����� ����� �������� �� �����. �� ���� �������� ��� ��������������� ����� ������� �������� �� ���������� � ����� �� ���������. ����� ���������� ����� LO � HI!!! �������� ��������� LO=H, HI=H, PKEY_ON=H!!! ��� ��������� ������� �� �����!!!
� ������ ������ REL_RST
���� �� ������������ ������� � HI ���������.
LAMP_ON	PB10 P1.0	P3.3	O	P	L	��������� �����. H - ��������
SYNC	PB7	P3.4	O	P	L	�� ������� ��� ���
VSEL1	PB5	P3.5	O	P	L	����� ���������� ������� ����������� ������� ��� (��� ���) ��� ����������� ������� ����������� ����������� ���� ������ �� ���� (GPS_TX)!
VSEL2	PB6	P3.6	O	P	L	����� ���������� ������� ����������� ������� ��� (��� ���)
BURN_ON	P12	P4.0	O	P	L	��������� ��������. H - �������
RADIO_ON	PB13	P4.1	O	P	L	��������� �����. H - ��������.
KSEL_A	PA4	PJ.2	O	P	L	����� ������������ �������� ��������� ������� ��� (��� ���)
KSEL_B	PA5	PJ.3	O	P	L	����� ������������ �������� ��������� ������� ��� (��� ���)
MOSI_SPI		P1.6	O	SPI		���� SPI RTC
MISO_SPI		P1.7	I	SPI		����� SPI RTC
		P3.7	nO	P	H	Chip select ��� RTC
		PJ.0	I	P	I	���� ���� ����� �������� � �����, �� ����� �����. �������� ��������. ���������, ���� ������� ��������� � �������, ������ ����� ������, ���� ��� - �����. (�.�. ������ ���� ��������)
���� �� ������������, ������� � Z ���������
		PJ.1	Z	P	I	�� ������������
		PJ.4	I	XTAL		���� ���������� ����������
		PJ.5	O	XTAL		����� ���������� ����������

*/
void port_io_init(void)
{
    // P1.0, P1.1, P1.2 - ����� ��������� �� 9, 11.5, 13 ���
    P1DIR &= ~(BIT0 + BIT1 + BIT2);	// ����      
    P1DIR &= ~(BIT3 + BIT4);

    // RED LED (ADC)
    P1DIR |= BIT5;
    RED_LED_OFF();

    // Transducer HI LO     
    // ����� � ����������� ������ PWM pins
    P2DIR |= (BIT3 + BIT4);	// P2.3 and P2.4 output = transducer
    P2OUT &= ~(BIT3 + BIT4);	// �����������! ����� ������ ���� ������ �� ������


    // SHD - ��������� ������������   
    P2DIR |= BIT7;		// �� �����
    SHD_DISABLE();		// P2OUT |= BIT7 


    // P3.0 ���� 14���, ��� ����� ���� � �������, ��� ������
    P3DIR &= ~(BIT0);

    // �������� ����������� - Tx power = 1 value   VSEL1
    P3DIR |= (BIT6 + BIT5);
    TX_POWER_2();

    // LAMP
    P3DIR |= BIT3;
    LAMP_OFF();

    // RELE  pins for SET & RST
    P3DIR |= (BIT1 + BIT2);
    RELE_SET_ON();
    RELE_RST_ON();

    // �� ������������ - Burn ON , Radio ON
    P4DIR |= (BIT0 + BIT1);
    P4OUT |= (BIT0 + BIT1);

    PJDIR &= ~BIT0;		// ������������ ������ ������ Z-state

    // ���������������� ��������� - Rx sense = 1 value  VSEL2
    PJDIR |= (BIT3 + BIT2);
    RX_SENSE_2();

    save_time.temp0 = 0;
    save_time.temp1 = 0;
    port_irq_init();
}


/******************************************************************************* 
 * INTERRUPT PORT 1  
 * ������� ���������� ����� 1. ���������, ���� ������ ������ ������ 1� 50�� 
 * ����� ����������, �� ��� ����� �������. ���� ������ �� 200�� ����� �� ����
 * (����� �� ����������� ������������� ����������) � ������ �� 1� 50�� ��� �� 
 * ����� ��� ������ ��� ����� �������, �� �� ����� ���������� ������������ �����
 * ��� ������ (0 ��� 1) � ����������� � ����� �������
********************************************************************************/
/*  Port1 init interrupts */
void port_irq_init(void)
{
  save_time.temp0 = 0;
  save_time.temp1 = 0;
  save_time.temp2 = 0;

  
    P1DIR &= ~BIT1;	// ����

    P1IES |= BIT1;	// ����� ������ hi to lo
   // P1IES &= ~BIT1;	// ����� ������ lo to hi

    P1IE |= BIT1;	// ��� ����������
    P1IFG &= ~BIT1;	// ����� ����� ����������
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

   /* ����� ������ ������ 8 - �� ���������, �.�. �����. */
    switch (P1IV) {

	    case P1IV_P1IFG1:	/*  11.5 kHz  */

              // ������� � hi � lo
              // ������ ��������
              if(!(P1IN & BIT1)) {
                 save_time.temp0 = timer_get_ticks(); // ����� ������ ��������
               
               // ����� ���������� �� ����� 850 ��  
               if(save_time.temp0 - save_time.temp2 > IMP_GAP_TIME) {
                    save_time.temp2 = save_time.temp0;
                    P1IES &= ~BIT1;
                    asm("nop");
                   }
              } else { // ������� lo � hi

		// ���� ����!
		flags.run_flag = 0;

                save_time.temp1 = timer_get_ticks();
                
                // ������� ����������� ����������������� > 7.5 �� (75 * 100 ���)
                if(save_time.temp1 - save_time.temp0 > IMP_LEN_TIME) {
                  flags.rx_imp_num++;                
                  if(flags.rx_imp_num > 8) {
                    flags.rx_imp_num = 0;   
                  }

                  // �������� ����� - � ������� ����� ���������
                  if (!timers.delay_toggle_led) {
                    timers.delay_toggle_led = 1000;
                    RED_LED_ON();
                   }
                }
                save_time.temp0 = 0;
                save_time.temp1 = 0;               
                P1IES |= BIT1;	// ����� ������ hi to lo
              }
		break;             

    	    default:
		break;
	    }

    P1IFG &= ~BIT1;	// !! ���������� ������������ ����� ����������- �����������
}


void LED_test(void)
{
    // ���� ����������
    RED_LED_ON();
    timers.self_test = 20000;
    while (timers.self_test) {
    };
}


/****************************************************************
 * �������� �����������
 ****************************************************************/
void tx_power_set(int p)
{
    switch (p) {
    case 0:
	TX_POWER_1();
	break;

    case 1:
	TX_POWER_2();
	break;

    case 2:
	TX_POWER_3();
	break;

    default:
	TX_POWER_4();
	break;
    }
}


/**************************************************************************
 * ���������������� ��������� 
 *************************************************************************/
void   rx_sense_set(int p)
{
    switch (p) {
    case 0:
	RX_SENSE_1();
	break;

    case 1:
	RX_SENSE_2();
	break;

    case 2:
	RX_SENSE_3();
	break;

    default:
	RX_SENSE_4();
	break;
    }
}

//-----------------------------------------------------------------------------------------------
//-----------------------------  *******  MSP430  ****** 
// -----------------------------------------------------------------------------
// ������� ������� �� ������
// -----------------------------------------------------------------------------
void pin_high(int PORT, int PIN)
{
    switch (PORT) {
    case 1:
	P1OUT |= 1 << PIN;
	break;
    case 2:
	P2OUT |= 1 << PIN;
	break;
    case 3:
	P3OUT |= 1 << PIN;
	break;
    case 4:
	P4OUT |= 1 << PIN;
	break;
    case 'J':
	PJOUT |= 1 << PIN;
	break;
    }

}

// -----------------------------------------------------------------------------
// ������� ������� �� ������ ����� ����� �� �����
// -----------------------------------------------------------------------------
void port_high(int PORT, int MASK)
{
    switch (PORT) {
    case 1:
	P1OUT |= MASK;
	break;
    case 2:
	P2OUT |= MASK;
	break;
    case 3:
	P3OUT |= MASK;
	break;
    case 4:
	P4OUT |= MASK;
	break;
    case 'J':
	PJOUT |= MASK;
	break;
    }
    //(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x18 + 0x20*PORT)) |= MASK;
}

// -----------------------------------------------------------------------------
// ������ ������� �� ������
// -----------------------------------------------------------------------------
void pin_low(int PORT, int PIN)
{
    switch (PORT) {
    case 1:
	P1OUT &= ~(1 << PIN);
	break;
    case 2:
	P2OUT &= ~(1 << PIN);
	break;
    case 3:
	P3OUT &= ~(1 << PIN);
	break;
    case 4:
	P4OUT &= ~(1 << PIN);
	break;
    case 'J':
	PJOUT &= ~(1 << PIN);
	break;
    }
    //(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x1C + 0x20*PORT)) |= 1<<PIN;
}

// -----------------------------------------------------------------------------
// ������ ������� �� ������ ����� ����� �� �����
// -----------------------------------------------------------------------------
void port_low(int PORT, int MASK)
{
    switch (PORT) {
    case 1:
	P1OUT &= ~MASK;
	break;
    case 2:
	P2OUT &= ~MASK;
	break;
    case 3:
	P3OUT &= ~MASK;
	break;
    case 4:
	P4OUT &= ~MASK;
	break;
    case 'J':
	PJOUT &= ~MASK;
	break;
    }
    //(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x1C + 0x20*PORT)) |=MASK;
}

// -----------------------------------------------------------------------------
// ��������� ����������� �����
// 0 - ����, 1 - �����
// -----------------------------------------------------------------------------
void pin_dir(int PORT, int PIN, int dir)
{
    if (dir) {
	switch (PORT) {
	case 1:
	    P1DIR |= 1 << PIN;
	    break;
	case 2:
	    P2DIR |= 1 << PIN;
	    break;
	case 3:
	    P3DIR |= 1 << PIN;
	    break;
	case 4:
	    P4DIR |= 1 << PIN;
	    break;
	case 'J':
	    PJDIR |= 1 << PIN;
	    break;
	}
	//(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x00 + 0x20*PORT)) |= 1<<PIN;
    } else {
	switch (PORT) {
	case 1:
	    P1DIR &= ~(1 << PIN);
	    break;
	case 2:
	    P2DIR &= ~(1 << PIN);
	    break;
	case 3:
	    P3DIR &= ~(1 << PIN);
	    break;
	case 4:
	    P4DIR &= ~(1 << PIN);
	    break;
	case 'J':
	    PJDIR &= ~(1 << PIN);
	    break;
	}
	//(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x00 + 0x20*PORT)) &= ~(1<<PIN);
    }
}

/***************************************************
	��������� ����������� ����� �� �����
	"1" � ����� - �����
	"0" - ����
****************************************************/

void port_dir(int PORT, int MASK)
{
    switch (PORT) {
    case 1:
	P1DIR |= MASK;
	break;
    case 2:
	P2DIR |= MASK;
	break;
    case 3:
	P3DIR |= MASK;
	break;
    case 4:
	P4DIR |= MASK;
	break;
    case 'J':
	PJDIR |= MASK;
	break;
    }
    //(*(volatile uint32_t *)(FIO_BASE_ADDR + 0x00 + 0x20*PORT)) |= MASK;
}
