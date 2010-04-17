#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "obs_math.h"
#include "convert.h"
#include "filter_io.h"
#include "filter_cuda_kernels.h"
#include "filter_cuda_data.h"
#include "filter_cuda_util.h"
#include "filter_constants.h"

// inititalize random seeds
// cuda kernels do not have access to c library rand function
void h_init_seed( struct particles *host, int num )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    host[i].seed = rand();
  }
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // read waypoint lists for sensor and target
  struct waypoint_list *waypoints1 = read_waypoints( "data/waypoints1.txt" );
  struct waypoint_list *waypoints2 = read_waypoints( "data/waypoints2.txt" );

  printf("Sensor Waypoints:\n");
  print_waypoints( waypoints1 );

  printf("Target Waypoints:\n");
  print_waypoints( waypoints2 );

  // generate errored range and azimuth observations based on the waypoints
  //struct observation_list *range_obs_list = generate_observations( waypoints1, waypoints2, 2, 100, 0.0, 500.0, 2000.0 );
  //struct observation_list *azimuth_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 2000.0 );
  //struct observation_list *h_obs_list = combine_observations( range_obs_list, azimuth_obs_list );

  struct observation_list *h_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 100.0 );

  printf("Observations:\n");
  print_observations( h_obs_list );

  // initialize particle memory
  struct particles *d_particle_list = d_init_particle_mem( NUM_PARTICLES );
  struct particles *h_particle_list = h_init_particle_mem( NUM_PARTICLES );

  // initialize temporary weight array
  float *weights = ( float * ) malloc( sizeof( float ) * ( NUM_PARTICLES / THREADS_PER_BLOCK ) );

  // initialize random seeds for each particle using host random number generator
  h_init_seed( h_particle_list, NUM_PARTICLES );

  // copy particles to device
  copy_particles_host_to_device( d_particle_list, h_particle_list, NUM_PARTICLES );

  // initialize particle positions 
  init_particles( d_particle_list, NUM_PARTICLES );

  int i;
  float previous_time = 0.0;
  float current_time = 0.0;
  for ( i = 0 ; i < h_obs_list->size ; i++ )
  {
    printf("observation %d\n", i);

    struct observation *obs = (h_obs_list->observations) + i;
    previous_time = current_time;
    current_time = obs->time;
    float diff_time = current_time - previous_time;
    time_update( d_particle_list, NUM_PARTICLES, diff_time, MEAN_MANEUVER_TIME );
    information_update( obs, d_particle_list, NUM_PARTICLES );
    float weight_sum = sum_weight( d_particle_list, weights, NUM_PARTICLES );
    //resample2( NUM_PARTICLES );
  }

  // copy particles back to host
  copy_particles_device_to_host( h_particle_list, d_particle_list, NUM_PARTICLES );

  write_particles( h_particle_list, OUTPUT_NAME, NUM_PARTICLES, 10 );
  print_particles( h_particle_list, NUM_PARTICLES, 1000 );

  // free host and device memory
  h_free_particle_mem( h_particle_list );
  d_free_particle_mem( d_particle_list );
}
