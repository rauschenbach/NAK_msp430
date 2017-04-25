#ifndef _TIMER_H_
#define _TIMER_H_

#include "ports.h"
#include "type.h"

void timer_init(void);
u32  timer_get_msec(void);
u32  timer_get_sec(void);
void timer_set_sec(u32);
u32 timer_get_ticks(void);

#endif