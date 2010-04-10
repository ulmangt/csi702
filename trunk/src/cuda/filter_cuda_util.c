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
void print_particle( struct particles *part )
{
  printf( "%f\t%f\t%f\t%f\t%f\t%f\n", part->x_pos, part->y_pos, part->x_vel, part->y_vel, part->weight, part->seed );
}

// prints information on all particles to the console
void print_particles( struct particles *list, int num, int downsample )
{
  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    print_particle( list+i );
  }
}

// writes the current set of particles out to disk as
// a tab delimited text file
void write_particles( struct particles *list, char* out_name, int num, int downsample )
{
  FILE* file = fopen( out_name, "w" );

  int i;

  for ( i = 0 ; i < num ; i = i + downsample )
  {
    struct particles *part = list + i;
    fprintf( file, "%f\t%f\t%f\t%f\t%f\n", part->x_pos, part->y_pos, part->x_vel, part->y_vel, part->weight );
  }

  int success = fclose( file );
}
