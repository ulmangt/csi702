#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

int main( )
{
  // initialize random seed
  srand( time( NULL ) );

  int i;
  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    write_particles( get_particle_file_name( i ) , PARTICLES_PER_PROC );
  }
}


