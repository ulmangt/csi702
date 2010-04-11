#import <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "obs_math.h"

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
__global__ void time_update_kernel( struct particles *list, float time_sec, float mean_maneuver )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles *particle = list + index;

  particle->x_pos = particle->x_pos + particle->x_vel * time_sec;
  particle->y_pos = particle->y_pos + particle->y_vel * time_sec;
}

extern "C" void time_update( struct particles *list, int num, float time_sec, float mean_maneuver )
{
  int numBlocks = num / THREADS_PER_BLOCK;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(THREADS_PER_BLOCK);
  time_update_kernel<<< dimGrid, dimBlock >>>( list, time_sec, mean_maneuver );

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("time_update");
}

// CUDA kernel function : initialize a particle
__global__ void init_particles_kernel( struct particles *list )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles *particle = list + index;

  int seed = particle->seed;

  particle->x_pos  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  particle->y_pos  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  particle->x_vel  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  particle->y_vel  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  particle->weight = 1.0;

  particle->seed = seed;
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

__device__ float azimuth( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  if ( x_diff == 0 && y_diff > 0 ) return DEVICE_PI / 2.0;
  if ( x_diff == 0 && y_diff < 0 ) return -DEVICE_PI / 2.0;

  return atan2( y_diff, x_diff );
}

// calculates the probability density function for the gaussian
// distribution with given mean and sigma
__device__ float gvalue( float value, float mean, float sigma )
{
  float z = ( value - mean ) / sigma ;
  return exp( -0.5 * z * z ) / ( sqrt( 2.0 * DEVICE_PI ) * sigma );
}

__device__ float range( float to_x_pos, float to_y_pos, float from_x_pos, float from_y_pos )
{
  float x_diff = from_x_pos - to_x_pos;
  float y_diff = from_y_pos - to_y_pos;

  return sqrt( x_diff * x_diff + y_diff * y_diff );
}

__global__ void apply_azimuth_observation_kernel( struct particles *list, float x_pos, float y_pos, float value, float error )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles *particle = list + index;

  float particle_azimuth = azimuth( x_pos , y_pos , particle->x_pos , particle->y_pos );
  float observed_azimuth = value;
  float likelihood = gvalue( particle_azimuth - observed_azimuth , 0.0 , error );

  particle->weight = particle->weight * likelihood;
}

__global__ void apply_range_observation_kernel( struct particles *list, float x_pos, float y_pos, float value, float error )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  struct particles *particle = list + index;

  float particle_range = range( x_pos , y_pos , particle->x_pos , particle->y_pos );
  float observed_range = value;
  float likelihood = gvalue( particle_range - observed_range , 0.0 , error );

  particle->weight = particle->weight * likelihood;
}

extern "C" void information_update( struct observation *obs, struct particles *host, int num )
{
  int numBlocks = num / THREADS_PER_BLOCK;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(THREADS_PER_BLOCK);

  switch( obs->type )
  {
    case AZIMUTH:
      apply_azimuth_observation_kernel<<< dimGrid, dimBlock >>>( host, obs->x_pos, obs->y_pos, obs->value, obs->error );
      break;
    case RANGE:
      apply_range_observation_kernel<<< dimGrid, dimBlock >>>( host, obs->x_pos, obs->y_pos, obs->value, obs->error );
      break;
  }

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("information_update");
}

/*
extern "C" struct observation_list * d_init_and_copy_observations( struct observation_list *h_obs_list )
{
  int size = sizeof( struct observation_list );
  struct observation_list *d_obs_list;

  cudaMalloc( (void **) &d_obs_list, size );

  d_obs_list->size = h_obs_list->size;

  size = sizeof( struct observation ) * h_obs_list->size;
  cudaMalloc( (void **) &d_obs_list.observations, size );
 
  int i;
  for ( i = 0 ; i < h_obs_list->size ; i++ )
  {
    struct observation *h_obs = (h_obs_list->observations) + i;
    struct observation *d_obs = (d_obs_list->observations) + i;

    d_obs->type = h_obs->type;
    d_obs->time = h_obs->time;
    d_obs->x_pos = h_obs->x_pos;
    d_obs->y_pos = h_obs->y_pos;
    d_obs->value = h_obs->value
    d_obs->type = h_obs->type;
      // observation_type (determines how value is interpreted)
  int type;
  // the timestamp of the observation
  float time;
  // x position of sensor
  float x_pos;
  // y position of sensor
  float y_pos;
  // observation value
  float value;
  // observation error
  float error;
  }
}
*/

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






