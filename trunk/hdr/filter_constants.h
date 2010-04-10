
#ifndef FILTER_CONSTANTS_H_INCLUDED
#define FILTER_CONSTANTS_H_INCLUDED

#define THREADS_PER_BLOCK 512

#define NUM_PARTICLES THREADS_PER_BLOCK * 500
#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

#endif
