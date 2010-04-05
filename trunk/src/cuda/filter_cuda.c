
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "obs_math.h"
#include "filter_io.h"
#include "convert.h"

#include "cuda_util.h"
#include "cuda_constants.h"

void h_init_particle_mem( int );
void h_free_particle_mem( );
void h_init_seed( int num );

void print_particles( int, int );
void print_particle( int );

void checkCUDAError( const char* );

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
  init_particles( numBlocks, numThreadsPerBlock, d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed );

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

// prints a single particle's information to the console
void print_particle( int i )
{
  printf( "%f\t%f\t%f\t%f\t%f\n", h_x_pos[i], h_y_pos[i], h_x_vel[i], h_y_vel[i], h_weight[i] );
}
