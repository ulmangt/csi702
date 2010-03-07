#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obs_math.h"
#include "filter_math.h"
#include "filter_io.h"
#include "convert.h"

#define NUM_PARTICLES 10
#define MAX_RANGE 8000
#define MAX_VEL 15

#define MEAN_MANEUVER_TIME 3600

#define INITIAL_MAX_WAYPOINTS 10

#define OUTPUT_NAME "particles.out"

float *x_pos; // meters
float *y_pos; // meters
float *x_vel; // meters/second
float *y_vel; // meters/second
float *weight;

void init_particle_mem( int );
void init_particle_val( int, float, float );

void time_update( int, float, float );
void time_update_index( int, float );
void maneuver_index( int );

void write_particles( char*, int );

void print_particles( int );
void print_particle( int );

int main( int argc, char* argv )
{
  struct waypoint_list *waypoints = read_waypoints( "data/waypoints1.txt" );
  print_waypoints( waypoints );
  float x_pos_interp, y_pos_interp, time;

  int i;

  /*
     for ( time = 0 ; time < 2000.0 ; time += 100 )
     {
     interpolate( waypoints, time, &x_pos_interp, &y_pos_interp );
     printf( "at time %f x_pos %f y_pos %f\n", time, x_pos_interp, y_pos_interp );
     }
   */

  init_particle_mem( NUM_PARTICLES );
  init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );
  time_update( NUM_PARTICLES, 100.0, MEAN_MANEUVER_TIME );
  write_particles( OUTPUT_NAME, NUM_PARTICLES );
  print_particles( NUM_PARTICLES );
  
  for ( i = 0 ; i < NUM_PARTICLES - 1 ; i++ )
  {
    print_particle( i );
    print_particle( i + 1 );
    printf( "%f\n" , toDegrees( azimuth( x_pos[i], y_pos[i], x_pos[i+1], y_pos[i+1] ) ) );
  }
}

// allocate memory for num particles
void init_particle_mem( int num )
{
  x_pos  = ( float* ) malloc( sizeof( float ) * num );
  y_pos  = ( float* ) malloc( sizeof( float ) * num );
  x_vel  = ( float* ) malloc( sizeof( float ) * num );
  y_vel  = ( float* ) malloc( sizeof( float ) * num );
  weight = ( float* ) malloc( sizeof( float ) * num );
}

// initialize positions, velocities, and weights of num particles
void init_particle_val( int num, float max_range, float max_vel )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    x_pos[i]  = frand( -max_range, max_range );
    y_pos[i]  = frand( -max_range, max_range );
    x_vel[i]  = frand( -max_vel, max_vel );
    y_vel[i]  = frand( -max_vel, max_vel );
    weight[i] = 1;
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
    printf("Updating particle %d seconds %f\n", i, seconds);

    remaining_time = seconds;

    while ( 1 )
    {
      if ( next_maneuver < remaining_time )
      {
        printf("Particle %d maneuvering update time (next_maneuver) %f remaining_time %f\n", i, next_maneuver, remaining_time);

        time_update_index( i, next_maneuver );
        maneuver_index( i );
        remaining_time -= next_maneuver;
        next_maneuver = erand( mean_maneuver );
      }
      else
      {
        printf("Particle %d finished time update time %f\n", i, remaining_time);

        time_update_index( i, remaining_time );
        next_maneuver -= remaining_time;

        printf("next maneuver %f\n", next_maneuver);
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
  x_vel[i] = frand( -MAX_VEL, MAX_VEL );
  y_vel[i] = frand( -MAX_VEL, MAX_VEL );
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( char* out_name, int num )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
  }

  int success = fclose( file );
}

// prints information on all particles to the console
void print_particles( int num )
{
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    print_particle( i );
  }
}

// prints a single particle's information to the console
void print_particle( int i )
{
  printf( "%f\t%f\t%f\t%f\t%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
}
