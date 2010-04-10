#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "obs_math.h"
#include "filter_io.h"
#include "convert.h"

#include "cuda_test.h"
#include "cuda_test_functions.h"

#define NUM_PARTICLES 32



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

// prints a single particle's information to the console
void init_host_particles( struct particles *list, int num )
{
  int i;
  for ( i = 0 ; i < num ; i++ )
  {
    struct particles *part = list + i;
    part->x_pos = -999;
    part->y_pos = -999;
    part->x_vel = -999;
    part->y_vel = -999;
    part->weight = -999;
  }
}

int main( int argc, char** argv )
{
  printf("Hello CUDA 3\n");

  struct particles *d_particle_list = d_init_particle_mem( NUM_PARTICLES );
  struct particles *h_particle_list = h_init_particle_mem( NUM_PARTICLES );
  
  init_host_particles( h_particle_list, NUM_PARTICLES );

  h_init_seed( h_particle_list, NUM_PARTICLES );

  copy_particles_host_to_device( d_particle_list, h_particle_list, NUM_PARTICLES );

  init_particles( d_particle_list, NUM_PARTICLES );

  copy_particles_device_to_host( h_particle_list, d_particle_list, NUM_PARTICLES );

  print_particles( h_particle_list, NUM_PARTICLES, 1 );

  printf("Hello CUDA 2\n");
}
