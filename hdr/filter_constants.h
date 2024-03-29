
#ifndef FILTER_CONSTANTS_H_INCLUDED
#define FILTER_CONSTANTS_H_INCLUDED

// reduction routines are picky about these values
// NUM_PARTICLES must be a power of 2 (routines perform a binary fan-in)
// THREADS_PER_BLOCK must be greater than or equal to 64
// (routines have unrolled loops which fail for smaller numbers of threads per block)
#define THREADS_PER_BLOCK 256
#define NUM_BLOCKS 512 * 16
#define NUM_PARTICLES THREADS_PER_BLOCK * NUM_BLOCKS
#define NUM_EFFECT_CUTOFF NUM_PARTICLES >> 4

#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "data/particles.out"
#define OWNSHIP_POS_NAME "data/ownship.out"
#define TARGET_POS_NAME "data/target.out"

#endif
