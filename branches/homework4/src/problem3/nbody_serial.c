
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

#define PARTICLES_PER_PROC 100
#define NUM_PROC 10

int main( )
{
  int i;

  float **x_proc = malloc( sizeof( float* ) * NUM_PROC );
  float **y_proc = malloc( sizeof( float* ) * NUM_PROC );
  float **z_proc = malloc( sizeof( float* ) * NUM_PROC );

  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    load_particles( get_particle_file_name( i ), *(x_proc+i), *(y_proc+i), *(z_proc+i) );
  }
}
