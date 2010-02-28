#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_PARTICLES 10000
#define MAX_RANGE 8000
#define MAX_VEL 15

#define MEAN_MANEUVER_TIME 3600

#define OUTPUT_NAME "particles.out"


float *x_pos; // meters
float *y_pos; // meters
float *x_vel; // meters/second
float *y_vel; // meters/second
float *weight;

float frand0( float );
float frand( float, float );
float erand( float );

void init_particle_mem( int );
void init_particle_val( int, float, float );

void time_update( int, float, float );
void time_update_index( int, float );
void maneuver_index( int );

void write_particles( char*, int );

int main( int argc, char* argv )
{
  init_particle_mem( NUM_PARTICLES );
  init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );
  time_update( NUM_PARTICLES, 100.0, MEAN_MANEUVER_TIME );
  write_particles( OUTPUT_NAME, NUM_PARTICLES );
}

// return a random float value evenly distributed between 0 and max
float frand0( float max )
{
  return ( float ) rand( ) / ( float ) RAND_MAX * max ;
}

// return a random float value evenly distributed between min and max
float frand( float min, float max )
{
  float diff = max - min;
  return frand0( diff ) + min;
}

// return an exponentially distributed random float
float erand( float inv_lambda )
{
  return -log( frand0( 1.0 ) ) * inv_lambda;
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
