
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "obs_math.h"
#include "device_filter_math.h"
#include "filter_io.h"
#include "convert.h"

#define NUM_PARTICLES 100000
#define MAX_RANGE 20000 // meters
#define MAX_VEL 15 // meters per second

#define MAX_POS_PERTURB 1000.0 // meters
#define MAX_VEL_PERTURB    2.0 // meters per second

#define MEAN_MANEUVER_TIME 3600 // seconds

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

// particles in host memory
float *h_x_pos; // meters
float *h_y_pos; // meters
float *h_x_vel; // meters/second
float *h_y_vel; // meters/second
float *h_weight;
float *h_seed; // random seed for particle

// particles in device memory
float *d_x_pos; // meters
float *d_y_pos; // meters
float *d_x_vel; // meters/second
float *d_y_vel; // meters/second
float *d_weight;
float *d_seed; // random seed for particle

// functions from filter_math.h
__device__ float d_frand0( float max );
__device__ float d_frand( float min, float max );

void h_init_particle_mem( int );
void h_free_particle_mem( );
void h_init_seed( int num );

void d_init_particle_mem( int );
void d_free_particle_mem( );

void copy_particles_device_to_host( int );
void copy_particles_host_to_device( int );

void print_particles( int, int );
void print_particle( int );

void checkCUDAError( const char* );

// CUDA kernel function : initialize a particle
__global__ void init_particle_val( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  int seed = d_seed[index];

  d_x_pos[index]  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  d_y_pos[index]  = device_frand( seed, -MAX_RANGE, MAX_RANGE );
  seed = device_lcg_rand( seed );
  d_x_vel[index]  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  d_y_vel[index]  = device_frand( seed, -MAX_VEL, MAX_VEL );
  seed = device_lcg_rand( seed );
  d_weight[index] = 1.0;

  d_seed[index] = seed;
}


int main( int argc, char** argv )
{
  srand( time( NULL ) );

  struct waypoint_list *waypoints1 = read_waypoints( "data/waypoints1.txt" );
  struct waypoint_list *waypoints2 = read_waypoints( "data/waypoints2.txt" );

  printf("Sensor Waypoints:\n");
  print_waypoints( waypoints1 );

  printf("Target Waypoints:\n");
  print_waypoints( waypoints2 );

  struct observation_list *range_obs_list = generate_observations( waypoints1, waypoints2, 2, 100, 0.0, 500.0, 2000.0 );
  struct observation_list *azimuth_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 2000.0 );
  struct observation_list *obs_list = combine_observations( range_obs_list, azimuth_obs_list );
  printf("Observations:\n");
  print_observations( obs_list );

  h_init_particle_mem( NUM_PARTICLES );

  // we could init the particle seeds from a function of their thread index
  // but we should avoid that kind of linear dependence between the seeds
  // so initialize the seeds on the host and copy them to the device
  h_init_seed( NUM_PARTICLES );

  d_init_particle_mem( NUM_PARTICLES );

  // copy the seeds generated on the host to the device
  copy_particles_host_to_device( NUM_PARTICLES );

  // define grid and block size
  int numThreadsPerBlock = 256;

  // compute number of blocks needed based on array size and desired block size
  int numBlocks = NUM_PARTICLES / numThreadsPerBlock;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(numThreadsPerBlock);
  init_particle_val<<< dimGrid, dimBlock >>>( d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed );

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("init_particle_val");

  print_particles( NUM_PARTICLES , 1 );

  copy_particles_device_to_host( NUM_PARTICLES );

  print_particles( NUM_PARTICLES , 1 );
}

void h_init_seed( int num )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    h_seed[i] = rand();
  }
}

void copy_particles_host_to_device( int num )
{
  int size = sizeof( float ) * num;

  cudaMemcpy( h_x_pos, d_x_pos, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_y_pos, d_y_pos, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_x_vel, d_x_vel, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_y_vel, d_y_vel, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_weight, d_weight, size, cudaMemcpyHostToDevice );
  cudaMemcpy( h_seed, d_seed, size, cudaMemcpyHostToDevice );
}

