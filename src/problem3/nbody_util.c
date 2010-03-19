
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

// generate a random float between 0 and 1
float frand( )
{
  return ( float ) rand( ) / ( float ) RAND_MAX ;
}

char *get_particle_file_name( int num )
{
  char *file_name = malloc( sizeof(char) * 100 );
  sprintf( file_name, "particle%d", num );
  return file_name;
}

void write_particles( char *file_name, int num_particles )
{
  FILE *file = fopen( file_name, "w" );

  int count;

  for ( count = 0 ; count < num_particles ; count++ )
  {
    fprintf( file, "%f %f %f\n", frand( ), frand( ), frand( ) );
  }
  
  close( file );
}

/*
void load_particles( char *file_name, float **x_pos, float **y_pos, float **z_pos )
{
  FILE *file = fopen( file_name, "r" );
  
  int total, count;
  int success = fscanf( file, "%d", &total );

  if ( success == EOF ) {
    close( file );
    return;
  }

  *x_pos = malloc( sizeof( float ) * total );
  *y_pos = malloc( sizeof( float ) * total );
  *z_pos = malloc( sizeof( float ) * total );

  for ( count = 0 ; count < total ; count++ )
  {
    int success = fscanf( file, "%f%f%f" , (*x_pos) + count, (*y_pos) + count, (*z_pos) + count );

    if ( success == EOF )
      break;
  }

  close( file );
}
*/

void load_particles( char *file_name, float *x_pos, float *y_pos, float *z_pos )
{
  FILE *file = fopen( file_name, "r" );

  int count;
  for ( count = 0 ; count < PARTICLES_PER_PROC ; count++ )
  {
    int success = fscanf( file, "%f%f%f" , x_pos++, y_pos++, z_pos++ );

    if ( success == EOF )
      break;
  }

  close( file );
}
