#include "mdriver.h"
#include "sem.h"
#include "tdriver.h"
#include "leds.h"

static tlcState_t *state;
static unsigned short l_chan_data[12];


int m_d_ready;                    //writing a 1 here causes an update.
unsigned short channel_data[12];    //data for all driver channels (0-65535)
int m_sem;                     //semaphore for channel data. -1 for errors

unsigned long md_stack[M_STACK_SIZE];

static void run_md();


/***
* Begin execution of the motor driver loop in a new Cog
**/
void launch_m(){
    _coginit_C(&run_md,(unsigned long *)&md_stack + M_STACK_SIZE); //start the motor driver code
}

/***
* main motor driver loop.
**/
static void run_md(){
    int i;

    _dira(1<<HEARTBEAT, 1<<HEARTBEAT);
    while((m_sem = get_semaphore()) == -1){ //attempt to get a semaphore
        _waitcnt(_cnt() + 80000000); //wait a second between semaphore retries
    }
    state = init(SDTI, SCKI);
    while(1){
        if(m_d_ready == 1){
            wait(m_sem); //wait to acquire semaphore
            for(i = 0; i < 12; i++){
                l_chan_data[i] = channel_data[i]; //update local copy
                //t_aprintf("Chan %3d Local %5d Motor %5d\r", i, l_chan_data[i], channel_data[i]);
            }
            m_d_ready = 0; //clear d_ready
            clear(m_sem); //release semaphore

            //t_aprintf("Sending new channel data: \r");
            for(i = 0; i < 12; i++){
                //t_aprintf("Channel %d - Level %d\r", i, l_chan_data[i]);
            }
            heartbeat();
            setchannels((unsigned short *)&l_chan_data, state); //send new data

            //heartbeat();

            _waitcnt(_cnt() + MS_TO_CNT(M_UPDATE_DELAY));
        }
        else {
            _waitcnt(_cnt() + MS_TO_CNT(M_TIMEOUT_DELAY));
        }
    }

}
