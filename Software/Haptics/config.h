#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED



#endif // CONFIG_H_INCLUDED


#include <catalina_cog.h>

//========[COMPILATION OPTIONS]================================================
//#define DEBUG                       //uncomment to enable debugging code
//========[STATE MACHINE CONSTANTS AND DELAYS (ms)]============================
#define PB_POLL_DELAY   100         //delay between pollings of user buttons

#define C_TIMEOUT_DELAY 100         //delay between checking if compass enabled
#define C_UPDATE_DELAY  100         //delay between successive compass updates

#define M_TIMEOUT_DELAY 100         //delay between motor driver level checks
#define M_UPDATE_DELAY  10          //delay between motor driver state updates
//========[DEBUGGING AND FEATURE SETTINGS]=====================================
#define BLUETOOTH       0           //enable or disable bluetooth support.
#define NO_BT_ON_USB    0           //switch from bluetooth to USB if present?
#define VERBOSITY       0           //how much debugging information?
#define CLI_ON_BOOT     0           //start up in CLI mode?
#define POST_ENABLE     0           //perform a power-on self test on boot?
#define COMPASS_TIMEOUT 10000       //ms for compass to remain active when button pressed
//========[SERIAL IO SETTINGS]=================================================
#define BT_PORT         1
#define USB_PORT        0
#define USB_DETECT      21          //pin for USB presence detection
#define RX_BUFFER_SIZE  32          //number of characters to locally buffer
#define TX_OVERHEAD     64          //number of additional bytes to reserve for t_aprintf operations
#define T_STACK_SIZE    256        //bytes to reserve for terminal stack
//========[MOTOR DRIVER CONFIGURATION]=========================================
#define SDTI            17
#define SCKI            16
#define M_DATARATE      10000       //serial datarate in bits per second
#define M_STACK_SIZE    128          //longs to reserve for motor driver stack

//========[COMPASS SETTINGS]===================================================
#define C_MOSI          0
#define C_MISO          2
#define C_SS            3
#define C_CLK           1
#define C_DATARATE      10000       //spi datarate in bits per second
#define C_STACK_SIZE    1024          //longs to reserve for compass stack
//========[USER INTERFACE SETTINGS]============================================
#define HEARTBEAT       9
#define LED0            19
#define LED1            20
#define PB_COMPASS      10
#define PB_TEST         11
#define PB_STACK_SIZE   128
//========[MACROS]=============================================================
#define MS_TO_CNT(a)       (a*80000) //convert milliseconds to counter ticks
