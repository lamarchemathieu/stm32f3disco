#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

void timer_init(void (*callback)(void *arg), void *arg);
uint16_t timer_get(void);

#endif