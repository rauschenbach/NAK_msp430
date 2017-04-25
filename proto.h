#ifndef _PROTO_H_
#define _PROTO_H_

#include "type.h"


#define BUFF_SIZE          32
#define CMD_SIZE           7


enum {SET, GET};
#define DATA_IN_COMM_PLACE    7  // местоположение данных в команде
#define DEVNUM_FIELD_SIZE     4  // XXXX
#define CLOCK_FIELD_SIZE      14 // DD.MM.YY,HH:MM
#define SUMTIME_FIELD_SIZE    11 // HH:MM-HH:MM
#define TZ_FIELD_SIZE         3  // MMM
#define FILESIZE_FIELD_SIZE   8  // XXXXXXXX




void exec_uart_cmd(void);
void send_ping_cmd(c8);
void send_enable_cmd(c8);
void send_release_cmd(c8);
void send_reset_cmd(c8);
void send_rx_sense_cmd(c8);
void send_tx_power_cmd(c8);



#endif
