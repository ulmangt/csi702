#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

#define PARTICLES_PER_PROC 100
#define NUM_PROC 10

float frand( );
void write_particles( char *, int );
void load_particles ( char*, float*, float*, float* );

int main( )
{
  // initialize random seed
  srand( time( NULL ) );

  char file_name[100];

  int i;
  for ( i = 0 ; i < NUM_PROC ; i++ )
  {
    sprintf( file_name, "particle%d", i );
    write_particles( file_name, PARTICLES_PER_PROC );
  }
}


