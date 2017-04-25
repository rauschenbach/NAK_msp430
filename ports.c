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


#define         IMP_LEN_TIME        10 /* 1 мс */
#define         IMP_GAP_TIME        8500    


/*

I - вход
nI - инверсный вход
O - выход
nO - инверсный выход
P - порт ввода/вывода




	Порт	430	I/O		Init	Описание
DET_9KHZ	PA1	P1.0	nI	P	I	Сигнал с частотного детектора 9 кГц. При низком уровне детектор зафиксировал наличие в сигнале данной частоты.
DET_11.5KHZ	PA2	P1.1	nI	P	I	Сигнал с частотного детектора 11,5 кГц. При низком уровне детектор зафиксировал наличие в сигнале данной частоты.
DET_13KHZ	PA3	P1.2	nI	P	I	Сигнал с частотного детектора 13 кГц. При низком уровне детектор зафиксировал наличие в сигнале данной частоты. В модеме данной версии не используется.
Если не используется, держать в Z состоянии
ADC_FOUT	PA0	P1.3	I	ADC	I	Аналоговый сигнал с выхода последнего каскада усилителя. Будет использоваться  для оценки интегрального уровня сигнала
Если не используется, держать в Z состоянии
ADC_VCAP	PB11	P1.4	I	ADC	I	Аналоговый сигнал: напряжение питания передатчика (12V). Позволяет оценить степень заряда конденсаторов
Если не используется, держать в Z состоянии
LED	PB4	P1.5	O	P	L	Управление светодиодным индикатором на передней панели (красный светодиод). Когда высокий уровень - светится.
GPS_TX	PA9	P2.0	O	UART / P	L	Выход на радиомодуль (передача тона 500Гц и GPS NMEA). При отключенном питании радиомодуля переключить на вход!
GPS_RX	PA10	P2.1	I	UART		Вход для принятия NMEA с регистратора
HI	PA7	P2.3	O	P	L	Управление "верхним плечом" передатчика. При акустическом приеме подать высокий уровень совместно с LO!!!Избегать состояния LO=H, HI=H, PKEY_ON=H!!! Это замыкание питания на землю!!!
LO	PB0	P2.4	O	P	L	Управление нижним плечом передатчика. При акустическом приеме подать высокий уровень совместно с HI. !!!Избегать состояния LO=H, HI=H, PKEY_ON=H!!! Это замыкание питания на землю!!!
TXD_SWCLK	PA14	P2.5	O	UART		выход UART на регистратор / PC
RXD	PA15	P2.6	I	UART		вход UART c регистратора / PC
SHD	PA6	P2.7	O	P	H	Блокировка передатчика. H - передатчик не передает
STEPUP_ON	PB1	P3.1	O	P	L	Выход включения повышающего преобразователя для питания передатчика (нужно включать при переходе в активный акустический режим (если 7 бит команды "свои", включить) и отключать при выходе). Также включать при ресете и включении.
В старом модеме REL_SET
Если не используется держать в HI состоянии.
PKEY_ON	PB2	P3.2	O	P	L	Ключ подключения питания 12V к передатчику. Этот ключ нужно выключать, когда модем работает на прием. То есть включаем его непосредственно перед выдачей импульса на передатчик и сразу же выключаем. Перед включением снять LO и HI!!! Избегать состояния LO=H, HI=H, PKEY_ON=H!!! Это замыкание питания на землю!!!
В старом модеме REL_RST
Если не используется держать в HI состоянии.
LAMP_ON	PB10 P1.0	P3.3	O	P	L	Включение лампы. H - включена
SYNC	PB7	P3.4	O	P	L	на будущее для НАК
VSEL1	PB5	P3.5	O	P	L	Выбор напряжения питания передатчика младший бит (для НАК) При отключенном питании радиомодуля переключить порт данных на вход (GPS_TX)!
VSEL2	PB6	P3.6	O	P	L	Выбор напряжения питания передатчика старший бит (для НАК)
BURN_ON	P12	P4.0	O	P	L	Включение пережига. H - пережиг
RADIO_ON	PB13	P4.1	O	P	L	Включение радио. H - включено.
KSEL_A	PA4	PJ.2	O	P	L	Выбор коэффициента усиления приемника младший бит (для НАК)
KSEL_B	PA5	PJ.3	O	P	L	Выбор коэффициента усиления приемника старший бит (для НАК)
MOSI_SPI		P1.6	O	SPI		Вход SPI RTC
MISO_SPI		P1.7	I	SPI		Выход SPI RTC
		P3.7	nO	P	H	Chip select для RTC
		PJ.0	I	P	I	Если этот выход притянут к земле, то модем новый. Включить подтяжку. Проверить, если удалось подтянуть к питанию, значит модем старый, если нет - новый. (т.е. вместо реле пережига)
Если не используется, держать в Z состоянии
		PJ.1	Z	P	I	не используется
		PJ.4	I	XTAL		вход кварцевого генератора
		PJ.5	O	XTAL		выход кварцевого генератора

*/
void port_io_init(void)
{
    // P1.0, P1.1, P1.2 - входы частотные на 9, 11.5, 13 кГц
    P1DIR &= ~(BIT0 + BIT1 + BIT2);	// вход      
    P1DIR &= ~(BIT3 + BIT4);

    // RED LED (ADC)
    P1DIR |= BIT5;
    RED_LED_OFF();

    // Transducer HI LO     
    // Выход в противофазе трансд PWM pins
    P2DIR |= (BIT3 + BIT4);	// P2.3 and P2.4 output = transducer
    P2OUT &= ~(BIT3 + BIT4);	// обязательно! иначе первый пинг ответа не делает


    // SHD - включение трансдьюсера   
    P2DIR |= BIT7;		// на выход
    SHD_DISABLE();		// P2OUT |= BIT7 


    // P3.0 вход 14кГц, вкл фронт прер с низкого, вкл прерыв
    P3DIR &= ~(BIT0);

    // Мощность передатчика - Tx power = 1 value   VSEL1
    P3DIR |= (BIT6 + BIT5);
    TX_POWER_2();

    // LAMP
    P3DIR |= BIT3;
    LAMP_OFF();

    // RELE  pins for SET & RST
    P3DIR |= (BIT1 + BIT2);
    RELE_SET_ON();
    RELE_RST_ON();

    // не используются - Burn ON , Radio ON
    P4DIR |= (BIT0 + BIT1);
    P4OUT |= (BIT0 + BIT1);

    PJDIR &= ~BIT0;		// опеределение версии модема Z-state

    // Чувствительность приемника - Rx sense = 1 value  VSEL2
    PJDIR |= (BIT3 + BIT2);
    RX_SENSE_2();

    save_time.temp0 = 0;
    save_time.temp1 = 0;
    port_irq_init();
}


