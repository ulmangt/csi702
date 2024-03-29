#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "observation.h"
#include "waypoint.h"
#include "convert.h"

#include "filter_math.h"
#include "filter_constants.h"

/////////////////////////////////////////////////////
// filter_serial.c
//
// Entry point for filter_serial executable. This code
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

float *x_pos; // meters
float *y_pos; // meters
float *x_vel; // meters/second
float *y_vel; // meters/second
float *weight;

void init_particle_mem( int );
void init_particle_val( int, float, float );
void free_particle_mem( );

void time_update( int, float, float );
void time_update_index( int, float );
void maneuver_index( int );

void information_update( int, struct observation* );

void resample( int );
void mark_particle( int, int );
void copy_particle( int, int, float );
void perturb_particle( int );

float calc_effective_particle_count( int );

void write_positions( char*, struct waypoint_list*, float );
void write_particles( char*, int, int );

void print_particles( int, int );
void print_particle( int );

int main( int argc, char* argv )
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
  //struct observation_list *range_obs_list = generate_observations( waypoints1, waypoints2, 2, 100, 0.0, 500.0, 2000.0 );
  //struct observation_list *azimuth_obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 2000.0 );
  //struct observation_list *obs_list = combine_observations( range_obs_list, azimuth_obs_list );
  
  struct observation_list *obs_list = generate_observations( waypoints1, waypoints2, 1, fromDegrees(8.0), 0.0, 100.0, 4000.0 );

  printf("Observations:\n");
  print_observations( obs_list );

  // allocate particle memory and initialize state
  init_particle_mem( NUM_PARTICLES );
  init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );

  int i;
  float previous_time = 0.0;
  float current_time = 0.0;
  for ( i = 0 ; i < obs_list->size ; i++ )
  {
    printf("observation %d\n", i);

    struct observation *obs = (obs_list->observations) + i;
    previous_time = current_time;
    current_time = obs->time;
    time_update( NUM_PARTICLES, current_time - previous_time, MEAN_MANEUVER_TIME );
    information_update( NUM_PARTICLES, obs );
    resample( NUM_PARTICLES );
  }

  write_positions( OWNSHIP_POS_NAME, waypoints1, current_time );
  write_positions( TARGET_POS_NAME, waypoints2, current_time );
  write_particles( OUTPUT_NAME, NUM_PARTICLES, 1000 );

  free_particle_mem( );
}

// allocate memory for num particles
void init_particle_mem( int num )
{
  int size = sizeof( float ) * num;

  x_pos  = ( float* ) malloc( size );
  y_pos  = ( float* ) malloc( size );
  x_vel  = ( float* ) malloc( size );
  y_vel  = ( float* ) malloc( size );
  weight = ( float* ) malloc( size );
}

void free_particle_mem( )
{
  free( x_pos );
  free( y_pos );
  free( x_vel );
  free( y_vel );
  free( weight );
}

// provides an estimate of the total number of particles
// which are actually contributing information to the distribution
// see: http://en.wikipedia.org/wiki/Particle_filter#Sampling_Importance_Resampling_.28SIR.29
float calc_effective_particle_count( int num )
{
    int j;
    // sum particle weights
    float weight_sum = 0.0;
    for ( j = 0 ; j < num ; j++ )
    {
      weight_sum += weight[j];
    }

    float sum_normalized_squared = 0.0;
    for ( j = 0 ; j < num ; j++ )
    {
      float norm_weight = weight[j] / weight_sum;
      sum_normalized_squared += norm_weight * norm_weight;
    }

    return 1.0 / sum_normalized_squared;
}

// initialize positions, velocities, and weights of num particles
void init_particle_val( int num, float max_range, float max_vel )
{
  float particle_weight = 1.0;

  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    x_pos[i]  = frand( -max_range, max_range );
    y_pos[i]  = frand( -max_range, max_range );
    x_vel[i]  = frand( -max_vel, max_vel );
    y_vel[i]  = frand( -max_vel, max_vel );
    weight[i] = particle_weight;
  }
}