void copy_particles_device_to_host( int num )
{
  int size = sizeof( float ) * num;

  cudaMemcpy( h_x_pos, d_x_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_pos, d_y_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_x_vel, d_x_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_vel, d_y_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_weight, d_weight, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_seed, d_seed, size, cudaMemcpyDeviceToHost );
}

// allocate memory for num particles on host
void h_init_particle_mem( int num )
{
  int size = sizeof( float ) * num;

  h_x_pos  = ( float* ) malloc( size );
  h_y_pos  = ( float* ) malloc( size );
  h_x_vel  = ( float* ) malloc( size );
  h_y_vel  = ( float* ) malloc( size );
  h_weight = ( float* ) malloc( size );
  h_seed = ( float* ) malloc( size );
}

// allocate memory for num particles on device
void d_init_particle_mem( int num )
{
  int size = sizeof( float ) * num;

  cudaMalloc( (void **) &d_x_pos, size );
  cudaMalloc( (void **) &d_y_pos, size );
  cudaMalloc( (void **) &d_x_vel, size );
  cudaMalloc( (void **) &d_y_vel, size );
  cudaMalloc( (void **) &d_weight, size );
  cudaMalloc( (void **) &d_seed, size );
}

// free particle memory on host
void h_free_particle_mem( )
{
  free( h_x_pos );
  free( h_y_pos );
  free( h_x_vel );
  free( h_y_vel );
  free( h_weight );
  free( h_seed );
}

// free particle memory on device
void d_free_particle_mem( )
{
  cudaFree( d_x_pos );
  cudaFree( d_y_pos );
  cudaFree( d_x_vel );
  cudaFree( d_y_vel );
  cudaFree( d_weight );
  cudaFree( d_seed );
}

// function adapted from CUDA Technical Training
void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) 
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(EXIT_FAILURE);
    }                         
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( char* out_name, int num, int downsample )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", h_x_pos[i], h_y_pos[i], h_x_vel[i], h_y_vel[i], h_weight[i] );
  }

  int success = fclose( file );
}

// prints information on all particles to the console
void print_particles( int num, int downsample )
{
  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    print_particle( i );
  }
}









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
  return ( float ) seed / ( float ) RAND_MAX * max ;
}

// return a random float value evenly distributed between min and max
__device__ float device_frand( int seed, float min, float max )
{
  float diff = max - min;
  return device_frand0( seed, diff ) + min;
}

// return an exponentially distributed random float
__device__ float device_erand( int seed, float inv_lambda )
{
  return -log( device_frand0( seed, 1.0 ) ) * inv_lambda;
}

// return a float value chosen from the normal distribution
// algorithm from polar method of G. E. P. Box, M. E. Muller, and G. Marsaglia, 
// as described by Donald E. Knuth in The Art of Computer Programming,
// Volume 2: Seminumerical Algorithms, section 3.4.1, subsection C, algorithm P
// also the implementation used by java.util.Random.nextGaussian
__device__ float device_grand0( int seed1, int seed2 )
{
  float v1, v2, s;
  
  do
  { 
    v1 = device_frand(seed1,-1,1);
    v2 = device_frand(seed2,-1,1);
    s = v1 * v1 + v2 * v2;
  }
  while ( s >= 1 || s == 0 );
  
  float multiplier = sqrtf(-2 * logf(s)/s);
  
  return v1 * multiplier;
}

// returns a random float value from a gaussian distribution
// with the given mean and sigma
__device__ float device_grand( int seed1, int seed2, float mean, float sigma )
{
  return mean + sigma * device_grand0( seed1, seed2 );
}

// calculates the probability density function for the gaussian
// distribution with given mean and sigma
__device__ float device_gvalue( float value, float mean, float sigma )
{
  float z = ( value - mean ) / sigma ;
  return expf( -0.5 * z * z ) / ( sqrtf( 2.0 * DEVICE_PI ) * sigma );
}
