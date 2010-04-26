#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "obs_math.h"
#include "convert.h"
#include "filter_io.h"
#include "filter_cuda_kernels.h"
#include "filter_cuda_util.h"
#include "filter_constants.h"
#include "filter_cuda_data.h"

// host particle arrays

float *h_x_pos; // meters
float *h_y_pos; // meters
float *h_x_vel; // meters/second
float *h_y_vel; // meters/second
float *h_weight;
float *h_seed; // random seed for particle

// device particle arrays
float *d_x_pos; // meters
float *d_y_pos; // meters
float *d_x_vel; // meters/second
float *d_y_vel; // meters/second
float *d_weight;
float *d_seed; // random seed for particle

// swap arrays -- double buffering strategy
// is used since resampling is not performed in place
float *d_x_pos_swap; // meters
float *d_y_pos_swap; // meters
float *d_x_vel_swap; // meters/second
float *d_y_vel_swap; // meters/second
float *d_weight_swap;
float *d_seed_swap; // random seed for particle

// temporary device array for calculating sums
float *d_temp_weight1;
float *d_temp_weight2;

// inititalize random seeds
// cuda kernels do not have access to c library rand function
void h_init_seed( )
{
  int i;

  for ( i = 0 ; i < NUM_PARTICLES ; i++ )
  {
    h_seed[i] = rand();
  }
}

// double buffering strategy is used since resampling is not performed in place
void swap_device_arrays( )
{
  float *d_x_pos_temp = d_x_pos;
  float *d_y_pos_temp = d_y_pos;
  float *d_x_vel_temp = d_x_vel;
  float *d_y_vel_temp = d_y_vel;
  float *d_weight_temp = d_weight;
  float *d_seed_temp = d_seed;

  d_x_pos = d_x_pos_swap;
  d_y_pos = d_y_pos_swap;
  d_x_vel = d_x_vel_swap;
  d_y_vel = d_y_vel_swap;
  d_weight = d_weight_swap;
  d_seed = d_seed_swap;

  d_x_pos_swap =d_x_pos_temp;
  d_y_pos_swap =d_y_pos_temp;
  d_x_vel_swap =d_x_vel_temp;
  d_y_vel_swap =d_y_vel_temp;
  d_weight_swap =d_weight_temp;
  d_seed_swap =d_seed_temp;
}

void h_init_particles( )
{
  h_x_pos = h_init_array_mem( NUM_PARTICLES );
  h_y_pos = h_init_array_mem( NUM_PARTICLES );
  h_x_vel = h_init_array_mem( NUM_PARTICLES );
  h_y_vel = h_init_array_mem( NUM_PARTICLES );
  h_weight = h_init_array_mem( NUM_PARTICLES );
  h_seed = h_init_array_mem( NUM_PARTICLES );
}

void d_init_particles( )
{
  d_x_pos = d_init_array_mem( NUM_PARTICLES );
  d_y_pos = d_init_array_mem( NUM_PARTICLES );
  d_x_vel = d_init_array_mem( NUM_PARTICLES );
  d_y_vel = d_init_array_mem( NUM_PARTICLES );
  d_weight = d_init_array_mem( NUM_PARTICLES );
  d_seed = d_init_array_mem( NUM_PARTICLES );

  d_x_pos_swap = d_init_array_mem( NUM_PARTICLES );
  d_y_pos_swap = d_init_array_mem( NUM_PARTICLES );
  d_x_vel_swap = d_init_array_mem( NUM_PARTICLES );
  d_y_vel_swap = d_init_array_mem( NUM_PARTICLES );
  d_weight_swap = d_init_array_mem( NUM_PARTICLES );
  d_seed_swap = d_init_array_mem( NUM_PARTICLES );
}

void h_free_particles( )
{
  h_free_particle_mem( h_x_pos );
  h_free_particle_mem( h_y_pos );
  h_free_particle_mem( h_x_vel );
  h_free_particle_mem( h_y_vel );
  h_free_particle_mem( h_weight );
  h_free_particle_mem( h_seed );
}

void d_free_particles( )
{
  d_free_particle_mem( d_x_pos );
  d_free_particle_mem( d_y_pos );
  d_free_particle_mem( d_x_vel );
  d_free_particle_mem( d_y_vel );
  d_free_particle_mem( d_weight );
  d_free_particle_mem( d_seed );

  d_free_particle_mem( d_x_pos_swap );
  d_free_particle_mem( d_y_pos_swap );
  d_free_particle_mem( d_x_vel_swap );
  d_free_particle_mem( d_y_vel_swap );
  d_free_particle_mem( d_weight_swap );
  d_free_particle_mem( d_seed_swap );
}

void copy_particles_host_to_device( )
{
  copy_array_host_to_device( h_x_pos, d_x_pos, NUM_PARTICLES );
  copy_array_host_to_device( h_y_pos, d_y_pos, NUM_PARTICLES );
  copy_array_host_to_device( h_x_vel, d_x_vel, NUM_PARTICLES );
  copy_array_host_to_device( h_y_vel, d_y_vel, NUM_PARTICLES );
  copy_array_host_to_device( h_weight, d_weight, NUM_PARTICLES );
  copy_array_host_to_device( h_seed, d_seed, NUM_PARTICLES );
}

void copy_particles_device_to_host( )
{
  copy_array_device_to_host( h_x_pos, d_x_pos, NUM_PARTICLES );
  copy_array_device_to_host( h_y_pos, d_y_pos, NUM_PARTICLES );
  copy_array_device_to_host( h_x_vel, d_x_vel, NUM_PARTICLES );
  copy_array_device_to_host( h_y_vel, d_y_vel, NUM_PARTICLES );
  copy_array_device_to_host( h_weight, d_weight, NUM_PARTICLES );
  copy_array_device_to_host( h_seed, d_seed, NUM_PARTICLES );
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

  struct observation_list *h_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 0.0 );

  printf("Observations:\n");
  print_observations( h_obs_list );

  // initialize particle memory
  h_init_particles( );
  d_init_particles( );

  // initialize temporary weight array on device and host
  d_temp_weight1 = d_init_array_mem( NUM_PARTICLES );
  d_temp_weight2 = d_init_array_mem( NUM_PARTICLES );

  // initialize random seeds for each particle using host random number generator
  h_init_seed( );

  // copy particles to device
  copy_particles_host_to_device( );

  // initialize particle positions 
  init_particles( d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed, NUM_PARTICLES );

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
    time_update( d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed, NUM_PARTICLES, diff_time, MEAN_MANEUVER_TIME );
    
    // adjust particle weights based on the observation (using a likelihood function)
    information_update( obs, d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed, NUM_PARTICLES );

    // remove particles with low weights and replace them with perturbed copies of particles with higher weights
    resample( d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed,
              d_x_pos_swap, d_y_pos_swap, d_x_vel_swap, d_y_vel_swap, d_weight_swap, d_seed_swap,
              NUM_PARTICLES );

    swap_device_arrays( );
  }

  // copy particles back to host
  copy_particles_device_to_host( );

  write_particles( h_x_pos, h_y_pos, h_x_vel, h_y_vel, h_weight, h_seed, OUTPUT_NAME, NUM_PARTICLES, 20 );
  //print_particles( d_x_pos, d_y_pos, d_x_vel, d_y_vel, d_weight, d_seed,, NUM_PARTICLES, 1000 );

  // free host and device memory
  h_free_particles( );
  d_free_particles( );
}
