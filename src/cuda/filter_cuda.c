#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "observation.h"
#include "convert.h"
#include "waypoint.h"
#include "filter_cuda_kernels.h"
#include "filter_cuda_util.h"
#include "filter_constants.h"
#include "filter_cuda_data.h"


/////////////////////////////////////////////////////
// filter_cuda.c
//
// Entry point for filter_cuda executable. This code
// performs a single particle filter scenario. The
// paths that the sensor and target follow are defined
// in waypoint files as described in src/common/waypoint.c.
//
// Observations are randomly generated from the waypoints
// as described in src/common/observation.c.
//
// After all observations have been processed, the particles
// and the final positions of the target and sensor are
// written to output files in the data directory in a format
// which can be displayed by the plot_particles.sh script.
/////////////////////////////////////////////////////


// uncomment to perform effective particle count calculation before resampling
//#define CALC_EFFECTIVE_COUNT

// host and device arrays and temporary device array
// for operations that require double buffering/swapping of particle data
struct particles *host_particles;
struct particles *device_particles;
struct particles *device_particles_swap;

// inititalize random seeds
// cuda kernels do not have access to c library rand function
void h_init_seed( )
{
  int i;

  for ( i = 0 ; i < NUM_PARTICLES ; i++ )
  {
    host_particles->seed[i] = rand();
  }
}

// double buffering strategy is used since resampling is not performed in place
void swap_device_arrays( )
{
  struct particles *temp_particles = device_particles; 
  device_particles = device_particles_swap;
  device_particles_swap = temp_particles;
}

// initialize particle memory on host
void h_init_particles( )
{
  host_particles = h_init_particles_mem( );

  host_particles->x_pos = h_init_array_mem( NUM_PARTICLES );
  host_particles->y_pos = h_init_array_mem( NUM_PARTICLES );
  host_particles->x_vel = h_init_array_mem( NUM_PARTICLES );
  host_particles->y_vel = h_init_array_mem( NUM_PARTICLES );
  host_particles->weight = h_init_array_mem( NUM_PARTICLES );
  host_particles->seed = h_init_array_mem( NUM_PARTICLES );
}

// initialize particle memory on device
void d_init_particles( )
{
  device_particles = h_init_particles_mem( );

  d_init_array_mem( &(device_particles->x_pos), NUM_PARTICLES );
  d_init_array_mem( &(device_particles->y_pos), NUM_PARTICLES );
  d_init_array_mem( &(device_particles->x_vel), NUM_PARTICLES );
  d_init_array_mem( &(device_particles->y_vel), NUM_PARTICLES );
  d_init_array_mem( &(device_particles->weight), NUM_PARTICLES );
  d_init_array_mem( &(device_particles->seed), NUM_PARTICLES );

  device_particles_swap = h_init_particles_mem( );

  d_init_array_mem( &(device_particles_swap->x_pos), NUM_PARTICLES );
  d_init_array_mem( &(device_particles_swap->y_pos), NUM_PARTICLES );
  d_init_array_mem( &(device_particles_swap->x_vel), NUM_PARTICLES );
  d_init_array_mem( &(device_particles_swap->y_vel), NUM_PARTICLES );
  d_init_array_mem( &(device_particles_swap->weight), NUM_PARTICLES );
  d_init_array_mem( &(device_particles_swap->seed), NUM_PARTICLES );
}

// free particle memory on host
void h_free_particles( )
{
  h_free_mem( host_particles->x_pos );
  h_free_mem( host_particles->y_pos );
  h_free_mem( host_particles->x_vel );
  h_free_mem( host_particles->y_vel );
  h_free_mem( host_particles->weight );
  h_free_mem( host_particles->seed );

  h_free_mem( host_particles );
}

// free particle memory on device
void d_free_particles( )
{
  d_free_mem( device_particles->x_pos );
  d_free_mem( device_particles->y_pos );
  d_free_mem( device_particles->x_vel );
  d_free_mem( device_particles->y_vel );
  d_free_mem( device_particles->weight );
  d_free_mem( device_particles->seed );

  d_free_mem( device_particles );

  d_free_mem( device_particles_swap->x_pos );
  d_free_mem( device_particles_swap->y_pos );
  d_free_mem( device_particles_swap->x_vel );
  d_free_mem( device_particles_swap->y_vel );
  d_free_mem( device_particles_swap->weight );
  d_free_mem( device_particles_swap->seed );

  d_free_mem( device_particles_swap );
}

