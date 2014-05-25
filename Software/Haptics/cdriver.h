#ifndef CDRIVER_H_INCLUDED
#define CDRIVER_H_INCLUDED

#include "sem.h"
#include "config.h"
#include "LSM303D.h"
#include "comms.h"

/*
 * cdriver.h - Adds data reading and heading computation capabilities. Allows for
 *             threaded or multicore implmentations
 */

// Compass heading value - extern because it is accessed in cdriver.c and main.c
extern int heading;

// Writing a 1 to this value causes the compass to update
extern int compass_enable;

// Top-level compass function
void launch_c();


#endif // CDRIVER_H_INCLUDED
