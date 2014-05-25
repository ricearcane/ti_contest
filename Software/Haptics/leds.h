#ifndef LEDS_H_INCLUDED
#define LEDS_H_INCLUDED

#endif // LEDS_H_INCLUDED
#include <catalina_cog.h>
#include "config.h"

void heartbeat();
void led0();
void led1();
void blink(int times, int pin);
void led1_state(int state);
