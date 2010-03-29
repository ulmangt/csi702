
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "nbody_util.h"

// generate a random double between 0 and 1
float frand( )
{
  return ( float ) rand( ) / ( float ) RAND_MAX ;
}

// calculates the distance between two particles
double distance( double x1, double y1, double z1, double x2, double y2, double z2 )
{
  return sqrt( pow( x1 - x2 , 2.0 ) + pow( y1 - y2 , 2.0 ) + pow( z1 - z2 , 2.0 ) ) ;
}

// returns the name of the particle data file for the num process
char *get_particle_file_name( int num )
{
  return get_file_name( "particle", num );
}

char *get_file_name( char* name_prefix, int num )
{
  char *file_name = malloc( sizeof(char) * 100 );
  sprintf( file_name, "%s%d", name_prefix, num );
  return file_name;
}

// writes the particles in the provided arrays to file_name
void write_potentials( char *file_name, int num_particles, double *potential )
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
void load_particles( char *file_name, double *x_pos, double *y_pos, double *z_pos )
{
  float x;
  float y;
  float z;

  FILE *file = fopen( file_name, "r" );

  int count;
  for ( count = 0 ; count < PARTICLES_PER_PROC ; count++ )
  {
    int success = fscanf( file, "%f%f%f" , &x, &y, &z );

    *x_pos++ = (double) x;
    *y_pos++ = (double) y;
    *z_pos++ = (double) z; 

    if ( success == EOF )
      break;
  }

  close( file );
}
