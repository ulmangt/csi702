
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

int main( )
{
  int i, j;

  int total = NUM_PROC * PARTICLES_PER_PROC;

  // allocates space for all the particles
  float *x_proc = malloc( sizeof( float ) * total );
  float *y_proc = malloc( sizeof( float ) * total );
  float *z_proc = malloc( sizeof( float ) * total );
  float *potential = malloc( sizeof( float ) * total );

  // load particles from NUM_PROC files into the _proc arrays
  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    int shift = i * PARTICLES_PER_PROC;
    load_particles( get_particle_file_name( i ), x_proc + shift , y_proc + shift, z_proc + shift );
  }

  for ( i = 0 ; i < total ; i++ )
  {
    potential[i] = 0.0;

    for( j = 0 ; j < total ; j++ )
    {
      if ( i != j )
      {
        float R_ij = distance( x_proc[i], x_proc[j], y_proc[i], y_proc[j], z_proc[i], z_proc[j] );
        // G, M[i], and M[j] are all assumed to be 1
        potential[i] = potential[i] + 1.0 / R_ij;
      }
    }
  }

  // calculate particle with maximum and minimum potential
  float max_potential = potential[0];
  float min_potential = potential[0];
  for ( i = 0 ; i < total ; i++ )
  {
    if ( max_potential < potential[i] )
      max_potential = potential[i];

    if ( min_potential > potential[i] )
      min_potential = potential[i];
  }

  printf( "max potential: %f\n", max_potential );
  printf( "min potential: %f\n", min_potential );

  // write all particles to file
  write_potentials( "potential", total, potential );
}


