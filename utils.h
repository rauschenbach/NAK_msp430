#ifndef _UTILS_H_
#define _UTILS_H_

#include "type.h"
#include "ports.h"



#define BUFF_AM3_DIAGRAM_SIZE      32
#define BUFF_PC_DIAGRAM_SIZE       17


enum { OFF, ON };
enum { DISABLE, ENABLE, RELEASE };


typedef volatile struct {
     u32 ticks;

     u16 delay_toggle_led;
     u16 cycle_delay_1sec;
     u16 red_led;
     u16 self_test;
} TCLOCK;


typedef volatile struct {
     u16 am3_num;	// Номер модема
     u16 tx_cmd;	// Код команды на передачу

     u8 tx_power;	// мощность излучателя
     u8 rx_sense;	// чувствительность приемника

     u8 rx_imp_num;	// номер импульса от акустики
     u8 tx_imp_num;	// номер передаваемого импульса

     u32 ping_time;
     u32 tx_imp_time;

     u8  run_flag;
} TFLAGS;

void set_crc8(char *, char *, int);
int make_full_status(char *);
int make_day_time(char *);


#endif
