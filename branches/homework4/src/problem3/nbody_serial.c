
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

int main( )
{
  int i;

  float *x_proc = malloc( sizeof( float ) * NUM_PROC * PARTICLES_PER_PROC );
  float *y_proc = malloc( sizeof( float ) * NUM_PROC * PARTICLES_PER_PROC );
  float *z_proc = malloc( sizeof( float ) * NUM_PROC * PARTICLES_PER_PROC );

  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    int shift = i * PARTICLES_PER_PROC;
    load_particles( get_particle_file_name( i ), x_proc + shift , y_proc + shift, z_proc + shift );
  }

  for ( i = 0 ; i < NUM_PROC * PARTICLES_PER_PROC ; i++ )
  {
    printf("%d %f %f %f\n", i, x_proc[i], y_proc[i], z_proc[i]);
  }
}


