#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"


/* Все переменные опишу здесь */
TCLOCK timers;
TFLAGS flags;

/* Считаем что начинается с $  */
void set_crc8(char *dst, char *src, int num)
{
    int i;
    u8 crc = 0;

    for (i = 0; i < num; i++) {
	dst[i] = src[i];

	if (src[i] == '$') {
	    continue;
	} else if (src[i] != '$' && src[i] != '*') {
	    crc ^= src[i];
	} else {
	    // в конец вставить crc
	    sprintf(&dst[i], "*%02X\r\n", crc);
	    break;
	}
    }
}


/* Выдать статус. WITHOUD DIV!!! */
int make_full_status(char* buf)
{
     sprintf(buf,"$PASTA,%03d,%02d,%02d,%02d,%02d,%02d,%08lX,%08lX,*00\r\n",
	    flags.am3_num > 999 ? 0: flags.am3_num,	
            flags.tx_cmd > 99? 0 : flags.tx_cmd,
 	    flags.tx_power,
            flags.rx_sense,

            flags.rx_imp_num,
            flags.tx_imp_num,

            flags.ping_time, // передаем время пинга
            flags.tx_imp_time);

	
   return strlen(buf);
}


/* Выдать время */
int make_day_time(char* buf)
{
    sprintf(buf,"$PATIM,%08ld,*00\r\n",timer_get_sec());
    return strlen(buf);
}

