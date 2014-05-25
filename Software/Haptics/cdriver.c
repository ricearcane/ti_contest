/*
 * cdriver.c
 */

 #include "cdriver.h"
 #include "tdriver.h"

int compass_enable;
int heading;

unsigned long c_stack[C_STACK_SIZE];

void c_get_heading();

void launch_c(){
    _coginit_C(&c_get_heading, (unsigned long *)&c_stack + C_STACK_SIZE);
}


void c_get_heading() {
    // Initialize SPI and data structs and heading value
    spiState_t *spi_handle;
    data_t *data_handle;

    // Intialize all compass settings and vars
    spi_handle = comms_init(C_CLK, C_SS, C_MOSI, C_MISO);
	data_handle = init_data_struct();

	// Set the LSM303D to send the correct data
	lsm303d_wakeup(spi_handle);

    while (1) {

        if (compass_enable){
            // Get raw data
            read_accel_data(data_handle, spi_handle);
            read_mag_data(data_handle, spi_handle);

            // Get the compass heading
            heading = get_heading(data_handle);

            //t_aprintf("Compass enabled. Current heading: %d\r", heading);

            _waitcnt(_cnt() + MS_TO_CNT(C_UPDATE_DELAY));

        } else {
            _waitcnt(_cnt() + MS_TO_CNT(C_TIMEOUT_DELAY));
        }

    }


}
