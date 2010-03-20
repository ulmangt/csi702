
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

// generate a random float between 0 and 1
float frand( )
{
  return ( float ) rand( ) / ( float ) RAND_MAX ;
}

// calculates the distance between two particles
float distance( float x1, float y1, float z1, float x2, float y2, float z2 )
{
  return sqrt( pow( x1 - x2 , 2.0 ) + pow( y1 - y2 , 2.0 ) + pow( z1 - z2 , 2.0 ) ) ;
}

// returns the name of the particle data file for the num process
char *get_particle_file_name( int num )
{
  char *file_name = malloc( sizeof(char) * 100 );
  sprintf( file_name, "particle%d", num );
  return file_name;
}

// writes the particles in the provided arrays to file_name
void write_potentials( char *file_name, int num_particles, float *potential )
{
  FILE *file = fopen( file_name, "w" );

  int count;

  for ( count = 0 ; count < num_particles ; count++ )
  {
    fprintf( file, "%d %f\n", count, potential[count] );
  }
  
  close( file );
}


// writes a random set of num_particles to file_name
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

// loads PARTICLES_PER_PROC particles from file_name into the provided arrays
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