// copy particles from host to device
void copy_particles_host_to_device( )
{
  copy_array_host_to_device( host_particles->x_pos, device_particles->x_pos, NUM_PARTICLES );
  copy_array_host_to_device( host_particles->y_pos, device_particles->y_pos, NUM_PARTICLES );
  copy_array_host_to_device( host_particles->x_vel, device_particles->x_vel, NUM_PARTICLES );
  copy_array_host_to_device( host_particles->y_vel, device_particles->y_vel, NUM_PARTICLES );
  copy_array_host_to_device( host_particles->weight, device_particles->weight, NUM_PARTICLES );
  copy_array_host_to_device( host_particles->seed, device_particles->seed, NUM_PARTICLES );
}

// copy particles from device to host
void copy_particles_device_to_host( )
{
  copy_array_device_to_host( host_particles->x_pos, device_particles->x_pos, NUM_PARTICLES );
  copy_array_device_to_host( host_particles->y_pos, device_particles->y_pos, NUM_PARTICLES );
  copy_array_device_to_host( host_particles->x_vel, device_particles->x_vel, NUM_PARTICLES );
  copy_array_device_to_host( host_particles->y_vel, device_particles->y_vel, NUM_PARTICLES );
  copy_array_device_to_host( host_particles->weight, device_particles->weight, NUM_PARTICLES );
  copy_array_device_to_host( host_particles->seed, device_particles->seed, NUM_PARTICLES );
}

int main( int argc, char** argv )
{
  // initialize random seed
  srand( time( NULL ) );

  // read waypoint lists for sensor and target
  struct waypoint_list *waypoints1 = read_waypoints( "data/waypoints3.txt" );
  struct waypoint_list *waypoints2 = read_waypoints( "data/waypoints4.txt" );

  printf("Sensor Waypoints:\n");
  print_waypoints( waypoints1 );

  printf("Target Waypoints:\n");
  print_waypoints( waypoints2 );

  // generate errored range and azimuth observations based on the waypoints
  // this example combines two lists of observations into a single observation list
/*
  struct observation_list *range_obs_list = generate_observations( waypoints1, waypoints2, 2, 100, 0.0, 500.0, 2000.0 );
  struct observation_list *azimuth_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 2000.0 );
  struct observation_list *h_obs_list = combine_observations( range_obs_list, azimuth_obs_list );
*/

  // generate a single list of errored azimuth observations
  struct observation_list *h_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 4000.0 );

  printf("Observations:\n");
  print_observations( h_obs_list );

  // initialize particle memory
  h_init_particles( );
  d_init_particles( );

  // initialize random seeds for each particle using host random number generator
  h_init_seed( );

  // copy particles to device
  copy_particles_host_to_device( );

  // initialize particle positions 
  init_particles( *device_particles, NUM_PARTICLES );

  int i;
  float previous_time = 0.0;
  float current_time = 0.0;
  for ( i = 0 ; i < h_obs_list->size ; i++ )
  {
    printf("observation %d\n", i);

    // get the next observation from the list
    struct observation *obs = (h_obs_list->observations) + i;
    
    // calculate how much time has passed since the last observation
    previous_time = current_time;
    current_time = obs->time;
    float diff_time = current_time - previous_time;

    // time updated the particles to the time of the new observation
    time_update( *device_particles, NUM_PARTICLES, diff_time, MEAN_MANEUVER_TIME );

    // adjust particle weights based on the observation (using a likelihood function)
    information_update( *obs, *device_particles, NUM_PARTICLES );

    // remove particles with low weights and replace them with perturbed copies of particles with higher weights
    // if CALC_EFFECTIVE_COUNT is indefined, this is done for every observation, otherwise resampling is only
    // performed when the calculated effective number of particles drops below a threshold defined by NUM_EFFECT_CUTOFF
    float effective_count = 0;
#ifdef CALC_EFFECTIVE_COUNT
    effective_count = calc_effective_particle_count( *device_particles, *device_particles_swap, NUM_PARTICLES);
#endif
    if ( effective_count < NUM_EFFECT_CUTOFF )
    {
      resample_v2( *device_particles, *device_particles_swap, NUM_PARTICLES );
      swap_device_arrays( );
    }
  }

  // copy particles back to host
  copy_particles_device_to_host( );

  // write final sensor and target positions and particle values to output files
  write_positions( OWNSHIP_POS_NAME, waypoints1, current_time );
  write_positions( TARGET_POS_NAME, waypoints2, current_time );
  write_particles( host_particles, OUTPUT_NAME, NUM_PARTICLES, 500 );

  // free host and device memory
  h_free_particles( );
  d_free_particles( );

  return 0;
}