// time update the particles
// adjust positions based on velocity
// velocity changes occur exponentially distributed in time
// num number of particles to update
// seconds number of seconds into future to update particles
// mean_maneuver the mean time between particle maneuvers (exponentially distributed)
void time_update( int num, float seconds, float mean_maneuver )
{
  int i;

  float remaining_time;
  float next_maneuver = erand( mean_maneuver );

  for ( i = 0 ; i < num ; i++ )
  {
    remaining_time = seconds;

    while ( 1 )
    {
      if ( next_maneuver < remaining_time )
      {
        time_update_index( i, next_maneuver );
        maneuver_index( i );
        remaining_time -= next_maneuver;
        next_maneuver = erand( mean_maneuver );
      }
      else
      {
        time_update_index( i, remaining_time );
        next_maneuver -= remaining_time;
        break;
      }
    }
  }
}

// time updates a single particle with no maneuver
void time_update_index( int i, float seconds )
{
  x_pos[i] = x_pos[i] + x_vel[i] * seconds;
  y_pos[i] = y_pos[i] + y_vel[i] * seconds;
}

// maneuver (adjust the velocity) of a single particle
// this is an instantanious change (no time elapses)
void maneuver_index( int i )
{
  x_vel[i] = x_vel[i] + frand( -MAX_VEL_PERTURB, MAX_VEL_PERTURB );
  y_vel[i] = y_vel[i] + frand( -MAX_VEL_PERTURB, MAX_VEL_PERTURB );
}



void information_update( int num, struct observation *obs )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    weight[i] = apply_observation( obs, x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
  }
}

void resample( int num )
{
  int i;
  // sum particle weights
  float weight_sum = 0.0;
  for ( i = 0 ; i < num ; i++ )
  {
    weight_sum += weight[i];
  }

  // weight of an average particle
  float wcutoff_increment = num / weight_sum;
  // running total of weight
  float wsum = 0.0;
  // running total of resampled particles
  int rsum = 0;
  // running total of discarded particles
  int dsum = 0;

  // replace each particle weight with the number of times the particle should be duplicated
  // this is essentially equal to weight * ( num / weight_sum )
  // the added complexity deals with accumulating fractional weights and assigning them to a particle
  for ( i = 0 ; i < num ; i++ )
  {
    wsum += wcutoff_increment * weight[i];
    int r = (int) floor(wsum - rsum + 0.5);

    if ( r < 0 )
      r = 0;

    weight[i] = r;
    rsum += r;

    if ( r == 0 )
      dsum++;
  }

  // overwrite the particles with weight 0 with copies of particles with weight > 0
  int overwrite = -1;
  for ( i = 0 ; i < num ; i++ )
  {
    if ( weight[i] <= 1 )
      continue;

    int j;
    for ( j = 0 ; j < weight[i] - 1 ; j++ )
    {
      do {
        overwrite++;
      }
      while ( overwrite < num && weight[overwrite] != 0 );
      
      if ( overwrite >= num ) break;

      copy_particle( i , overwrite , 1.0 );
      perturb_particle( overwrite );
    }

    weight[i] = 1.0;
  }
}

void copy_particle( int source_index, int target_index, float particle_weight )
{
  x_pos[target_index]  = x_pos[source_index];
  y_pos[target_index]  = y_pos[source_index];
  x_vel[target_index]  = x_vel[source_index];
  y_vel[target_index]  = y_vel[source_index];
  weight[target_index] = particle_weight;
}

void perturb_particle( int index )
{
  x_pos[index] += frand( -MAX_POS_PERTURB, MAX_POS_PERTURB );
  y_pos[index] += frand( -MAX_POS_PERTURB, MAX_POS_PERTURB );
  x_vel[index] += frand( -MAX_VEL_PERTURB, MAX_VEL_PERTURB );
  y_vel[index] += frand( -MAX_VEL_PERTURB, MAX_VEL_PERTURB );
}

// writes the interpolated x and y positions at the given time along the waypoints list
// this is used to record the position of the sensor and ownship at the end of the scenario
// for graphical output purposes 
void write_positions( char* out_name, struct waypoint_list *waypoints, float time )
{
  float x_pos, y_pos;
  interpolate( waypoints , time , &x_pos, &y_pos );
  
  FILE* file = fopen( out_name, "w" );
  fprintf( file, "%f\t%f\n", x_pos, y_pos );

  int success = fclose( file );
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( char* out_name, int num, int downsample )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
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
  printf( "%f\t%f\t%f\t%f\t%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
}
