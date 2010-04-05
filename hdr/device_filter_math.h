#ifndef DEVICE_FILTER_MATH_H_INCLUDED
#define DEVICE_FILTER_MATH_H_INCLUDED

#define DEVICE_PI acosf(-1)

// linear congruential random number generator constants
// these values are used in java.util.Random.next()
#define a 0x5DEECE66DL
#define c 0xB
#define mask_bits 48
// generate only 31 bits so that we always generate positive integers
#define result_bits 31

// bit mask the least significant m bits
const long mask = ( 1L << mask_bits ) - 1;

const int LCG_RAND_MAX = ( 1 << result_bits ) - 1;

__device__ int device_lcg_rand( int );
__device__ float device_frand0( int, float );
__device__ float device_frand( int, float, float );
__device__ float device_erand( int, float );
__device__ float device_grand0( int, int );
__device__ float device_grand( int, int, float, float );
__device__ float device_gvalue( float, float, float );

#endif
