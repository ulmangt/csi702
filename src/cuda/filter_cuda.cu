
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "obs_math.h"
#include "filter_math.h"
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

// particles in device memory
float *d_x_pos; // meters
float *d_y_pos; // meters
float *d_x_vel; // meters/second
float *d_y_vel; // meters/second
float *d_weight;

void h_init_particle_mem( int );
void h_free_particle_mem( );

void d_init_particle_mem( int );
void d_free_particle_mem( );

void copy_particles_device_to_host( int );

void print_particles( int, int );
void print_particle( int );

void checkCUDAError( const char* );

// CUDA kernel function : initialize a particle
__global__ void init_particle_val( )
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  d_x_pos[index]  = frand( -MAX_RANGE, MAX_RANGE );
  d_y_pos[index]  = frand( -MAX_RANGE, MAX_RANGE );
  d_x_vel[index]  = frand( -MAX_VEL, MAX_VEL );
  d_y_vel[index]  = frand( -MAX_VEL, MAX_VEL );
  d_weight[index] = 1.0;
}


int main( int argc, char* argv )
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

  // define grid and block size
  int numThreadsPerBlock = 256;

  // compute number of blocks needed based on array size and desired block size
  int numBlocks = NUM_PARTICLES / numThreadsPerBlock;

  // launch kernel
  dim3 dimGrid(numBlocks);
  dim3 dimBlock(numThreadsPerBlock);
  init_particle_val<<< dimGrid, dimBlock >>>( );

  // block until the device has completed kernel execution
  cudaThreadSynchronize();

  // check if the init_particle_val kernel generated errors
  checkCUDAError("init_particle_val");

  print_particles( NUM_PARTICLES , 1 );

  copy_particles_device_to_host( NUM_PARTICLES );

  print_particles( NUM_PARTICLES , 1 );
}

void copy_particles_device_to_host( int num )
{
  int size = sizeof( float ) * num;

  cudaMemcpy( h_x_pos, d_x_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_pos, d_y_pos, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_x_vel, d_x_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_y_vel, d_y_vel, size, cudaMemcpyDeviceToHost );
  cudaMemcpy( h_weight, d_weight, size, cudaMemcpyDeviceToHost );
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
}

// free particle memory on host
void h_free_particle_mem( )
{
  free( h_x_pos );
  free( h_y_pos );
  free( h_x_vel );
  free( h_y_vel );
  free( h_weight );
}

// free particle memory on device
void d_free_particle_mem( )
{
  cudaFree( d_x_pos );
  cudaFree( d_y_pos );
  cudaFree( d_x_vel );
  cudaFree( d_y_vel );
  cudaFree( d_weight );
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
