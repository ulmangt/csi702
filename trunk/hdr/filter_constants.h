
#ifndef FILTER_CONSTANTS_H_INCLUDED
#define FILTER_CONSTANTS_H_INCLUDED

// reduction routines are picky about these values
// NUM_PARTICLES must be a power of 2 (routines perform a binary fan-in)
// THREADS_PER_BLOCK must be greater than or equal to 64
// (routines have unrolled loops which fail for smaller numbers of threads per block)
#define THREADS_PER_BLOCK 512
//#define NUM_BLOCKS 512 * 8
#define NUM_BLOCKS 512 * 64
#define NUM_PARTICLES THREADS_PER_BLOCK * NUM_BLOCKS

#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

#endif
