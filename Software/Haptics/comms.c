/*
 * comms.c - Contains low-level communication protocols for Team ARCANE's
 * 			 Parallax Propeller code. Meant to be generally used
 *			 for any various protocols required.
 */


// Includes
#include "comms.h"

 spiState_t spiState;

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

/* Read the value on a pin */
static int read(int pin) {
	int val  = (_ina() >> pin) & 1;
	return val;
}

 spiState_t *comms_init (int clk, int cs, int mosi, int miso) {

	// Init struct and attach pins
	spiState_t *state = &spiState;
	state -> clk = clk;
	state -> cs = cs;
	state -> miso = miso;
	state -> mosi = mosi;

	// Set pin directions
	makeoutput(clk);
	makeoutput(cs);
	makeoutput(mosi);
	makeinput(miso);

	// Initialize pin values
	set(cs, 1);
	set(clk, 1);
	set(mosi, 0);

	return state;
  }


 char spi_read (spiState_t *handle, int address) {
	int bit, waitticks, addr_save;
	char data_get = 0x00;
	waitticks = _clockfreq()/(C_DATARATE << 1);
#ifdef DEBUG
    if (VERBOSITY > 0) {
		t_aprintf("[comms.c DEBUG 1]: Reading message from address 0x%x\n", address);
		addr_save = address;
	}
#endif // DEBUG

	// Indicate that an SPI message is starting
	set(handle->cs, 0);
	set(handle->clk, 0);

	// Indicate to slave this is a read
	set(handle->mosi, 1);
	clocktick (handle, waitticks);

	// Do not increment address
	set(handle->mosi, 0);
	clocktick (handle, waitticks);

	// Bitbang address
	for (bit = 0; bit < 6; bit++) {

		// This is compared with 0x20 because max address is 0x3F,
		// so effective MSB is 0x20, not 0x80
		if (address & 0x20) {
			set(handle->mosi, 1);
		} else {
			set(handle->mosi, 0);
		}

		// Shift address to left so next bit can be examined
		address <<= 1;
		clocktick (handle, waitticks);
	}

	// Bitbang receiving data
	for (bit = 0; bit < 8; bit++) {

		// Data comes in MSB first
        data_get <<= 1;
		if (read(handle->miso) == 1) {
			data_get |= 0x01;
		}
		clocktick(handle, waitticks);
	}

	// Indicate end of message
	set(handle->clk, 1);
	set(handle->cs, 1);
#ifdef DEBUG
	if (VERBOSITY > 0) {
		t_aprintf("[comms.c DEBUG 1]: Got %d from address 0x%x\n", data_get, addr_save);
	}
#endif // DEBUG

	// Return data that was read in
	return data_get;
   }

 void spi_write (spiState_t *handle, int address, int data ) {
	int bit, waitticks;
	waitticks = _clockfreq()/(C_DATARATE << 1);
#ifdef DEBUG
	if (VERBOSITY > 0) {
		t_aprintf("[comms.c DEBUG 1]: Writing %02x to address 0x%x\n", data, address);
	}
	if (VERBOSITY > 1) {
        t_aprintf("[comms.c DEBUG 2]: waitticks = %d\n", waitticks);
	}
#endif // DEBUG
	// Indicate that an SPI message is starting
	set(handle->cs, 0);
	set(handle->clk, 0);

	// Indicate to slave this is a write
	set(handle->mosi, 0);
	clocktick (handle, waitticks);

	// Do not increment address
	set(handle->mosi, 0);
	clocktick (handle, waitticks);

	// Bitbang address
	for (bit = 0; bit < 6; bit++) {

		// This is compared with 0x20 because max address is 0x3F,
		// so effective MSB is 0x20, not 0x80
		if (address & 0x20) {
			set(handle->mosi, 1);
		} else {
			set(handle->mosi, 0);
		}

		// Shift address to left so next bit can be examined
		address <<= 1;
		clocktick (handle, waitticks);
	}

	// Bitbang data
	for (bit = 0; bit < 8; bit++) {

		// See if MSB is 1 or 0
		if (data & 0x80) {
			set(handle->mosi, 1);
		} else {
			set(handle->mosi, 0);
		}

		// Shift data to the left so next bit can be examined
		data <<= 1;
		clocktick (handle, waitticks);

	}

	// Indicate end of message
	set(handle->clk, 1);
	set(handle->cs, 1);
#ifdef DEBUG
	if (VERBOSITY > 0) {
		t_aprintf("[comms.c DEBUG 1]: Wrote %02x to address 0x%x\n", data, address);
	}
#endif // DEBUG
 }


 void clocktick (spiState_t *handle, int waitticks) {
	_waitcnt(_cnt() + waitticks);
	set(handle->clk,1);
	_waitcnt(_cnt() + waitticks);
	set(handle->clk, 0);
 }


