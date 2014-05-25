/*
 * bdriver.c
 */

 #include "bdriver.h"
 #include "tdriver.h"
 #include "leds.h"


#define CMASK (1 << PB_COMPASS)
#define TMASK (1 << PB_TEST)
#define PINMASK (1 << PB_COMPASS) | (1 << PB_TEST)

 unsigned long pb_stack[PB_STACK_SIZE];
 int buttonstates;

 static void poll_buttons(){
    //_dira(1<<LED1, 1<<LED1);

 //t_aprintf("bdriver.c pinmask: %%%b\r", PINMASK);
     while (1) {
        //led1();
        // Read input buttons and adjust buttonstates accordingly
        // Buttons are active low
        //_waitpeq(CMASK,CMASK,0); //wait for a button to be pressed
        //t_aprintf("ina: %%%b\r", _ina());
        //t_aprintf("mask:%%%b\r", PINMASK);
        if (((_ina() >> PB_COMPASS) & 1) == 0) {
            //t_aprintf("compass button down\r");
            buttonstates |= 0x01;
        }
        if (((_ina() >> PB_TEST) & 1) == 0) {
            //t_aprintf("test button down\r");
            buttonstates |= 0x02;
        }
        //t_aprintf("Button press detected. Current buttonstate: %d\r", buttonstates);
        _waitcnt(_cnt() + MS_TO_CNT(PB_POLL_DELAY));
     }
 }

  void launch_b() {
    _coginit_C(&poll_buttons,(unsigned long *)&pb_stack + PB_STACK_SIZE);
 }
