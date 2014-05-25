#include "tdriver.h"
#include "sem.h"
#include <string.h>
#include <catalina_tty.h>
#include <stdio.h>
#include <stdarg.h>

unsigned short t_chan_data[12];
int t_rx_sem;
int t_data_ready;

int t_print_ready;
int t_tx_sem;

unsigned long t_stack[T_STACK_SIZE];

static char buf[RX_BUFFER_SIZE + 1];
static int rxptr;
static int cmdline;

/***
* Wrapper around printf to support printf calls to the terminal. Ensures that
* only one cog may print to the output terminal at a time. Autonegotiates for
* active terminal, whether bluetooth or usb. Only supports the following
* format types:
*
* %x : print an integer as a hexadecimal value
* %d : print an integer as a decimal value
* %s : print a string (null terminated)
* %c : print a character
* %b : print an integer as a binary value
* %% : print a %
*
**/
void t_aprintf(const char *_format, ...){
    const char *p;
    int len, zext;

    va_list args;
    va_start(args, _format);        //extract vararg list

    wait(t_tx_sem);                 //acquire mutex

    for(p = _format; *p != 0; p++){ //handle each character
        if(*p == '%'){
            p++;
            if(*p > 47 && *p < 58){     //format tag has length specifier
                if(*p == '0'){      //case %0_?
                    p++;
                    zext = 1;           //zero extend
                    len = *p - 48;   //get format length
                    p++;
                    //tty_str(0, "zero-extending, length: ");
                    //tty_dec(0, len);
                    //tty_newline(0);
                }else{                  //case %n_?
                    zext = 0;           //don't zero extend
                    len = *p - 48;      //get format length
                    p++;
                }
            }else{                      //case %_?
                zext = 0;               //don't zero pad
                len = 0;
            }
            switch(*p){
                case 'x':
                case 'X':
                    if(len == 0){
                        len = 8;
                    }
                    tty_hex((unsigned)va_arg(args, int), len);
                    break;
                case 'd':
                case 'D':
                    if(len == 0){
                        tty_decl(va_arg(args, int), 10, 0);
                    }else{
                        tty_decl(va_arg(args, int), len,(zext + 1));
                    }
                    break;
                case 's':
                case 'S':
                    tty_str(va_arg(args, char *));
                    break;
                case 'c':
                case 'C':
                    tty_tx(va_arg(args, int));
                    break;
                case 'b':
                case 'B':
                    if(len == 0) len = 32; //print all digits if unspecified
                    tty_bin((unsigned)va_arg(args, int), len);
                    break;
                case '%':
                    tty_tx('%');
                    break;
                }
        }else{
            tty_tx(*p); //print things not following format flag
        }

    }
    clear(t_tx_sem);
    va_end(args);
}

