#ifndef MDRIVER_H_INCLUDED
#define MDRIVER_H_INCLUDED


#endif // MDRIVER_H_INCLUDED

#include "TLC59711.h"
#include "sem.h"
#include "config.h"
#include "leds.h"

extern int m_d_ready;                    //writing a 1 here causes an update.
extern unsigned short channel_data[12];    //data for all driver channels (0-65535)
extern int m_sem;                     //semaphore for channel data. -1 for errors


typedef struct{
    unsigned int data[12];
} cdata;
/***
* Adds motor driver functionality, including a parallel version
* with message-passing functionality supporting threaded or
* mulicore implementations
***/

/***
* Begin execution of the motor driver loop in a new Cog
**/
void launch_m();
