#ifndef TDRIVER_H_INCLUDED
#define TDRIVER_H_INCLUDED



#endif // TDRIVER_H_INCLUDED

#include "config.h"
#include "leds.h"

extern unsigned short t_chan_data[12];
extern int t_rx_sem;
extern int t_data_ready;


/***
* Wrapper for stdio's printf(). This will automatically direct output to the
* active serial port and supports all of printf's format options.
**/
void t_aprintf(const char *_format, ...);

/***
* Launch the terminal driver code to run in the next available cog.
* @return - the ID of the cog code was launched into or -1 if no free cogs exist
**/
int launch_t();
