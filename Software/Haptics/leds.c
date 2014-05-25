#include "leds.h"


void toggle(int pin){
    int curouta;
    curouta = _outa(0, 0);
    curouta ^= (1 << pin);
    _outa((1 << pin), curouta);
}
void heartbeat(){
    toggle(HEARTBEAT);
}
void led0(){
    toggle(LED0);
}
void led1(){
    toggle(LED1);
}
void led1_state(int state){
    if(state){
        _outa(1<<LED1, 1<<LED1);
    }else{
        _outa(1<<LED1, 0);
    }
}
void blink(int times, int pin){
    int i;
    for(i = times << 1; i > 0; i--){
        toggle(pin);
        _waitcnt(_cnt()+20000000);
    }
    _waitcnt(_cnt() + 40000000);
}
