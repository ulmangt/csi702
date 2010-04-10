#import <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "filter_cuda_data.h"
#include "filter_constants.h"

#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

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

const long LCG_RAND_MAX = ( 1L << result_bits ) - 1;

// implementation based on examples from:
// http://en.wikipedia.org/wiki/Linear_congruential_generator
// java.util.Random.next()
__device__ int device_lcg_rand( int x )
{
  // bitwise and with mask is equivalent to mod 2^mask_bits
  long xl = (x * a + c) & mask;
  // we have generated mask_bits but only need result_bits
  // use the highest order bits because they have longer periods
  return (int) ( xl >> ( mask_bits - result_bits ) );
}

// Return a random float value evenly distributed between 0 and max
// Because cuda threads will keep track of their own seeds, this function
// and the others like it are not random. They simply take an already
// generated random value (seed) and transform it in some way.
// Seed values should be generated by repeated calls to device_lcg_rand().
__device__ float device_frand0( int seed, float max )
{
  return ( float ) seed / ( float ) LCG_RAND_MAX * max ;
}

// return a random float value evenly distributed between min and max
__device__ float device_frand( int seed, float min, float max )
{
  float diff = max - min;
  return device_frand0( seed, diff ) + min;
}


void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) 
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(-1);
    }
    else
    {
        fprintf(stderr, "No error: %s.\n", msg);
    }
}

// CUDA kernel function : time update a particle
__global__ void time_update_kernel( struct particles *list, float seconds, float mean_maneuver )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles particle = list[index];

  particle.x_pos = particle.x_pos + particle.x_vel * seconds;
  particle.y_pos = particle.y_pos + particle.y_vel * seconds;
}

extern "C" void time_update( struct particles *list, int num, float seconds, float mean_maneuver )
{
  int numBlocks = num / THREADS_PER_BLOCK;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(THREADS_PER_BLOCK);
  time_update_kernel<<< dimGrid, dimBlock >>>( list, seconds, mean_maneuver );

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("time_update");
}

// CUDA kernel function : initialize a particle
__global__ void init_particles_kernel( struct particles *list )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles particle = list[index];

  int seed = particle.seed;

  particle.x_pos  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  particle.y_pos  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  particle.x_vel  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  particle.y_vel  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  particle.weight = 1.0;

  particle.seed = seed;
}

extern "C" void init_particles( struct particles *host, int num )
{
  int numBlocks = num / THREADS_PER_BLOCK;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(THREADS_PER_BLOCK);
  init_particles_kernel<<< dimGrid, dimBlock >>>( host );

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("init_particles");
}


extern "C" void h_init_seed( struct particles *host, int num )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    host[i].seed = rand();
  }
}

extern "C" void copy_particles_host_to_device( struct particles *device, struct particles *host, int num )
{
  int size = sizeof( struct particles ) * num;

  cudaMemcpy( device, host, size, cudaMemcpyHostToDevice );
}

extern "C" void copy_particles_device_to_host( struct particles *host, struct particles *device, int num )
{
  int size = sizeof( struct particles ) * num;

  cudaMemcpy( host, device, size, cudaMemcpyDeviceToHost );
}

// allocate memory for num particles on host
extern "C" struct particles * h_init_particle_mem( int num )
{
  int size = sizeof( struct particles ) * num ;
  return ( struct particles * ) malloc( size );
}

// allocate memory for num particles on device
extern "C" struct particles *d_init_particle_mem( int num )
{
  int size = sizeof( struct particles ) * num ;
  struct particles *list;

  cudaMalloc( (void **) &list, size );

  return list;
}

// free particle memory on host
extern "C" void h_free_particle_mem( struct particles *list )
{
  free( list );
}

// free particle memory on device
extern "C" void d_free_particle_mem( struct particles *list )
{
  cudaFree( list );
}






