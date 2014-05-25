/* LSM303D.c - High-level functions for interfacing with the LSM303D
 *			   compass and accelerometer module
 */



#include "LSM303D.h"



data_t data_struct;

/* Private functions --------------------------------------*/


/* Perform a dot product of vectors a and b */
static float vector_dot (vector* a, vector* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

/* Perform a cross product of vectors a and b, storing the result in out */
static void vector_cross (vector* a, vector* b, vector* out) {
    out->x = (a->y * b->z) - (a->z * b->y);
    out->y = (a->z * b->x) - (a->x * b->z);
    out->z = (a->x * b->y) - (a->y * b->x);
}

/* Normalize the elements of vector v */
static void vector_norm (vector* v) {
    float mag = sqrt(vector_dot(v,v));
    v -> x /= mag;
    v -> y /= mag;
    v -> z /= mag;
}

/*
 * Used to send the accel and magnetometer data as a
 * comma-separated list
 */
static void send_csv(int first, int second, int third) {
/*
    // The ints themselves can be up to 5 digits long,
    // but there could also be a negative sign at the front.
    int intlength = 6;

    // Send 3 values separated by commas followed by newline
    t_dec(first, intlength);
    t_char(',');
    t_dec(second, intlength);
    t_char(',');
    t_dec(third, intlength);
    t_char(',');
    t_nl();
  */
    t_aprintf("%d,%d,%d/n", first, second, third);

}

/* Public functions ----------------------------------------*/

vector *vector_init(vector *new_vec, float x, float y, float z) {
    /*vector* new_vec = malloc (sizeof(vector));
    if(new_vec == NULL){
        t_aprintf("malloc failure!\r");
    }*/
#ifdef DEBUG
    if(VERBOSITY > 1){
        t_aprintf("[LM303D DEBUG 2]: initializing vector [%f,%f,%f] at address %x\n", x, y, z, (int)new_vec);
    }
#endif // DEBUG
    new_vec -> x = x;
    new_vec -> y = y;
    new_vec -> z = z;

    return new_vec;
}

void vector_destroy(vector* v) {
#ifdef DEBUG
    if(VERBOSITY > 1){
        t_aprintf("[LM303D DEBUG 2]: destroying vector at address %x\n", (int)v);
    }
#endif // DEBUG
    v -> x = 0;
    v -> y = 0;
    v -> z = 0;
    //free(v);
}

data_t* init_data_struct(void) {

	data_t* data = &data_struct;

	// Initializa all data fields to 0
	data-> Ax = 0;
	data-> Ay = 0;
	data-> Az = 0;
	data-> Mx = 0;
	data-> My = 0;
	data-> Mz = 0;

	return data;
}

 void lsm303d_wakeup(spiState_t* handle) {
#ifdef DEBUG
    if (VERBOSITY > 0){
        t_aprintf("[LM303D DEBUG 1]: Sending Wakeup and config packets...\n");
    }
#endif // DEBUG
	// Set the accelerometer to 50 Hz BW, continuous mode, all axes on
	spi_write(handle, CTRL1, 0x5F); //disable register update until read

	// Set magnetometer to 50 Hz BW
	spi_write(handle, CTRL5, 0x10);

	// Set magnetometer readings to be between +/- 2g
	spi_write(handle, CTRL6, 0x00);

	/* Note that accelerometer is already at +/- 2g */

    spi_write(handle, CTRL7, 0x00); //enable continuous mode for mag data
 }

 void read_accel_data(data_t* data, spiState_t* handle) {

	// There are two values each for X, Y, and Z
	char accel_data[6];

	// Read all 6 data fields
	accel_data[0] = spi_read(handle, OUT_X_L_A);
	accel_data[1] = spi_read(handle, OUT_X_H_A);
	accel_data[2] = spi_read(handle, OUT_Y_L_A);
	accel_data[3] = spi_read(handle, OUT_Y_H_A);
	accel_data[4] = spi_read(handle, OUT_Z_L_A);
	accel_data[5] = spi_read(handle, OUT_Z_H_A);

	// Calculate full values
	data -> Ax = (int)(accel_data[1] << 8) + accel_data[0];
	data -> Ay = (int)(accel_data[3] << 8) + accel_data[2];
	data -> Az = (int)(accel_data[5] << 8) + accel_data[4];
#ifdef DEBUG
	if(VERBOSITY > 0){
        t_aprintf("[LM303D DEBUG 1]: Raw Accelerometer Data [%d,%d,%d]\n", data->Ax, data->Ay, data->Az);
	}
#endif // DEBUG
 }

  void read_mag_data(data_t* data, spiState_t* handle) {

	// There are two values each of X, Y, and Z
	char mag_data[6];

	// Read all 6 data fields
	mag_data[0] = spi_read(handle, OUT_X_L_M);
	mag_data[1] = spi_read(handle, OUT_X_H_M);
	mag_data[2] = spi_read(handle, OUT_Y_L_M);
	mag_data[3] = spi_read(handle, OUT_Y_H_M);
	mag_data[4] = spi_read(handle, OUT_Z_L_M);
	mag_data[5] = spi_read(handle, OUT_Z_H_M);

	// Calculate full values
	data -> Mx = (int)(mag_data[1] << 8) + mag_data[0];
	data -> My = (int)(mag_data[3] << 8) + mag_data[2];
	data -> Mz = (int)(mag_data[5] << 8) + mag_data[4];
#ifdef DEBUG
	if(VERBOSITY > 0){
        t_aprintf("[LM303D DEBUG 1]: Raw Magnetometer Data [%d,%d,%d]\n", data->Mx, data->My, data->Mz);
	}
#endif // DEBUG
  }

 int get_heading (data_t* data) {
    // Initialize heading value -  it's an integer because it is
    // going to eventually be binned into North, South, East, or West
    vector north, east, mag_vector, accel_vector, from;
    int heading;

    // Initialize north and east vectors
    vector_init(&north, 0, 0, 0);
    vector_init(&east, 0, 0, 0);

    // Put magnetometer and accelerometer data into vector format
    vector_init(&mag_vector, data->Mx, data->My, data-> Mz);
    vector_init(&accel_vector, data->Ax, data->Ay, data-> Az);

    // Initialize "from" vector based on carrier board design
    vector_init(&from, 1, 0, 0);

    // Subtract calibration offset from magnetometer readings
    mag_vector.x -= ((float) MAG_MAX_X + MAG_MIN_X) / 2.0;
    mag_vector.y -= ((float) MAG_MAX_Y + MAG_MIN_Y) / 2.0;
    mag_vector.z -= ((float) MAG_MAX_Z + MAG_MIN_Z) / 2.0;

    // Compute East and North
    vector_cross(&mag_vector, &accel_vector, &east);
    vector_norm(&east);
    vector_cross(&accel_vector, &east, &north);
    vector_norm(&north);

    // Compute the heading and make sure it's positive
    heading = (int)(atan2(vector_dot(&east, &from), vector_dot(&north, &from))* 180 / PI);
    if (heading < 0) {
        heading += 360;
    }

    // Memory cleanup
    vector_destroy(&north);
    vector_destroy(&east);
    vector_destroy(&mag_vector);
    vector_destroy(&accel_vector);
    vector_destroy(&from);

    return heading;


 }


void print_accel_vals(data_t* data) {
    int Ax, Ay, Az;

    // Get accel data
    Ax = data -> Ax;
    Ay = data -> Ay;
    Az = data -> Az;

    // Send accel data
    send_csv(Ax, Ay, Az);
}
void print_mag_vals(data_t* data) {
    int Mx, My, Mz;

    // Get magnetometer data
    Mx = data -> Mx;
    My = data -> My;
    Mz = data -> Mz;

    // Send magnetomoter data
    send_csv(Mx, My, Mz);

}

