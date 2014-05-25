/* LSM303D.h - Header file for interfacing with the LSM303D compass and
 * 			   acceleromter module.
 */

 #include <math.h>
 #include <stdlib.h>
 #include "comms.h"
 #include "config.h"
 #include "tdriver.h"

 #ifndef H_LSM303D
 #define H_LSM303D

 // For some reason the math.h standard PI is not defined.
 // This goes to far fewer sig figs than the math.h version, but we're quantizing anyway.
 #define PI             3.14259

 // Define names for LSM303D registers
 // Control registers:
 #define CTRL0			0x1F
 #define CTRL1			0x20
 #define CTRL2			0x21
 #define CTRL3			0x22
 #define CTRL4			0x23
 #define CTRL5			0x24
 #define CTRL6			0x25
 #define CTRL7			0x26

 // Magnetometer data
 #define OUT_X_L_M		0x08
 #define OUT_X_H_M		0x09
 #define OUT_Y_L_M		0x0A
 #define OUT_Y_H_M		0x0B
 #define OUT_Z_L_M		0x0C
 #define OUT_Z_H_M		0x0D

 // Accelerometer data
 #define OUT_X_L_A		0x28
 #define OUT_X_H_A		0x29
 #define OUT_Y_L_A		0x2A
 #define OUT_Y_H_A		0x2B
 #define OUT_Z_L_A		0x2C
 #define OUT_Z_H_A		0x2D

 // Calibration data
 #define MAG_MIN_X      -32767
 #define MAG_MIN_Y      -32767
 #define MAG_MIN_Z      -32767
 #define MAG_MAX_X       32767
 #define MAG_MAX_Y       32767
 #define MAG_MAX_Z       32767


/* Generic 3D vector structure */
typedef struct {
    float x, y, z;
} vector;

 /*
  * Struct which contains raw compass and accelerometer data
  * Vars starting with 'A' are accel data, vars starting with 'M' are
  * magnetometer data
  */

 typedef struct  {
	short Ax, Ay, Az, Mx, My, Mz;
 } data_t;


 /* Initialize data struct */
 data_t* init_data_struct (void);

 /* Initialize a vector with values x, y, and z */
 vector* vector_init(vector * new_vec, float x, float y, float z);

 /* Frees the memory used for a vector */
 void vector_destroy(vector* v);

 /* Wake up the LSM303D module by writing to its control registers */
 void lsm303d_wakeup(spiState_t *handle);

 /* Read accelerometer data */
 void read_accel_data(data_t* data, spiState_t* handle);

 /* Read magnetomter data */
 void read_mag_data(data_t* data, spiState_t* handle);


/*
Returns the angular difference in the horizontal plane between the
vector <1 0 0> (decided by the defined directions of the LSM303D carrier board)
and north, in degrees.

Description of heading algorithm:
Shift and scale the magnetic reading based on calibration data to find
the North vector. Use the acceleration readings to determine the Up
vector (gravity is measured as an upward acceleration). The cross
product of North and Up vectors is East. The vectors East and North
form a basis for the horizontal plane. The From vector is projected
into the horizontal plane and the angle between the projected vector
and horizontal north is returned.

Note that this algorithm and its functions are adapted from the Pololu Arduino Library at:
https://github.com/pololu/lsm303-arduino/blob/master/LSM303/LSM303.cpp
*/
 int get_heading (data_t* data);

// Functions for printing values over the serial port
void print_accel_vals(data_t* data);
void print_mag_vals(data_t* data);


 #endif //H_LSM303D