/******************************************************************************* 
 * INTERRUPT PORT 1  
 * Функция прерывания Порта 1. Проверяет, если пришел сигнал поздее 1с 50мс 
 * после последнего, то это новая посылка. Если таймер на 200мс дошел до нуля
 * (чтобы не происходило многократного прерывания) и таймер на 1с 50мс еще не 
 * истек или первый шаг новой посылки, то по флагу прерывания определяется какой
 * бит пришел (0 или 1) и добавляется в общую посылку
********************************************************************************/
/*  Port1 init interrupts */
void port_irq_init(void)
{
  save_time.temp0 = 0;
  save_time.temp1 = 0;
  save_time.temp2 = 0;

  
    P1DIR &= ~BIT1;	// вход

    P1IES |= BIT1;	// фронт прерыв hi to lo
   // P1IES &= ~BIT1;	// фронт прерыв lo to hi

    P1IE |= BIT1;	// вкл прерывание
    P1IFG &= ~BIT1;	// сняли флаги прерывания
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

   /* Когда данных больше 8 - не разбираем, т.к. мусор. */
    switch (P1IV) {

	    case P1IV_P1IFG1:	/*  11.5 kHz  */

              // Переход с hi в lo
              // начало импульса
              if(!(P1IN & BIT1)) {
                 save_time.temp0 = timer_get_ticks(); // время начала импульса
               
               // Между импульсами не менее 850 мс  
               if(save_time.temp0 - save_time.temp2 > IMP_GAP_TIME) {
                    save_time.temp2 = save_time.temp0;
                    P1IES &= ~BIT1;
                    asm("nop");
                   }
              } else { // переход lo в hi

		// стоп пинг!
		flags.run_flag = 0;

                save_time.temp1 = timer_get_ticks();
                
                // Импульс достаточной продолжительности > 7.5 мс (75 * 100 мкс)
                if(save_time.temp1 - save_time.temp0 > IMP_LEN_TIME) {
                  flags.rx_imp_num++;                
                  if(flags.rx_imp_num > 8) {
                    flags.rx_imp_num = 0;   
                  }

                  // включить лампу - в таймере будет выключена
                  if (!timers.delay_toggle_led) {
                    timers.delay_toggle_led = 1000;
                    RED_LED_ON();
                   }
                }
                save_time.temp0 = 0;
                save_time.temp1 = 0;               
                P1IES |= BIT1;	// фронт прерыв hi to lo
              }
		break;             

    	    default:
		break;
	    }

    P1IFG &= ~BIT1;	// !! сбрасываем накопившиеся флаги прерываний- обязательно
}


void LED_test(void)
{
    // тест детекторов
    RED_LED_ON();
    timers.self_test = 20000;
    while (timers.self_test) {
    };
}


/****************************************************************
 * Мощность передатчика
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
 * Чувствительность приемника 
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
// высокий уровень на выходе
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
// высокий уровень на выходе всего порта по маске
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
// низкий уровень на выходе
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
// низкий уровень на выходе всего порта по маске
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
// установка направления порта
// 0 - вход, 1 - выход
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
	Установка направления порта по маске
	"1" в маске - выход
	"0" - вход
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
