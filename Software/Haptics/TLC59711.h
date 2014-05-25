#ifndef TLC59711_H_INCLUDED
#define TLC59711_H_INCLUDED



#endif // TLC59711_H_INCLUDED

#include "config.h"

//TLC59711 output channel "friendly names"
#define R0 0
#define G0 1
#define B0 2
#define R1 3
#define G1 4
#define B1 5
#define R2 6
#define G2 7
#define B2 8
#define R3 9
#define G3 10
#define B3 11

//#define bitrate 10000

typedef struct{
    long sdti;
    long scki;
    unsigned short channelstates[12];
    long cfg;
} tlcState_t;
/***
* Update all channels to new intensity levels.
*
* @param intensities - pointer to an array of 12 shorts of channel levels
* @param handle - state keeping variable to affect.
**/
int setchannels(unsigned short *intensities, tlcState_t *handle);
/***
* Set an individual channel on the device to specified intensity
* This command operates on the Grayscale configuration register for
* the specified channel.
**/
int setchannel(int channelid, unsigned short intensity, tlcState_t *handle);
/***
* Set the intensity of the specified channel group to the desired intensity
* This command operates on the group's Brightness Control register.
**/
int setgroup(int groupid, int intensity, tlcState_t *handle);
/***
* Initialize a connection to the device, returning a record for further calls
* to change the device state.
**/
tlcState_t *init(int sdti, int scki);
