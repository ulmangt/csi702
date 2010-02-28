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

void init_particle_mem( int );
void init_particle_val( int, float, float );
float frand( float );
void write_particles( char*, int );

int main( int argc, char* argv )
{
  init_particle_mem( NUM_PARTICLES );
  init_particle_val( NUM_PARTICLES, MAX_RANGE, MAX_VEL );
  write_particles( OUTPUT_NAME, NUM_PARTICLES );
}

// return a random float value between 0 and max
float frand( float max )
{
  return ( float ) rand( ) / ( float ) RAND_MAX * max ;
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
    x_pos[i]  = frand( max_range );
    y_pos[i]  = frand( max_range );
    x_vel[i]  = frand( max_vel );
    y_vel[i]  = frand( max_vel );
    weight[i] = 1;
  }
}

void write_particles( char* out_name, int num )
{
  FILE* file = fopen( out_name, "w" );
  
  int i;

  for ( i = 0 ; i < num ; i++ )
  {
    fprintf( file, "%f,%f,%f,%f,%f\n", x_pos[i], y_pos[i], x_vel[i], y_vel[i], weight[i] );
  }

  int success = fclose( file );
}
