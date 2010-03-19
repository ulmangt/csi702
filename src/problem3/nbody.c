#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NUM_PARTICLES 100

float frand( );
void write_particles( char *, int );
void load_particles ( char*, float*, float*, float* );

int main( )
{
  // initialize random seed
  srand( time( NULL ) );

  write_particles( "particle0", NUM_PARTICLES ); 
}

// generate a random float between 0 and 1
float frand( )
{
  return ( float ) rand( ) / ( float ) RAND_MAX ;
}

void write_particles( char *file_name, int num_particles )
{
  FILE *file = fopen( file_name, "w" );

  int count;

  // first line of the file contains the total number of particles
  fprintf( file, "%d\n", num_particles );

  for ( count = 0 ; count < num_particles ; count++ )
  {
    fprintf( file, "%f %f %f\n", frand( ), frand( ), frand( ) );
  }
  
  close( file );
}

void load_particles( char *file_name, float *x_pos, float *y_pos, float *z_pos )
{
  FILE *file = fopen( file_name, "r" );
  
  int total, count;
  int success = fscanf( file, "%d", &total );

  if ( success == EOF ) {
    close( file );
    return;
  }

  x_pos = malloc( sizeof( float ) * total );
  y_pos = malloc( sizeof( float ) * total );
  z_pos = malloc( sizeof( float ) * total );

  for ( count = 0 ; count < total ; count++ )
  {
    int success = fscanf( file, "%f%f%f" , x_pos++, y_pos++, z_pos++ );

    if ( success == EOF )
      break;
  }

  close( file );
}


