#include "TLC59711.h"



static void makeinput(int pin);
static void makeoutput(int pin);
static void set(int pin, int state);
static void toggle(int pin);
static void txd(tlcState_t *handle);

tlcState_t gstate;

tlcState_t *init(int sdti, int scki)
{

    tlcState_t *state = &gstate;
    state->sdti = sdti;
    state->scki = scki;

    makeoutput(sdti);
    makeoutput(scki);

    set(scki, 0);
    set(sdti, 0);

    return state;
}

int setchannels(unsigned short *intensities, tlcState_t *handle){
    int i;
    for(i = 0; i < 12; i++){
        handle->channelstates[i] = intensities[i];
    }
    txd(handle);
    return 0;
}

int setchannel(int channelid, unsigned short intensity, tlcState_t *handle){
    if(channelid > 11 || channelid < 0)
        return -1;
    handle->channelstates[channelid] = intensity;
#ifdef DEBUG
    if(VERBOSITY > 1) t_aprintf("[TLC59711.c DEBUG 2] setting channel %d to level %d\r", channelid, intensity);
    //t_aprintf("[TLC59711.c DEBUG 2] handle intensity: %d\r", handle->channelstates[channelid]);
#endif // DEBUG
    txd(handle);
    return 0;
}
int setgroup(int groupid, int intensity, tlcState_t *handle){
return -1;
}


static void txd(tlcState_t *handle){
    int i, j, waitticks;
    short temp;
    waitticks = _clockfreq()/(M_DATARATE << 1);
#ifdef DEBUG
    if(VERBOSITY > 2) t_aprintf("[TLC59711.c DEBUG 3]in txd, clkfreq = %d, waitticks = %d\r", _clockfreq(),waitticks);
#endif // DEBUG

    for(j = 14; j > 0 ; j--){
        if(j < 13) temp = handle->channelstates[j - 1]; //questionable. not sure if sizeof() necessary
        else if(j == 14) temp = (short)0x945F;
        else temp = (short)0xFFFF;
#ifdef DEBUG
        if(VERBOSITY > 1) t_aprintf("[TLC59711.c DEBUG 2] transmitting packet %d of 14, txd = 0x%x\r", j, temp);
#endif // DEBUG
        for (i = 15; i >= 0; i--){
            set(handle->scki, 0);
            set(handle->sdti, (temp >> i) & 1);
            _waitcnt(_cnt() + waitticks);
            set(handle->scki, 1);
            _waitcnt(_cnt() + waitticks);
        }
        set(handle->scki, 0);
    }
}
static void makeoutput(int pin){
    _dira((1 << pin),(1 << pin));
}

static void makeinput(int pin){
    _dira((1 << pin), 0);
}

static void set(int pin, int state){
    if(state == 1) _outa((1<<pin),(1<<pin));
    else _outa((1<<pin), 0);
}

static void toggle(int pin){
    int curouta = _outa(0, 0);
    curouta ^= (1 << pin);
    _outa((1 << pin), curouta);
}
