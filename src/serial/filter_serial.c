#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_PARTICLES 1000
#define MAX_RANGE 8000
#define MAX_VEL 15
#define OUTPUT_NAME "particles.out"

float *x_pos; // meters
float *y_pos; // meters
float *x_vel; // meters/second
float *y_vel; // meters/second
float *weight;

float frand0( float );
float frand( float, float );

void init_particle_mem( int );
void init_particle_val( int, float, float );

void time_update( int, float );

void write_particles( char*, int );

int main( int argc, char* argv )
{
  init_particle_mem( NUM_PARTICLES );
  init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );
  time_update( NUM_PARTICLES, 20.0 );
  write_particles( OUTPUT_NAME, NUM_PARTICLES );
}

// return a random float value between 0 and max
float frand0( float max )
{
  return ( float ) rand( ) / ( float ) RAND_MAX * max ;
}

float frand( float min, float max )
{
  float diff = max - min;
  return frand0( diff ) + min;
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
void time_update( int num, float seconds )
{
  int i;  

  for ( i = 0 ; i < num ; i++ )
  {
    x_pos[i]  = x_pos[i] + x_vel[i] * seconds;
    y_pos[i]  = y_pos[i] + y_vel[i] * seconds;
  }
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
