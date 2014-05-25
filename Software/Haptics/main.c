/*
 * main.c - main program
 */

#include "config.h"
#include "mdriver.h"
#include "cdriver.h"
#include "bdriver.h"
#include "tdriver.h"
#include "leds.h"

unsigned short loc_channels[12];
int c_heading;
int data_changed;

void self_test(){
    t_aprintf("Self-Test Button pressed!\r");
    return;
}

void merge(){
    int i;
    if(compass_enable){
        if(c_heading > -15 && c_heading < 15){ //aimed north
            for(i = 0; i < 12; i++){
                if(i == 5){
                    loc_channels[i] = 32768; //turn channel 5 on at half intensity
                }else{
                    loc_channels[i] = (loc_channels[i] >> 1); //decrease intensity of other channels by 1/2
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    int i, j, loc_buttonstate;
    unsigned int stop_cnt;
    _dira((1 << HEARTBEAT),(1 << HEARTBEAT));
    _dira((1 << LED1), (1<<LED1));
    //launch all drivers
    launch_t();
    //blink(1, HEARTBEAT);
    launch_c();
    //blink(2, HEARTBEAT);
    launch_m();
    //blink(3, HEARTBEAT);
    launch_b();
    //blink(4, HEARTBEAT);
    // insert your code here
    if(POST_ENABLE){
        self_test();
    }
    //compass_enable = 1;
    while(1){
        //blink(1, HEARTBEAT);
        if(t_data_ready){
            //t_aprintf("Terminal indicates data ready\r");
            wait(t_rx_sem);
            //blink(5, HEARTBEAT);
            for(i = 0; i < 12; i++){
                loc_channels[i] = t_chan_data[i];
                //t_aprintf("channel %d - level %d\r", i, loc_channels[i]);
            }
            t_data_ready = 0;
            clear(t_rx_sem);
            data_changed = 1;
        }
        switch (buttonstates){
            case 0:
            //compass_enable = 0;
            break;
            case 1:
            compass_enable = 0;
            stop_cnt = _cnt() + MS_TO_CNT(COMPASS_TIMEOUT);
            buttonstates = 0;
            break;
            case 2:
            case 3:
            self_test();
            buttonstates = 0;
            break;
        }
        led1_state(compass_enable);
        if(compass_enable){

            c_heading = heading;
            //t_aprintf("Compass enabled. Current heading: %d\r", 0);
            //_waitcnt(_cnt()+40000000);
            if(_cnt() >= stop_cnt)
                compass_enable = 0;
            merge();
            data_changed = 1;
        }
        if(data_changed){
            //blink(10, HEARTBEAT);
            //t_aprintf("Motor driver data changed! Updating.\r");
            wait(m_sem);
            for(i = 0; i < 12; i++){
                channel_data[i] = loc_channels[i];
                //t_aprintf("Chan %3d Local %5d Motor %5d\r", i, loc_channels[i], channel_data[i]);
            }
            m_d_ready = 1;
            clear(m_sem);
            data_changed = 0;
        }
    }
    return 0;
}
