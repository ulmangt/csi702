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