static int usb_connect(){
    return ((_ina() >> USB_DETECT) & 1);
}
static void help(){
t_aprintf("=============================================================================\r\r"
            "Command summary:\r"
            "_____________________________________________________________________________\r"
            "[s] i <ch> - set channel to intensity, or all to intensity if channel omitted\r"
            "[m] <live/blind> - enable or disable this helpful command line\r"
            "[?] - display this helpful help message\r"
            "[h] - same as [?]\r"
            "_____________________________________________________________________________\r");
}
static void parse(unsigned port){

    char *index1, *index2, *p;
    int arg1, arg2, argc;

    //tty_str(0, "received");
    //tty_str(0, buf);
    //tty_newline(0);
    for(p = buf; *p != 0; p++){
        if(*p == '\r') *p = 0;
    }

    index1 = strchr(buf, ' '); //find first and second spaces delimiting args
    index2 = strchr(index1 + 1, ' ');
    argc = 2;

    if (index2 == 0) argc = 1; //no second ' ': either 1 or 0 args
    if (index1 == 0) argc = 0; //no first  ' ': no args.

    arg1 = arg2 = -2;

    if(argc > 0){ //at least 1 argument?
        *index1 = 0; // replace ' ' with \0
        index1++;
        arg1 = atoi(index1); //get arg1 as an integer

        if(argc > 1){ //2 arguments?
            //buf[(int)(index2 - buf)] = 0; // this splits buf into substrings
            *index2 = 0;
            index2++;
            arg2 = atoi(index2); //get arg2 as an integer
        }
    }

     //get integers out of the two arg substrings


    switch(*buf) //handle buf char switch
    {
    case 's':
    case 'S':
        //blink(LED0, 3);
        wait(t_rx_sem);
        if (argc == 1){ //handle 'set all' command
            if(cmdline) t_aprintf("All channels set"
                        " to level %d\r", (unsigned)arg1);

            for(arg2 = 0; arg2 < 12; arg2++){ //reusing arg2 for size (minimal)
                t_chan_data[arg2] = (short)arg1; //arg1 has intensity.
            }
        }else{
            if((arg2 >= 0) && arg2 < 12){ //keep writes within array
                if(cmdline) t_aprintf("Channel %d set"
                        " to level %d\r", arg2, (unsigned) arg1);

                t_chan_data[arg2] = (short)arg1;
            }else{
                if(cmdline) t_aprintf("Channel %d is out of bounds! "
                        "Acceptable range is 0-11\r", arg2);
            }
        }
        t_data_ready = 1;
        clear(t_rx_sem);
        break;
    case 'm':
    case 'M':
        //blink(LED0, 4);
        if(argc == 1 && strncmp("live", index1, 4) == 0){
            t_aprintf("Entering Live Mode\r");
            cmdline = 1;
        }else if(argc == 1 && strncmp("blind", index1, 5) == 0){
            t_aprintf("Entering Blind Mode."
                        " No further CLI statements will print\r");
            cmdline = 0;
        }else{
            t_aprintf("Unknown CLI mode [%s]. Try [?] for help\r", index1);
        }
        break;
    case '?':
    case 'h':
    case 'H':
        //blink(5, LED0);
        help();
        break;
    default :
        break;
    }
}
static void init_t(){
    _dira(1<<LED0, 1<<LED0);
    _dira(1<<LED1, 1<<LED1);

    //blink (4, HEARTBEAT);
    while((t_rx_sem = get_semaphore()) == -1){  //acquire semaphore
        _waitcnt(_cnt() + 80000000);            //or die trying
    }
    //blink(3, HEARTBEAT);
    while((t_tx_sem = get_semaphore()) == -1){
    	_waitcnt(_cnt() + 80000000);
    }
    //blink(2, HEARTBEAT);
    rxptr = 0;
    cmdline = CLI_ON_BOOT;
    if(cmdline){
        tty_tx((char)16);
        help();
        tty_txflush();
    }
    tty_rxflush(1-usb_connect());
}
void test_printf(){
t_aprintf("Testing printf: \r");
t_aprintf("Decimal: %d\r", 10);
t_aprintf("hexadecimal: %x\r", 0x18);
t_aprintf("binary: %b\r", 0x5005);
t_aprintf("fixed-width decimals: %05d, %5d\r", 10, 11);
t_aprintf("fixed-width hex: %05x, %5x\r", 0x5004, 0x5005);
t_aprintf("fixed-width bin: %04b, %4b\r", 0x0A, 0x0B);
t_aprintf("character: %c\r", 'y');
t_aprintf("string: %s\r", "hello");
t_aprintf("percent: %%");

}
void t_main(){
    int i;
    char temp;
    init_t();
    //tty_str(0, "terminal driver past init. trying printf\r");
    //test_printf();
    while(1){
        //blink(1, HEARTBEAT);
        //led0();
        if((temp = tty_rxcheck()) != 255){
            if(temp == 8){ //handle backspace nicely
                rxptr-= 2;
            }else{
                buf[rxptr] = temp;
            }

            //t_aprintf("received: %d (%c)\r", temp, temp);

            if(buf[rxptr] == TTY_CR || buf[rxptr] == '\n' || buf[rxptr] == TTY_FF){ //newline terminated string?
                parse(USB_PORT); //parse it and update motor levels/terminal params
                rxptr = 0;
                for(i = 0; i <= RX_BUFFER_SIZE; i++){
                    buf[i] = 0;
                }
                led0();
                //tty_rxflush(USB_PORT);
            }else{
                //led1();
                rxptr++; //increment rx position
                if(rxptr >= RX_BUFFER_SIZE)
                    rxptr = 0;
            }//else
        }//if
    }//while
}

/***
* Launch the terminal driver code to run in the next available cog.
* @return - the ID of the cog code was launched into or -1 if no free cogs exist
**/
int launch_t(){
   return (_coginit_C(&t_main, (unsigned long *)&t_stack + T_STACK_SIZE));
}

