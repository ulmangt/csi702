
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

#define PARTICLES_PER_PROC 10
#define NUM_PROC 10

int main( )
{
  int i;

  float **x_proc = malloc( sizeof( float* ) * NUM_PROC );
  float **y_proc = malloc( sizeof( float* ) * NUM_PROC );
  float **z_proc = malloc( sizeof( float* ) * NUM_PROC );

  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    load_particles( get_particle_file_name( i ), x_proc+i, y_proc+i, z_proc+i );
  }

  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    printf( "file: %s\n" , get_particle_file_name( i ) );

    float *x_pos = *( x_proc + i );
    float *y_pos = *( y_proc + i );
    float *z_pos = *( z_proc + i );

    int j;
    for ( j = 0 ; j < PARTICLES_PER_PROC ; j++ )
    {
      printf( "%f %f %f\n", x_pos[j], y_pos[j], z_pos[j] );
    }
  }

}
