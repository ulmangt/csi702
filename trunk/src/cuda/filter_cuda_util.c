#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "obs_math.h"
#include "filter_io.h"
#include "convert.h"

#include "filter_cuda_kernels.h"
#include "filter_cuda_data.h"


// prints a single particle's information to the console
void print_particle( float x_pos, float y_pos, float x_vel, float y_vel, float weight, float seed )
{
  printf( "%f\t%f\t%f\t%f\t%f\t%f\n", x_pos, y_pos, x_vel, y_vel, weight, seed );
}

// prints information on all particles to the console
void print_particles( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, int num, int downsample )
{
  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    print_particle( d_x_pos[i], d_y_pos[i], d_x_vel[i], d_y_vel[i], d_weight[i], d_seed[i] );
  }
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( float *d_x_pos, float *d_y_pos, float *d_x_vel, float *d_y_vel, float *d_weight, float *d_seed, char* out_name, int num, int downsample )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", d_x_pos[i], d_y_pos[i], d_x_vel[i], d_y_vel[i], d_weight[i] );
  }

  int success = fclose( file );
}
