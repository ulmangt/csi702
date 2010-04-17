
#ifndef FILTER_CONSTANTS_H_INCLUDED
#define FILTER_CONSTANTS_H_INCLUDED

#define THREADS_PER_BLOCK 256
#define NUM_BLOCKS 512 * 8
// NUM_PARTICLES must be a power of 2 (for reduction routine to work)
#define NUM_PARTICLES THREADS_PER_BLOCK * NUM_BLOCKS
#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

#endif
