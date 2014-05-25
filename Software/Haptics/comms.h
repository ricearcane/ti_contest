/*
 * comms.h - Contains low-level communication protocols for Team ARCANE's
 * 			 Parallax Propeller code. Meant to be generally used for any
 *			 various protocols required. Functional pin declarations
 *			 should not be placed here.
 */

 #ifndef H_COMMS
 #define H_COMMS

 #include "config.h"

 typedef struct {
	long clk;		// SPI clock pin
	long cs; 		// Indicates if transmission is happening or not
	long miso;		// Data in
	long mosi;		// Data out
 } spiState_t;


 /* Read eight bits over SPI
  * Address is the address of the register being read from
  */
 char spi_read (spiState_t *handle, int address);

 /* Write eight bits over SPI
  * Address is the address of the register being written to, and data is what
  * is being written to that register
  */
 void spi_write (spiState_t *handle, int address, int data );

 /*
  * Initializes a spiState_t struct with the pins given as arguments.
  * clk, cs, and sdi will all be initialized as outputs and sdo will be an input
  */
 spiState_t *comms_init (int clk, int cs, int miso, int mosi);

 /*
  * Have the clock wait, turn off, wait, and turn on again. This assumes
  * the clock is already on.
  */
 void clocktick (spiState_t *handle, int waitticks);

 #endif // H_COMMS
