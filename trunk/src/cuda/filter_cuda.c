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
#include "filter_cuda_util.h"


#define NUM_PARTICLES 32

int main( int argc, char** argv )
{
  printf("Hello CUDA 3\n");

  struct particles *d_particle_list = d_init_particle_mem( NUM_PARTICLES );
  struct particles *h_particle_list = h_init_particle_mem( NUM_PARTICLES );

  h_init_seed( h_particle_list, NUM_PARTICLES );

  copy_particles_host_to_device( d_particle_list, h_particle_list, NUM_PARTICLES );

  init_particles( d_particle_list, NUM_PARTICLES );

  copy_particles_device_to_host( h_particle_list, d_particle_list, NUM_PARTICLES );

  print_particles( h_particle_list, NUM_PARTICLES, 1 );

  printf("Hello CUDA 2\n");
}
